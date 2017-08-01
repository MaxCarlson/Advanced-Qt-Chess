#include "ai_logic.h"

#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "externs.h"

//#include "move.h"
#include "evaluatebb.h"
#include "bitboards.h"
#include "movegen.h"
#include "zobristh.h"
//#include "bitboards.cpp"

#include "hashentry.h"

//holds historys and killers + eventually nodes searched + other data
searchDriver sd;

//principal variation array
Move pVArr[29];

//constructed once so as to pass to eval where constructing was slow
//for quiet search
MoveGen evalGenMoves;

//value to determine if time for search has run out
extern bool searchCutoff;
bool searchCutoff = false;

const int ASPIRATION = 50;

//master bitboard for turn
BitBoards newBoard;
//master zobrist for turn
ZobristH zobrist;

Ai_Logic::Ai_Logic()
{

}

Move Ai_Logic::iterativeDeep(int depth)
{
    //read array and construct the bitboards to match
    newBoard.constructBoards();
    //generate accurate zobrist key based on bitboards
    zobrist.getZobristHash(newBoard);
    //ai is always black atm, if it wasn't update color only applied on black turn
    zobrist.UpdateColor();

    //iterative deepening start time
    clock_t IDTimeS = clock();

    //time limit in miliseconds
    int timeLimmit = 100009999, currentDepth = 0;
    long endTime = IDTimeS + timeLimmit;

    //best overall move as calced
    Move bestMove;
    int distance = 1, bestScore, alpha = -1000000, beta = 1000000;
    //search has not run out of time
    searchCutoff = false;

    std::cout << zobrist.zobristKey << std::endl;

    //iterative deepening loop starts at depth 1, iterates up till max depth or time cutoff
    while(distance <= depth && IDTimeS < endTime){
        clock_t currentTime = clock();

        //if we're past 2/3's of our time limit, stop searching
        if(currentTime >= endTime - (1/3 * timeLimmit)){
            break;
        }

        //main search
        bestScore = alphaBeta(distance, alpha, beta, false, currentTime, timeLimmit, currentDepth +1, true);

   /*
        //aspiration window correction
        if (bestScore <= alpha || bestScore >= beta) {
            alpha = -100000;    // We fell outside the window, so try again with a
            beta = 100000;      //  full-width window (and the same depth).
            continue;
        }
        //if we don't fall out of window, set alpha and beta to a window size to narrow search
        alpha = bestScore - ASPIRATION;
        beta = bestScore + ASPIRATION;
   */
        //if the search is not cutoff
        if(!searchCutoff){

            //grab best move out of PV array
            bestMove = pVArr[distance];

        }
        //increment distance to travel (same as depth at max depth)
        distance++;
    }


    std::cout << std::endl;

    //make final move on bitboards + draw
    newBoard.makeMove(bestMove, zobrist, false);
    newBoard.drawBBA();

    clock_t IDTimeE = clock();
    //postion count and time it took to find move
    std::cout << positionCount << " positions searched." << std::endl;
    std::cout << (double) (IDTimeE - IDTimeS) / CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Depth of " << distance-1 << " reached."<<std::endl;
    std::cout << qCount << " non-quiet positions searched."<< std::endl;

    //decrease history values after a search
    ageHistorys();

    return bestMove;
}


