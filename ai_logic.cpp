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
#include "bitboards.cpp"




//value to determine if time for search has run out
extern bool searchCutoff;
bool searchCutoff = false;

const int ASPIRATION = 50;

Ai_Logic::Ai_Logic()
{

}
/*
struct helperThreads {

    std::thread helpers[8];
    ZobristH zobrists[8];
    BitBoards BBs[8];
    evaluateBB eval[8];
    Ai_Logic searches[8];
    unsigned threads;

    helperThreads(int numThreads, bool isWhite){
        //threads = std::thread::hardware_concurrency();
        threads = numThreads;

        for(int i = 0; i < numThreads; i++){
            //construct thread number of boards
            BBs[i].constructBoards();
            //construct thread number of zobrist keys
            zobrists[i].getZobristHash(&BBs[i]);
            //if ai is not white XOR zobrist key with black color key
            if(!isWhite){
                zobrists[i].UpdateColor();
            }

        }

    }

    void sychThreads(int distance, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth){
        //start thread number of threads searching alpha beta
        for(int i = 0; i < threads; i++){
            //distance += (int)(i/2);
            helpers[i] = std::thread(&searches[i].alphaBeta, searches[i], distance, alpha, beta, isWhite, currentTime, timeLimmit, currentDepth+1, true, &BBs[i], &zobrists[i], &eval[i]);

        }
    }

    void joinThreads(bool stopSearch){
        //set searchcutoff extern to true so that all threads stop searching once main search is done
        searchCutoff = true;

        //join all active threads
        for(int i = 0; i < threads; i++){
            helpers[i].join();
        }
        //reset search cutoff to it's real value
        if(stopSearch){
            searchCutoff = true;
        } else {
            searchCutoff = false;
        }
    }

};
*/
Move Ai_Logic::iterativeDeep(int depth)
{

    //master bitboard for turn
    BitBoards *newBoard = new BitBoards;
    newBoard->constructBoards();

    //master zobrist object for ai turn
    ZobristH *mZobrist = new ZobristH;    
    mZobrist->getZobristHash(newBoard);
    mZobrist->UpdateColor();

    //master evaluation object - evaluates board position and gives an int value (- for black)
    evaluateBB *mEval = new evaluateBB;

    //iterative deepening start time
    clock_t IDTimeS = clock();

    //time limit in miliseconds
    int timeLimmit = 100009999, currentDepth = 0;
    long endTime = IDTimeS + timeLimmit;

    //create helper threads object for lazy SMP
    //helperThreads *threads = new helperThreads(3, false);

    //best overall move as calced
    Move bestMove;
    int distance = 1, bestScore, alpha = -1000000, beta = 1000000;
    //search has not run out of time
    searchCutoff = false;

    std::cout << mZobrist->zobristKey << std::endl;

    //iterative deepening loop starts at depth 1, iterates up till max depth or time cutoff
    while(distance <= depth && IDTimeS < endTime){
        clock_t currentTime = clock();

        //if we're past 2/3's of our time limit, stop searching
        if(currentTime >= endTime - (1/3 * timeLimmit)){
            break;
        }

        //start lazy SMP helper threads ~~ they fill up hash table so main search can get more hits off it
        //threads->sychThreads(distance, alpha, beta, false, currentTime, timeLimmit, currentDepth +1);

        //main search
        bestScore = alphaBeta(distance, alpha, beta, false, currentTime, timeLimmit, currentDepth +1, true, newBoard, mZobrist, mEval);

        //temporarily set searchcutoff to true, stop and join threads, reset search cutoff to it's prior value
        //threads->joinThreads(searchCutoff);
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
    newBoard->makeMove(bestMove, mZobrist, false);
    newBoard->drawBBA();


    clock_t IDTimeE = clock();
    //postion count and time it took to find move
    std::cout << positionCount << " positions searched." << std::endl;
    std::cout << (double) (IDTimeE - IDTimeS) / CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Depth of " << distance-1 << " reached."<<std::endl;
    std::cout << qCount << " non-quiet positions searched."<< std::endl;

    delete mZobrist;
    delete newBoard;
    delete mEval;

    return bestMove;
}


int Ai_Logic::alphaBeta(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, bool allowNull, BitBoards *BBBoard, ZobristH *zobrist, evaluateBB *eval)
{

    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;
    bool FlagInCheck = false;
    int queitSD = 11, f_prune = 0;

    //create unqiue hash from zobrist key
    int hash = (int)(zobrist->zobristKey % 15485843);
    HashEntry entry = transpositionT[hash];

    //if the depth of the stored evaluation is greater and the zobrist key matches
    //don't return eval on root node
    if(entry.depth >= depth && entry.zobrist == zobrist->zobristKey){
        //return either the eval, the beta, or the alpha depending on flag
        switch(entry.flag){
            case 3:
            if(entry.flag == 3){
                return entry.eval;
            }
            break;
            case 2:
            if(entry.eval >= beta){
                return beta;
            }
            break;
            case 1:
            if(entry.eval <= alpha){
                return alpha;
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
        score = quiescent(alpha, beta, isWhite, currentDepth, queitSD, currentTime, timeLimmit, BBBoard, zobrist, eval);
        //score = eval->evalBoard(isWhite, BBBoard, zobrist);

        //add move to hash table with exact flag
        addMoveTT("0", depth, score, 3, zobrist);

        return score;
    }


    MoveGen gen_moves;
    gen_moves.grab_boards(BBBoard); //grab bitboards from BBBoard object and stor to var

    //are we in check?
    U64 king, eking;
    if(isWhite){ king = BBBoard->BBWhiteKing; eking = BBBoard->BBBlackKing; }
    else { king = BBBoard->BBBlackKing; eking = BBBoard->BBWhiteKing; }

    FlagInCheck = gen_moves.isAttacked(king, isWhite);

//eval pruning / static null move
    if(depth < 3 && !FlagInCheck && abs(beta - 1) > -100000 + 100){
        int static_eval = eval->evalBoard(isWhite, BBBoard, zobrist);
        int eval_margin = 120 * depth;
        if(static_eval - eval_margin >= beta){
            return static_eval - eval_margin;
        }
    }


//Null move heuristics, disabled if in check
    if(allowNull && depth > depthR && currentDepth > 1 && !FlagInCheck){ // ??
        if(depth > 6) depthR = 3;

        score = nullMoves(depth-1-depthR, -beta, -beta+1, !isWhite, currentTime, timeLimmit, currentDepth+1, BBBoard, zobrist, eval);
        //if after getting a free move the score is too good, prune this branch
        if(score >= beta){
            return score;
        }
    }

//razoring
    if(!FlagInCheck && allowNull && depth <= 3){
        int threshold = alpha - 300 - (depth - 1) * 60;

        if(eval->evalBoard(isWhite, BBBoard, zobrist) < threshold){
            score = quiescent(alpha, beta, isWhite, currentDepth, queitSD, currentTime, timeLimmit, BBBoard, zobrist, eval);

            if(score < threshold) return alpha;
        }
    }

//do we want to futility prune?
    int fmargin[4] = { 0, 200, 300, 500 };
    if(depth <= 3 && !FlagInCheck && abs(alpha) < 9000 && eval->evalBoard(isWhite, BBBoard, zobrist) + fmargin[depth] <= alpha){
        f_prune = 1;
    }


//generate psuedo legal moves
    gen_moves.generatePsMoves(isWhite, false, currentDepth);

    int hashFlag = 1, movesNum = gen_moves.moveCount, legalMoves = 0;

//search through psuedo legal moves checking against king safety
    std::string tempMove, hashMove;
    for(int i = 0; i < movesNum; ++i){
        positionCount ++;
        //change board accoriding to i possible move
        tempMove = "";
        //grab best scoring move
        Move newMove = gen_moves.movegen_sort(currentDepth);

        tempMove += newMove.x;
        tempMove += newMove.y;
        tempMove += newMove.x1;
        tempMove += newMove.y1;

        //make move on BB's store data to string so move can be undone
        BBBoard->makeMove(newMove, zobrist, isWhite);

        //is move legal? if not skip it
        if(gen_moves.isAttacked(king, isWhite)){
            BBBoard->unmakeMove(newMove, zobrist, isWhite);
            continue;
        }
        legalMoves ++;
        /*
        //futility pruning ~~ is not a promotion, is not a capture, and does not give check
        if(f_prune && i > 0 && newMove.flag != 'Q' && newMove.captured == '0'){ //&& !gen_moves.isAttacked(eking, !isWhite)){
            BBBoard->unmakeMove(moveToUnmake, zobrist);
            continue;
        }
        */
        //jump to other color and evaluate all moves that don't cause a cutoff if depth is greater than 1
        score = -alphaBeta(depth-1, -beta, -alpha,  ! isWhite, currentTime, timeLimmit, currentDepth +1, true, BBBoard, zobrist, eval);

        //undo move on BB's
        BBBoard->unmakeMove(newMove, zobrist, isWhite);

        if(score > alpha){

            //store the principal variation
            pVArr[depth] = newMove;

            //if move causes a beta cutoff stop searching current branch
            if(score >= beta){

                //add beta to transpositon table with beta flag
                addMoveTT(tempMove, depth, beta, 2, zobrist);

                //push killer move to top of stack for given depth
                //killerHArr[depth].push(tempMove);

                //addToKillers(currentDepth, tempMove);
                return beta;
            }
            //new best move
            alpha = score;

            //if we've gained a new alpha set hash Flag equal to exact
            hashFlag = 3;
            hashMove = tempMove;
        }
    }
/*
    //return mate/stalemate score if there are no more moves ~~ greater depth less score
    if(legalMoves == 0){
       return eval->returnMateScore(isWhite, BBBoard, depth);
    }
*/
    //add alpha eval to hash table
    addMoveTT(hashMove, depth, alpha, hashFlag, zobrist);


    return alpha;
}

std::string Ai_Logic::sortMoves(std::string moves, HashEntry entry, int currentDepth, bool isWhite, BitBoards *BBBoards, ZobristH *zobrist)
{
    //sort moves into most valuable victim least valuable attacker order
    //with non captures following all captures
    moves = mostVVLVA(moves, isWhite, BBBoards);

    //call killer moves + add killers to front of moves if there are any
    moves = killerHe(currentDepth, moves);
    //moves = killerTest(currentDepth, moves);

    //perfrom look up from transpositon table
    if(entry.move.length() == 4 && entry.zobrist == zobrist->zobristKey){
        std::string m;
        //if entry is a beta match, add beta cutoff move to front of moves
        if(entry.flag == 2){
            m = entry.move;
            //moves = m + moves;
        //if entry is an exact alpha match add move to front
        }else if(entry.flag == 3){
            m = entry.move;
            //moves = m + moves;
        }

        //validation that move is legit. Mainly used because of Lazy SMP possibly corrupting hashed moves
        for(int i = 0; i < moves.length(); i += 4){
            std::string t;
            t += moves[i];
            t += moves[i+1];
            t += moves[i+2];
            t += moves[i+3];
            if(t == m){
                moves = m + moves;
                break;
            }
        }

    }

    return moves;
}

std::string Ai_Logic::killerHe(int depth, std::string moves)
{
    std::string cutoffs, tempMove, tempMove1;
    int size = killerHArr[depth].size();

    //if no killer moves, return the same list of moves taken
    if(size == 0){
        return moves;
    }

    //loop through killer moves at given depth and test if they're legal
    //(done by testing if they're in move list that's already been legally generated)
    for(int i = 0; i < size ; ++i){
        //grab first killer move for given depth
        tempMove = killerHArr[depth].top();
        for(int j = 0; j < moves.size(); j+=4){
            tempMove1 = moves[j];
            tempMove1 += moves[j+1];
            tempMove1 += moves[j+2];
            tempMove1 += moves[j+3];
            //if killer move matches a move in the moveset for current turn, put it at the front of the list
            if(tempMove == tempMove1){
                cutoffs += tempMove;
                break;
            }
        }
        //remove killer move from depth stack once it's been tested to see if legal or not
        killerHArr[depth].pop();
    }

    //return moves with killer moves in the front
    cutoffs += moves;
    return cutoffs;

}

std::string Ai_Logic::killerTest(int depth, std::string moves)
{
    std::string cutoffs, tempMove, tempMove1;
    //if no killer moves, return the same list of moves taken
    int size = 0;
    if(killers[0][depth].length() >= 4){
        size ++;
    }
    if(killers[1][depth].length() >= 4){
        size ++;
    }
    if(killers[2][depth].length() >= 4){
        size ++;
    }
    if(size == 0){
        return moves;
    }

    //loop through killer moves at given depth and test if they're legal
    //(done by testing if they're in move list that's already been legally generated)
    for(int i = 0; i < size ; ++i){
        //grab first killer move for given depth
        tempMove = killers[i][depth];
        for(int j = 0; j < moves.size(); j+=4){
            tempMove1 = moves[j];
            tempMove1 += moves[j+1];
            tempMove1 += moves[j+2];
            tempMove1 += moves[j+3];
            //if killer move matches a move in the moveset for current turn, put it at the front of the list
            if(tempMove == tempMove1){
                cutoffs += tempMove;
                break;
            }
        }
    }
    cutoffs += moves;
    return cutoffs;
}

void Ai_Logic::addToKillers(int depth, std::string move)
{
    std::string temp, temp1;
    //if move is not identical to other moves in killer at depth
    if(killers[0][depth] != move && killers[1][depth] != move && killers[2][depth] != move){
        //shift killer moves
        temp = killers[0][depth];
        killers[0][depth] = move;
        temp1 = killers[1][depth];
        killers[1][depth] = temp;
        killers[2][depth] = temp1;
    }
}

int Ai_Logic::nullMoves(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, BitBoards *BBBoard, ZobristH *zobrist, evaluateBB *eval)
{
    //update key color
    zobrist->UpdateColor();
    //as well as indicate to transposition tables these are null move boards
    //zobrist->UpdateNull();

    int score = -alphaBeta(depth, alpha, beta, isWhite, currentTime, timeLimmit, currentDepth, false, BBBoard, zobrist, eval);
    //int score = -PVS(depth, alpha, beta, isWhite, currentTime, timeLimmit, currentDepth, false, BBBoard, zobrist, eval);

    //zobrist->UpdateNull();
    zobrist->UpdateColor();
    return score;
}

int Ai_Logic::quiescent(int alpha, int beta, bool isWhite, int currentDepth, int quietDepth, long currentTime, long timeLimmit, BitBoards *BBBoard, ZobristH *zobrist, evaluateBB *eval)
{
    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;

    //transposition hash quiet
    int hash = (int)(zobrist->zobristKey % 338207);
    HashEntry entry = transpositionTQuiet[hash];

    //if the time limmit has been exceeded finish searching
    if(elapsedTime >= timeLimmit){
        searchCutoff = true;
    }

    //evaluate board position (if curentDepth is even return -eval)
    int standingPat = eval->evalBoard(isWhite, BBBoard, zobrist);

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
    gen_moves.grab_boards(BBBoard);
    gen_moves.generatePsMoves(isWhite, true, currentDepth);

    int score;
    std::string hashMove, tempMove;
    //set hash flag equal to alpha Flag
    int hashFlag = 1, moveNum = gen_moves.moveCount;


    U64 king;
    if(isWhite) king = BBBoard->BBWhiteKing;
    else king = BBBoard->BBBlackKing;

    for(int i = 0; i < moveNum; ++i)
    {
        qCount ++;
        tempMove = "";

        Move newMove = gen_moves.movegen_sort(currentDepth);
        //convert move into a single string
        tempMove += newMove.x;
        tempMove += newMove.y;
        tempMove += newMove.x1;
        tempMove += newMove.y1;

        if(gen_moves.isAttacked(king, isWhite)){
            continue;
        }
        /*
        // ~~~NEEDS WORK + Testing + stop pruning in end game
        if(deltaPruning(tempMove, standingPat, isWhite, alpha, false, BBBoard)){
            continue; //uncomment to enable delta pruning!!!
        }

        //bad capture pruning
        if(badCapture(tempMove, isWhite, BBBoard) && tempMove[3] != 'Q'){
            continue;
        }
        */
        BBBoard->makeMove(newMove, zobrist, isWhite);
        /*
        if(BBBoard->isAttacked(isWhite, king)){
            BBBoard->unmakeMove(unmake, zobrist);
            continue;
        }
        */
        score = -quiescent(-beta, -alpha, ! isWhite, currentDepth+1, quietDepth-1, currentTime, timeLimmit, BBBoard, zobrist, eval);

        BBBoard->unmakeMove(newMove, zobrist, isWhite);

        if(score >= beta){
            //add beta to transpositon table with beta flag
            //addTTQuiet(tempMove, currentDepth, beta, 2, zobrist);

            //push killer move to top of stack for given depth
            //killerHArr[currentDepth].push(tempMove);
            //addToKillers(currentDepth, tempMove);
            return beta;
        }

        if(score > alpha){
           alpha = score;
           //if we've gained a new alpha set hash Flag to exact
           hashFlag = 3;
           hashMove = tempMove;
        }
    }
    //add alpha eval to hash table
    //addTTQuiet(hashMove, currentDepth, alpha, hashFlag, zobrist);
    return alpha;
}
/*
bool Ai_Logic::deltaPruning(std::string move, int eval, bool isWhite, int alpha, bool isEndGame, MoveGen *BBBoard)
{
    //if is end game, search all nodes
    if(isEndGame){
        return false;
    }

    U64 epawns, eknights, ebishops, erooks, equeens;
    //set enemy bitboards
    if(isWhite){
        //enemies
        epawns = BBBoard->BBBlackPawns;
        eknights = BBBoard->BBBlackKnights;
        ebishops = BBBoard->BBBlackBishops;
        erooks = BBBoard->BBBlackRooks;
        equeens = BBBoard->BBBlackQueens;
    } else {
        epawns = BBBoard->BBWhitePawns;
        eknights = BBBoard->BBWhiteKnights;
        ebishops = BBBoard->BBWhiteBishops;
        erooks = BBBoard->BBWhiteRooks;
        equeens = BBBoard->BBWhiteQueens;
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

bool Ai_Logic::badCapture(std::string move, bool isWhite, BitBoards *BBBoard)
{
    int x, y, x1, y1, xyI, xyE;
    U64 pieceMaskI = 0LL, pieceMaskE = 0LL;
    x = move[0]-0; y = move[1];
    xyI = y*8+x;
    pieceMaskI += 1LL << xyI;

    //pawns don't lose value in caputres
    if(pieceMaskI & BBBoard->BBBlackPawns || pieceMaskI & BBBoard->BBWhitePawns) return false;

    x1 = move[2]-0; y1 = move[3]-0;
    xyE = y1*8+x1;
    pieceMaskE += 1LL << xyE;

    int piece, cap;

    if(pieceMaskI & BBBoard->BBWhiteKnights || pieceMaskI & BBBoard->BBBlackKnights) piece = 320;
    if(pieceMaskI & BBBoard->BBWhiteBishops || pieceMaskI & BBBoard->BBBlackBishops) piece = 330;
    if(pieceMaskI & BBBoard->BBWhiteRooks || pieceMaskI & BBBoard->BBBlackRooks) piece = 500;
    if(pieceMaskI & BBBoard->BBWhiteQueens || pieceMaskI & BBBoard->BBBlackQueens) piece = 900;
    if(pieceMaskI & BBBoard->BBWhiteKing || pieceMaskI & BBBoard->BBBlackKing) piece = 20000;

    if(pieceMaskE & BBBoard->BBWhitePawns || pieceMaskE & BBBoard->BBBlackPawns) cap = 100;
    if(pieceMaskE & BBBoard->BBWhiteKnights || pieceMaskE & BBBoard->BBBlackKnights) cap = 320;
    if(pieceMaskE & BBBoard->BBWhiteBishops || pieceMaskE & BBBoard->BBBlackBishops) cap = 330;
    if(pieceMaskE & BBBoard->BBWhiteRooks || pieceMaskE & BBBoard->BBBlackRooks) cap = 500;
    if(pieceMaskE & BBBoard->BBWhiteQueens || pieceMaskE & BBBoard->BBBlackQueens) cap = 900;

    //captures lower takes higher
    if(cap >= piece - 50) return false;

    //find squares pawns are defending doesn't include pinned pawns
    U64 pawnDefends;
    if(isWhite){
        pawnDefends = BBBoard->BBBlackPawns;
        pawnDefends |= BBBoard->noEaOne(pawnDefends);
        pawnDefends |= BBBoard->noWeOne(pawnDefends);
        pawnDefends &= ~ BBBoard->BBBlackPawns;
    } else{
        pawnDefends = BBBoard->BBWhitePawns;
        pawnDefends |= BBBoard->soEaOne(pawnDefends);
        pawnDefends |= BBBoard->soWeOne(pawnDefends);
        pawnDefends &= ~ BBBoard->BBWhitePawns;
    }


    if(pieceMaskE & pawnDefends && cap + 200 < piece) return true;



return false;
}
*/
std::string Ai_Logic::mostVVLVA(std::string captures, bool isWhite, BitBoards *BBBoard)
{
    //arrays holding different captures 0 position for pawn captures, 1 = knight, 2 = bishops, 3 = rook, 4 = queen captures
    std::string pawnPromotions;
    std::string pawnCaps[5];
    std::string knightCaps[5];
    std::string bishopCaps[5];
    std::string rookCaps[5];
    std::string queenCaps[5];
    std::string kingCaps[5];

    U64 epawns, eknights, ebishops, erooks, equeens, pawns, knights, bishops, rooks, queens, king;
    //set enemy bitboards and friendly piece bitboards
    if(isWhite){
        //enemies
        epawns = BBBoard->BBBlackPawns;
        eknights = BBBoard->BBBlackKnights;
        ebishops = BBBoard->BBBlackBishops;
        erooks = BBBoard->BBBlackRooks;
        equeens = BBBoard->BBBlackQueens;
        //friendlys
        pawns = BBBoard->BBWhitePawns;
        knights = BBBoard->BBWhiteKnights;
        bishops = BBBoard->BBWhiteBishops;
        rooks = BBBoard->BBWhiteRooks;
        queens = BBBoard->BBWhiteQueens;
        king = BBBoard->BBWhiteKing;
    } else {
        epawns = BBBoard->BBWhitePawns;
        eknights = BBBoard->BBWhiteKnights;
        ebishops = BBBoard->BBWhiteBishops;
        erooks = BBBoard->BBWhiteRooks;
        equeens = BBBoard->BBWhiteQueens;

        pawns = BBBoard->BBBlackPawns;
        knights = BBBoard->BBBlackKnights;
        bishops = BBBoard->BBBlackBishops;
        rooks = BBBoard->BBBlackRooks;
        queens = BBBoard->BBBlackQueens;
        king = BBBoard->BBBlackKing;
    }


    int x, y, x1, y1, xyI, xyE;
    U64 pieceMaskI = 0LL, pieceMaskE = 0LL;
    std::string tempMove;

    //string for holding all non captures, to be added last to moves
    std::string nonCaptures;

    for(int i = 0; i < captures.length(); i+=4)
    {
        pieceMaskI = 0LL, pieceMaskE = 0LL;
        tempMove = "";
        //convert move into a single string
        tempMove += captures[i];
        tempMove += captures[i+1];
        tempMove += captures[i+2];
        tempMove += captures[i+3];
        x = tempMove[0]-0; y = tempMove[1];
        xyI = y*8+x;
        pieceMaskI += 1LL << xyI;
        //normal moves
        if(tempMove[3] != 'Q'){
            //find number representing board end position
            x1 = tempMove[2]-0; y1 = tempMove[3]-0;

        //pawn promotions
        } else {
            //forward non capture
            if(tempMove[2] == 'F'){
                x1 = x;
            //capture
            } else {
                x1 = tempMove[2]-0;
            }
            if(isWhite){
                y1 = 0;
            } else {
                y1 = 7;
            }
            pawnPromotions += tempMove;
        }
        xyE = y1*8+x1;
        //create mask of move end position
        pieceMaskE += 1LL << xyE;

        //if the initial piece is our x piece check which piece he captures (if any) and add move to appropriate array otherwise add it to non captures
        if(pieceMaskI & pawns){
            if(pieceMaskE & epawns){
                pawnCaps[0]+= tempMove;                
                continue;
            } else if(pieceMaskE & eknights){
                pawnCaps[1]+= tempMove;
                continue;
            } else if(pieceMaskE & ebishops){
                pawnCaps[2]+= tempMove;
                continue;
            } else if(pieceMaskE & erooks){
                pawnCaps[3]+= tempMove;
                continue;
            } else if(pieceMaskE & equeens){
                pawnCaps[4]+= tempMove;
                continue;
            }
        } else if (pieceMaskI & knights){
            if(pieceMaskE & epawns){
                knightCaps[0]+= tempMove;
                continue;
            } else if(pieceMaskE & eknights){
                knightCaps[1]+= tempMove;
                continue;
            } else if(pieceMaskE & ebishops){
                knightCaps[2]+= tempMove;
                continue;
            } else if(pieceMaskE & erooks){
                knightCaps[3]+= tempMove;
                continue;
            } else if(pieceMaskE & equeens){
                knightCaps[4]+= tempMove;
                continue;
            }
        } else if (pieceMaskI & bishops){
            if(pieceMaskE & epawns){
                bishopCaps[0]+= tempMove;
                continue;
            } else if(pieceMaskE & eknights){
                bishopCaps[1]+= tempMove;
                continue;
            } else if(pieceMaskE & ebishops){
                bishopCaps[2]+= tempMove;
                continue;
            } else if(pieceMaskE & erooks){
                bishopCaps[3]+= tempMove;
                continue;
            } else if(pieceMaskE & equeens){
                bishopCaps[4]+= tempMove;
                continue;
            }
        } else if (pieceMaskI & rooks){
            if(pieceMaskE & epawns){
                rookCaps[0]+= tempMove;
                continue;
            } else if(pieceMaskE & eknights){
                rookCaps[1]+= tempMove;
                continue;
            } else if(pieceMaskE & ebishops){
                rookCaps[2]+= tempMove;
                continue;
            } else if(pieceMaskE & erooks){
                rookCaps[3]+= tempMove;
                continue;
            } else if(pieceMaskE & equeens){
                rookCaps[4]+= tempMove;
                continue;
            }
        } else if (pieceMaskI & queens){
            if(pieceMaskE & epawns){
                queenCaps[0]+= tempMove;
                continue;
            } else if(pieceMaskE & eknights){
                queenCaps[1]+= tempMove;
                continue;
            } else if(pieceMaskE & ebishops){
                queenCaps[2]+= tempMove;
                continue;
            } else if(pieceMaskE & erooks){
                queenCaps[3]+= tempMove;
                continue;
            } else if(pieceMaskE & equeens){
                queenCaps[4]+= tempMove;
                continue;
            }
        } else if (pieceMaskI & king){
            if(pieceMaskE & epawns){
                kingCaps[0]+= tempMove;
                continue;
            } else if(pieceMaskE & eknights){
                kingCaps[1]+= tempMove;
                continue;
            } else if(pieceMaskE & ebishops){
                kingCaps[2]+= tempMove;
                continue;
            } else if(pieceMaskE & erooks){
                kingCaps[3]+= tempMove;
                continue;
            } else if(pieceMaskE & equeens){
                kingCaps[4]+= tempMove;
                continue;
            }

        //add non capture to string so they can be added last
        }
        nonCaptures += tempMove;

    }

    //add all captures in order of least valuable attacker most valuable victim
    std::string orderedCaptures;

    //ordered as most valuable victim, least valuable attacker ~~ Probably more effeciant order possible
    orderedCaptures += pawnPromotions;

    for(int i = 4; i > -1; --i){
        orderedCaptures += pawnCaps[i];
        orderedCaptures += knightCaps[i];
        orderedCaptures += bishopCaps[i];
        orderedCaptures += rookCaps[i];
        orderedCaptures += queenCaps[i];
        orderedCaptures += kingCaps[i];
    }

    orderedCaptures += nonCaptures;


 return orderedCaptures;

}

bool Ai_Logic::isCapture(std::string move, bool isWhite, BitBoards *BBBoard)
{
    U64 pieceMaskE = 0LL, enemies;
    int x1 = move[2]-0, y1 = move[3]-0;
    int xyE = y1*8+x1;
    pieceMaskE += 1LL << xyE;

    if(isWhite){ enemies = BBBoard->BBBlackPieces;}
    else {enemies = BBBoard->BBWhitePieces;}

    if(enemies & pieceMaskE) return true;

    return false;
}

void Ai_Logic::addMoveTT(std::string bestmove, int depth, long eval, int flag, ZobristH *zobrist)
{
    //get hash of current zobrist key
    int hash = (int)(zobrist->zobristKey % 15485843);
    //if the depth of the current move is greater than the one it's replacing or if it's older than
    if(depth >= transpositionT[hash].depth || transpositionT[hash].ancient < turns - 5){
        //add position to the table
        transpositionT[hash].zobrist = zobrist->zobristKey;
        transpositionT[hash].depth = depth;
        transpositionT[hash].eval = (int)eval;
        transpositionT[hash].flag = flag;
        transpositionT[hash].move = bestmove;
        transpositionT[hash].ancient = turns;

    }

}

void Ai_Logic::addTTQuiet(std::string bestmove, int quietDepth, long eval, int flag, ZobristH *zobrist)
{
    //get hash of current zobrist key
    int hash = (int)(zobrist->zobristKey % 338207);

    if(quietDepth <= transpositionTQuiet[hash].depth){
        //add position to the table
        transpositionTQuiet[hash].zobrist = zobrist->zobristKey;
        transpositionTQuiet[hash].depth = quietDepth;
        transpositionTQuiet[hash].eval = (int)eval;
        transpositionTQuiet[hash].move = bestmove;
        transpositionTQuiet[hash].flag = flag;

    }
}
