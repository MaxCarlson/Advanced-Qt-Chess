#include "ai_logic.h"
#include "zobristh.h"
#include "bitboards.h"
#include "tile.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>

//value to determine if time for search has run out
bool searchCutoff = false;

Ai_Logic::Ai_Logic()
{

}

std::string Ai_Logic::iterativeDeep(int depth)
{

    //master bitboard for turn
    BitBoards *newBoard = new BitBoards;

    //master zobrist object for ai turn
    ZobristH *mZobrist = new ZobristH;

    //master evaluation object - evaluates board position and gives an int value (- for black)
    evaluateBB *mEval = new evaluateBB;

    //construct boards to get zobrist hash
    newBoard->constructBoards();
    //get master zobrist hash for turn
    mZobrist->getZobristHash(newBoard);
    //update zobrsit hash to correct color
    mZobrist->UpdateColor();

    //iterative deepening start time
    clock_t IDTimeS = clock();

    //time limit in miliseconds
    int timeLimmit = 18000999999, currentDepth = 0;
    long endTime = IDTimeS + timeLimmit;

    std::cout << mZobrist->zobristKey << std::endl;    

    //best overall move as calced
    std::string bestMove;
    int distance = 1, bestScore, alpha = -100000, beta = 100000;
    //search has not run out of time
    searchCutoff = false;
/*
    ZobristH *z0 = new ZobristH;
    ZobristH *z1 = new ZobristH;
    ZobristH *z2 = new ZobristH;

    z0->getZobristHash(newBoard);
    z0->UpdateColor();
    z1->getZobristHash(newBoard);
    z1->UpdateColor();
    z2->getZobristHash(newBoard);
    z2->UpdateColor();

    BitBoards *BB0 = new BitBoards;
    BitBoards *BB1 = new BitBoards;
    BitBoards *BB2 = new BitBoards;
    BB0->constructBoards();
    BB1->constructBoards();
    BB2->constructBoards();
    evaluateBB *e0 = new evaluateBB;
    evaluateBB *e1 = new evaluateBB;
    evaluateBB *e2 = new evaluateBB;

    Ai_Logic *nai = new Ai_Logic;
    Ai_Logic *nai1 = new Ai_Logic;

    std::thread t0(&nai->alphaBeta, nai, distance, -100000, 100000, false, 0, 7000, currentDepth+1, true, BB0, z0, e0);
    std::thread t1(&nai1->alphaBeta, nai1, distance, -100000, 100000, false, 0, 8000, currentDepth+1, true, BB1, z1, e1);
*/
    helperThreads *threads = new helperThreads;

    //iterative deepening loop starts at depth 1, iterates up till max depth or time cutoff
    while(distance <= depth && IDTimeS < endTime){
        clock_t currentTime = clock();

        if(currentTime >= endTime){
            break;
        }

        helperThreads *t = multi(distance, alpha, beta, false, currentTime, timeLimmit, currentDepth +1, threads);

        bestScore = alphaBeta(distance, alpha, beta, false, currentTime, timeLimmit, currentDepth +1, true, newBoard, mZobrist, mEval);

        t->helpers[0].join();
        t->helpers[1].join();
        t->helpers[2].join();
        t->helpers[3].join();



        //aspiration window correction
        if (bestScore <= alpha || bestScore >= beta) {
            alpha = -100000;    // We fell outside the window, so try again with a
            beta = 100000;      //  full-width window (and the same depth).
            continue;
        }
        //if we don't fall out of window, set alpha and beta to a window size to narrow search
        alpha = bestScore - 25;
        beta = bestScore + 25;

        //if the search is not cutoff
        if(!searchCutoff){

            //grab best move out of PV array
            bestMove = pVArr[distance];

            std::cout << (int)bestMove[0] << (int)bestMove[1] << (int)bestMove[2] << (int)bestMove[3] << ", ";
        }
        //increment distance to travel (same as depth at max depth)
        distance++;
    }

    searchCutoff = true;

    //t0.join();
    //t1.join();

    if(bestMove.length() != 4){
        int a = 5;
    }

    std::cout << std::endl;

    //make final move on bitboards
    newBoard->makeMove(bestMove, mZobrist);

    newBoard->drawBBA();

    clock_t IDTimeE = clock();
    //postion count and time it took to find move
    std::cout << positionCount << " positions searched." << std::endl;
    std::cout << (double) (IDTimeE - IDTimeS) / CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Depth of " << distance-1 << " reached."<<std::endl;
    //std::cout << zobKey << std::endl;
    std::cout << qCount << " non-quiet positions searched."<< std::endl;

    return bestMove;
}