int Ai_Logic::alphaBeta(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, bool allowNull)
{

    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;
    bool FlagInCheck = false;
    int queitSD = 7, f_prune = 0;

    //create unqiue hash from zobrist key
    int hash = (int)(zobrist.zobristKey % 15485843);
    HashEntry entry = transpositionT[hash];

    //if the depth of the stored evaluation is greater and the zobrist key matches
    //don't return eval on root node
    if(entry.depth >= depth && entry.zobrist == zobrist.zobristKey){
        //return either the eval, the beta, or the alpha depending on flag
        switch(entry.flag){
            case 1:
            if(entry.eval <= alpha){
                return alpha;
            }
            break;
            case 2:
            if(entry.eval >= beta){
                return beta;
            }
            break;            
            case 3:
            if(entry.flag == 3){
                return entry.eval;
            }
            break;
        }

    }

    //if the time limmit has been exceeded set stop search flag
    if(elapsedTime >= timeLimmit){
        searchCutoff = true;
    }

    int score;
    if(depth < 1 || searchCutoff){
        //run capture search to max depth of queitSD
        score = quiescent(alpha, beta, isWhite, currentDepth, queitSD, currentTime, timeLimmit);
        return score;
    }

    MoveGen gen_moves;
    //grab bitboards from newBoard object and store color and board to var
    gen_moves.grab_boards(newBoard, isWhite);

    //are we in check?
    U64 king, eking;
    if(isWhite){ king = newBoard.BBWhiteKing; eking = newBoard.BBBlackKing; }
    else { king = newBoard.BBBlackKing; eking = newBoard.BBWhiteKing; }

    FlagInCheck = gen_moves.isAttacked(king, isWhite);
/*
//eval pruning / static null move
    if(depth < 3 && !FlagInCheck && abs(beta - 1) > -100000 + 100){
        int static_eval = eval->evalBoard(isWhite, newBoard, zobrist);
        int eval_margin = 120 * depth;
        if(static_eval - eval_margin >= beta){
            return static_eval - eval_margin;
        }
    }
*/

//Null move heuristics, disabled if in check
    if(allowNull && depth > depthR && currentDepth > 1 && !FlagInCheck){ // ??
        if(depth > 6) depthR = 3;

        score = nullMoves(depth-1-depthR, -beta, -beta+1, !isWhite, currentTime, timeLimmit, currentDepth+1);
        //if after getting a free move the score is too good, prune this branch
        if(score >= beta){
            return score;
        }
    }

//razoring
    if(!FlagInCheck && allowNull && depth <= 3){
        int threshold = alpha - 300 - (depth - 1) * 60;
        evaluateBB eval;
        if(eval.evalBoard(isWhite, newBoard, zobrist) < threshold){
            score = quiescent(alpha, beta, isWhite, currentDepth, queitSD, currentTime, timeLimmit);

            if(score < threshold) return alpha;
        }
    }
/*
//do we want to futility prune?
    int fmargin[4] = { 0, 200, 300, 500 };
    if(depth <= 3 && !FlagInCheck && abs(alpha) < 9000 && eval->evalBoard(isWhite, newBoard, zobrist) + fmargin[depth] <= alpha){
        f_prune = 1;
    }
*/

//generate psuedo legal moves
    gen_moves.generatePsMoves(false);

    //add killers scores to moves if there are any
    gen_moves.reorderMoves(currentDepth, entry);

    int hashFlag = 1, movesNum = gen_moves.moveCount, legalMoves = 0;

//search through psuedo legal moves checking against king safety
    Move hashMove;
    for(int i = 0; i < movesNum; ++i){
        //grab best scoring move
        Move newMove = gen_moves.movegen_sort(currentDepth);

        //make move on BB's store data to string so move can be undone
        newBoard.makeMove(newMove, zobrist, isWhite);

        //is move legal? if not skip it
        if(gen_moves.isAttacked(king, isWhite)){
            newBoard.unmakeMove(newMove, zobrist, isWhite);
            continue;
        }
        positionCount ++;
        legalMoves ++;
        /*
        //futility pruning ~~ is not a promotion, is not a capture, and does not give check
        if(f_prune && i > 0 && newMove.flag != 'Q' && newMove.captured == '0'){ //&& !gen_moves.isAttacked(eking, !isWhite)){
            newBoard.unmakeMove(newMove, zobrist, isWhite);
            continue;
        }*/

        //jump to other color and evaluate all moves that don't cause a cutoff if depth is greater than 1
        score = -alphaBeta(depth-1, -beta, -alpha,  ! isWhite, currentTime, timeLimmit, currentDepth +1, true);

        //undo move on BB's
        newBoard.unmakeMove(newMove, zobrist, isWhite);

        if(score > alpha){

            //store the principal variation
            pVArr[depth] = newMove;

            //if move causes a beta cutoff stop searching current branch
            if(score >= beta){

                //add beta to transpositon table with beta flag
                addMoveTT(newMove, depth, beta, 2);

                if(newMove.captured == '0' && newMove.flag != 'Q'){
                    //add move to killers
                    //addKiller(newMove, currentDepth);

                    int sPos = newMove.y * 8 + newMove.x, ePos = newMove.y1 * 8 + newMove.x1;
                    //add score to historys
                    sd.history[isWhite][sPos][ePos] += depth * depth;

                    //don't want historys to overflow if search is really big
                    if(sd.history[isWhite][sPos][ePos] > SORT_KILL){
                        for(int i = 0; i < 64; i++){
                            for(int i = 0; i < 64; i++){
                                sd.history[isWhite][sPos][ePos] /= 2;
                            }
                        }
                    }

                }


                return beta;
            }
            //new best move
            alpha = score;

            //if we've gained a new alpha set hash Flag equal to exact and store best move
            hashFlag = 3;
            hashMove = newMove;
        }
    }
/*
    //return mate/stalemate score if there are no more moves ~~ greater depth less score
    if(legalMoves == 0){
       return eval->returnMateScore(isWhite, newBoard, depth);
    }
*/
    //add alpha eval to hash table don't save a real move
    addMoveTT(hashMove, depth, alpha, hashFlag);

    return alpha;
}