helperThreads* Ai_Logic::multi(int distance, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, helperThreads *threads)
{
    ZobristH *z0 = new ZobristH;
    ZobristH *z1 = new ZobristH;
    ZobristH *z2 = new ZobristH;

    BitBoards *BB0 = new BitBoards;
    BitBoards *BB1 = new BitBoards;
    BitBoards *BB2 = new BitBoards;
    BB0->constructBoards();
    BB1->constructBoards();
    BB2->constructBoards();
    z0->getZobristHash(BB0);
    z0->UpdateColor();
    z1->getZobristHash(BB0);
    z1->UpdateColor();
    z2->getZobristHash(BB0);
    z2->UpdateColor();
    evaluateBB *e0 = new evaluateBB;
    evaluateBB *e1 = new evaluateBB;
    evaluateBB *e2 = new evaluateBB;

    Ai_Logic *nai = new Ai_Logic[4];
    //Ai_Logic *nai1 = new Ai_Logic;



    threads->helpers[0] = std::thread(&nai[0].alphaBeta, nai, distance, -100000, 100000, false, 0, 7000, currentDepth+1, true, BB0, z0, e0);
    //helperThreads.helpers[1] = std::thread(&nai1->alphaBeta, nai1, distance, -100000, 100000, false, 0, 8000, currentDepth+1, true, BB1, z1, e1);
    //helperThreads.helpers[1] = std::thread(&Ai_Logic::alphaBeta, this, depth, -100000, 100000, false, 0, 7000, currentDepth+1, true, BB2, z2, e2);

}