int Ai_Logic::nullMoves(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth)
{
    //update key color
    zobrist.UpdateColor();
    //as well as indicate to transposition tables these are null move boards
    //zobrist.UpdateNull();

    int score = -alphaBeta(depth, alpha, beta, isWhite, currentTime, timeLimmit, currentDepth, false);
    //int score = -PVS(depth, alpha, beta, isWhite, currentTime, timeLimmit, currentDepth, false, newBoard, zobrist, eval);

    //zobrist.UpdateNull();
    zobrist.UpdateColor();
    return score;
}

int Ai_Logic::quiescent(int alpha, int beta, bool isWhite, int currentDepth, int quietDepth, long currentTime, long timeLimmit)
{
    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;

    //create unqiue hash from zobrist key
    int hash = (int)(zobrist.zobristKey % 15485843);
    HashEntry entry = transpositionT[hash];

    //if the time limmit has been exceeded finish searching
    if(elapsedTime >= timeLimmit){
        searchCutoff = true;
    }

    evaluateBB eval;
    //evaluate board position (if curentDepth is even return -eval)
    int standingPat = eval.evalBoard(isWhite, newBoard, zobrist);

    if(quietDepth <= 0 || searchCutoff){
        return standingPat;
    }

    if(standingPat >= beta){
       return beta;
    }

    if(alpha < standingPat){
       alpha = standingPat;
    }

    //generate only captures with true capture gen var
    MoveGen gen_moves;
    gen_moves.grab_boards(newBoard, isWhite);
    gen_moves.generatePsMoves(true);
    gen_moves.reorderMoves(currentDepth, entry);

    int score;
    //set hash flag equal to alpha Flag
    int hashFlag = 1, moveNum = gen_moves.moveCount;


    U64 king;
    Move hashMove;
    if(isWhite) king = newBoard.BBWhiteKing;
    else king = newBoard.BBBlackKing;

    for(int i = 0; i < moveNum; ++i)
    {        
        Move newMove = gen_moves.movegen_sort(currentDepth);

        if(gen_moves.isAttacked(king, isWhite)){
            continue;
        }
        qCount ++;
        /*
        // ~~~NEEDS WORK + Testing + stop pruning in end game
        if(deltaPruning(tempMove, standingPat, isWhite, alpha, false, newBoard)){
            continue; //uncomment to enable delta pruning!!!
        }

        //bad capture pruning
        if(badCapture(tempMove, isWhite, newBoard) && tempMove[3] != 'Q'){
            continue;
        }
        */
        newBoard.makeMove(newMove, zobrist, isWhite);
        /*
        if(newBoard.isAttacked(isWhite, king)){
            newBoard.unmakeMove(unmake, zobrist);
            continue;
        }
        */
        score = -quiescent(-beta, -alpha, ! isWhite, currentDepth+1, quietDepth-1, currentTime, timeLimmit);

        newBoard.unmakeMove(newMove, zobrist, isWhite);

        if(score > alpha){

            if(score >= beta){
                if(newMove.captured == '0' && newMove.flag != 'Q'){
                    //add move to killers
                    addKiller(newMove, currentDepth);
/*
                    int sPos = newMove.y * 8 + newMove.x, ePos = newMove.y1 * 8 + newMove.x1;
                    //add score to historys
                    history[sPos][ePos].val += depth * depth;

                    //don't want historys to overflow if search is really big
                    if(history[sPos][ePos].val > SORT_KILL){
                        for(int i = 0; i < 64; i++){
                            for(int i = 0; i < 64; i++){
                                historys[sPos][ePos] /= 2;
                            }
                        }
                    }
                    */
                }

                return beta;
            }

           alpha = score;
           //if we've gained a new alpha set hash Flag to exact
           hashFlag = 3;
           hashMove = newMove;
        }
    }
    //add alpha eval to hash table
    //addTTQuiet(hashMove, currentDepth, alpha, hashFlag, zobrist);
    return alpha;
}