int Ai_Logic::alphaBeta(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, bool allowNull, BitBoards *BBBoard, ZobristH *zobrist, evaluateBB *eval)
{

    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;

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
    if(depth <= 0 || searchCutoff){
        int queitSD = 16;
        //run capture search to max depth of queitSD
        //score = quiescent(alpha, beta, isWhite, currentDepth, queitSD, currentTime, timeLimmit, BBBoard, zobrist, eval);
        score = eval->evalBoard(isWhite, BBBoard);
        //add move to hash table with exact flag
        addMoveTT("0", depth, score, 3, zobrist);

        return score;
    }


    //Null move heuristics, disabled if in check
    if(allowNull && depth > depthR && turns < 26 && ! BBBoard->isInCheck(isWhite)){ // ??
        score = nullMoves(depth-1-depthR, -beta, -beta+1, !isWhite, currentTime, timeLimmit, currentDepth+1, BBBoard, zobrist, eval);
        //if after getting a free move the score is too good, prune this branch
        if(score >= beta){
            return score;
        }
    }

    std::string moves;
    int bestTempMove;

    //generate normal moves
    moves = BBBoard->genWhosMove(isWhite);

    //return mate score if there are no more moves
    if(moves.length() == 0){
       return eval->returnMateScore(isWhite);
    }

    //apply heuristics and move entrys from hash table, add to front of moves
    moves = sortMoves(moves, entry, currentDepth, isWhite, BBBoard, zobrist);

    //set hash flag equal to alpha Flag
    int hashFlag = 1;

    std::string tempMove, moveToUnmake, hashMove;
    for(int i = 0; i < moves.length(); i+=4){
        positionCount ++;
        //change board accoriding to i possible move
        tempMove = "";
        //convert move into a single string
        tempMove += moves[i];
        tempMove += moves[i+1];
        tempMove += moves[i+2];
        tempMove += moves[i+3];

        //make move on BB's store data to string so move can be undone
        moveToUnmake = BBBoard->makeMove(tempMove, zobrist);

        //jump to other color and evaluate all moves that don't cause a cutoff if depth is greater than 1
        bestTempMove = -alphaBeta(depth-1, -beta, -alpha,  ! isWhite, currentTime, timeLimmit, currentDepth +1, true, BBBoard, zobrist, eval);

        //undo move on BB's
        BBBoard->unmakeMove(moveToUnmake, zobrist);

        //if move causes a beta cutoff stop searching current branch
        if(bestTempMove >= beta){

            //add beta to transpositon table with beta flag
            addMoveTT(tempMove, depth, beta, 2, zobrist);

            //push killer move to top of stack for given depth
            //killerHArr[currentDepth].push(tempMove);
            addToKillers(currentDepth, tempMove);

            return beta;
        }

        if(bestTempMove > alpha){
            //new best move
            alpha = bestTempMove;

            //store the principal variation
            pVArr[depth] = tempMove;

            //if we've gained a new alpha set hash Flag equal to exact
            hashFlag = 3;
            hashMove = tempMove;
        }
    }

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
    //moves = killerHe(currentDepth, moves);
    moves = killerTest(currentDepth, moves);

    //perfrom look up from transpositon table
    if(entry.move.length() == 4 && entry.zobrist == zobrist->zobristKey){
        std::string m;
        //if entry is a beta match, add beta cutoff move to front of moves
        if(entry.flag == 2){
            m = entry.move;
            moves = m + moves;
        //if entry is an exact alpha match add move to front
        }else if(entry.flag == 3){
            m = entry.move;
            moves = m + moves;
        }
/*
        //validation that move is legit. Mainly used because of Lazy SMP corrupting hash values
        for(int i = 0; i < moves.length(); i += 4){
            std::string t = "";
            t += moves[i];
            t += moves[i+1];
            t += moves[i+2];
            t += moves[i+3];
            if(t == m){
                moves = m + moves;
                break;
            }
        }
*/
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
    for(int i = 0; i < 3 ; ++i){
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

    //zobrist->UpdateNull();
    zobrist->UpdateColor();
    return score;
}

int Ai_Logic::quiescent(int alpha, int beta, bool isWhite, int currentDepth, int quietDepth, long currentTime, long timeLimmit, BitBoards *BBBoard, ZobristH *zobrist, evaluateBB *eval)
{
    static int depth = currentDepth;
    int standingPat;

    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;

    //transposition hash quiet
    int hash = (int)(zobrist->zobristKey % 338207);
    HashEntry entry = transpositionTQuiet[hash];

    if(entry.depth >= quietDepth && entry.zobrist == zobrist->zobristKey){
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

    //if the time limmit has been exceeded finish searching
    if(elapsedTime >= timeLimmit){
        searchCutoff = true;
    }

    //evaluate board position (if curentDepth is even return -eval)

    standingPat = eval->evalBoard(isWhite, BBBoard);


    if(quietDepth == 0 || searchCutoff){
        return standingPat;
    }


    if(standingPat >= beta){
       return beta;
    }

    //simple horrible node pruning, if no possible move can improve alpha, no reason to search
    ///ONCE pawn promotion is complete, augment with 775 increase to bigD if promotion possible

    int bigDelta = 900; // queen value
    if (standingPat < alpha - bigDelta ) {
       return alpha;
    }

    if(alpha < standingPat){
       alpha = standingPat;
    }

    //generate moves then parse them for captures
    std::string captures = BBBoard->generateCaptures(isWhite);

    //if there are no captures, return value of board
    if(captures.length() == 0){
        return standingPat;
    }

    //add killers and or exact/beta hash table matches
    sortMoves(captures, entry, currentDepth, isWhite, BBBoard, zobrist);

    int score;
    std::string unmake, hashMove, tempMove;
    //set hash flag equal to alpha Flag
    int hashFlag = 1;

    for(int i = 0; i < captures.length(); i+=4)
    {
        qCount ++;
        tempMove = "";
        //convert move into a single string
        tempMove += captures[i];
        tempMove += captures[i+1];
        tempMove += captures[i+2];
        tempMove += captures[i+3];
        // ~~~NEEDS WORK + Testing + stop pruning in end game
        if(deltaPruning(tempMove, standingPat, isWhite, alpha, false, BBBoard)){
            continue; //uncomment to enable delta pruning!!!
        }

        unmake = BBBoard->makeMove(tempMove, zobrist);

        score = -quiescent(-beta, -alpha, ! isWhite, currentDepth+1, quietDepth-1, currentTime, timeLimmit, BBBoard, zobrist, eval);

        BBBoard->unmakeMove(unmake, zobrist);

        if(score >= beta){
            //add beta to transpositon table with beta flag
            addTTQuiet(tempMove, currentDepth, beta, 2, zobrist);

            //push killer move to top of stack for given depth
            //killerHArr[currentDepth].push(tempMove);
            addToKillers(currentDepth, tempMove);
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
    addTTQuiet(hashMove, currentDepth, alpha, hashFlag, zobrist);
    return alpha;
}

bool Ai_Logic::deltaPruning(std::string move, int eval, bool isWhite, int alpha, bool isEndGame, BitBoards *BBBoard)
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
    int length = captures.length();

    for(int i = 0; i < length; i+=4)
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

    orderedCaptures += pawnCaps[4];
    orderedCaptures += knightCaps[4];
    orderedCaptures += bishopCaps[4];
    orderedCaptures += rookCaps[4];
    orderedCaptures += queenCaps[4];
    orderedCaptures += kingCaps[4];

    orderedCaptures += pawnCaps[3];
    orderedCaptures += knightCaps[3];
    orderedCaptures += bishopCaps[3];
    orderedCaptures += rookCaps[3];
    orderedCaptures += queenCaps[3];
    orderedCaptures += kingCaps[3];

    orderedCaptures += pawnCaps[2];   
    orderedCaptures += knightCaps[2];
    orderedCaptures += bishopCaps[2];
    orderedCaptures += rookCaps[2];
    orderedCaptures += queenCaps[2];
    orderedCaptures += kingCaps[2];

    orderedCaptures += pawnCaps[1];
    orderedCaptures += knightCaps[1];
    orderedCaptures += bishopCaps[1];
    orderedCaptures += rookCaps[1];
    orderedCaptures += queenCaps[1];
    orderedCaptures += kingCaps[1];

    orderedCaptures += pawnCaps[0];
    orderedCaptures += knightCaps[0];
    orderedCaptures += bishopCaps[0];
    orderedCaptures += rookCaps[0];
    orderedCaptures += queenCaps[0];
    orderedCaptures += kingCaps[0];

    orderedCaptures += nonCaptures;


 return orderedCaptures;

}

void Ai_Logic::addMoveTT(std::string bestmove, int depth, long eval, int flag, ZobristH *zobrist)
{
    //get hash of current zobrist key
    int hash = (int)(zobrist->zobristKey % 15485843);
    //if the depth of the current move is greater than the one it's replacing...
    if(depth >= transpositionT[hash].depth){
        //add position to the table
        transpositionT[hash].zobrist = zobrist->zobristKey;
        transpositionT[hash].depth = depth;
        transpositionT[hash].eval = (int)eval;
        transpositionT[hash].flag = flag;
        transpositionT[hash].move = bestmove;

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