void Ai_Logic::addKiller(Move move, int ply)
{
    if(move.captured == '0'){ //if move isn't a capture save it as a killer
        //make sure killer is different
        if((move.x != sd.killers[ply][0].x && move.y != sd.killers[ply][0].y)
        || (move.x1 != sd.killers[ply][0].x1 && move.y1 != sd.killers[ply][0].y1)){
            //save primary killer to secondary slot
            sd.killers[ply][1] = sd.killers[ply][0];
        }
        //save primary killer
        sd.killers[ply][0] = move;
    }
}

void Ai_Logic::ageHistorys()
{
    //used to decrease value after a search
    for (int cl = 0; cl < 2; cl++)
        for (int i = 0; i < 64; i++)
            for (int j = 0; j < 64; j++) {
                sd.history[cl][i][j] = sd.history[cl][i][j] / 8;

            }
}

void Ai_Logic::addMoveTT(Move move, int depth, int eval, int flag)
{
    //get hash of current zobrist key
    int hash = (int)(zobrist.zobristKey % 15485843);
    //if the depth of the current move is greater than the one it's replacing or if it's older than
    if(depth >= transpositionT[hash].depth || transpositionT[hash].ancient < turns - 5){
        //add position to the table
        transpositionT[hash].zobrist = zobrist.zobristKey;
        transpositionT[hash].depth = depth;
        transpositionT[hash].eval = eval;
        transpositionT[hash].flag = flag;
        transpositionT[hash].move = move;
        transpositionT[hash].ancient = turns;

    }

}

/*
bool Ai_Logic::deltaPruning(std::string move, int eval, bool isWhite, int alpha, bool isEndGame, MoveGen *newBoard)
{
    //if is end game, search all nodes
    if(isEndGame){
        return false;
    }

    U64 epawns, eknights, ebishops, erooks, equeens;
    //set enemy bitboards
    if(isWhite){
        //enemies
        epawns = newBoard.BBBlackPawns;
        eknights = newBoard.BBBlackKnights;
        ebishops = newBoard.BBBlackBishops;
        erooks = newBoard.BBBlackRooks;
        equeens = newBoard.BBBlackQueens;
    } else {
        epawns = newBoard.BBWhitePawns;
        eknights = newBoard.BBWhiteKnights;
        ebishops = newBoard.BBWhiteBishops;
        erooks = newBoard.BBWhiteRooks;
        equeens = newBoard.BBWhiteQueens;
    }



    U64 pieceMaskE = 0LL;
    int x, y, xyE;
    int score = 0, Delta = 200;
    //normal captures
    if(move[3] != 'Q'){
        x = move[2]-0; y = move[3]-0;

    } else {
        //pawn promotions
        score = 800;
        //forward non capture
        if(move[2] == 'F'){
            x = move[0];
        //capture
        } else {
            x = move[2]-0;
        }

        if(isWhite){
            y = 0;
        } else {
            y = 7;
        }
    }

    xyE = y*8+x;
    //create mask of move end position
    pieceMaskE += 1LL << xyE;

    //find whice piece is captured
    if(pieceMaskE & epawns){
        score += 100;
    } else if(pieceMaskE & eknights){
        score += 320;
    } else if(pieceMaskE & ebishops){
        score += 330;
    } else if(pieceMaskE & erooks){
        score += 500;
    } else if(pieceMaskE & equeens){
        score += 900;
    }

    //if score plus safety margin is less then alpha, don't bother searching node
    if(eval + score + Delta < alpha){
        return true;
    }

    return false;

}
*/

