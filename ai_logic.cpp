#include "ai_logic.h"
#include "zobristh.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <thread>
#include "bitboards.h"
#include "tile.h"

#include <future>


//best overall move as calced
std::string bestMove;

int bestScore;

//evaluation object - evaluates board position and gives an int value (- for black)
evaluateBB *eval = new evaluateBB;

//bool value to determine if time for search has run out
bool searchCutoff = false;
//count of quiescnce positions checked
int qcount = 0;

//number representing amount to reduce search with Null-Moves
const int depthR = 2;

//master bitboard for turn
BitBoards *newBoard = new BitBoards;

//master zobrist key for turn
U64 zobristK;

Ai_Logic::Ai_Logic()
{
    //construct boards to get zobrist hash
    newBoard->constructBoards();
    //once opponent move is made update the zorbist hash key
    zobristK = ZKey->getZobristHash(true, newBoard);
    //update zobrsit hash to correct color
    ZKey->UpdateColor();
}

std::string Ai_Logic::iterativeDeep(int depth)
{


    //iterative deepening start time
    clock_t IDTimeS = clock();

    //time limit in miliseconds
    int timeLimmit = 18000, currentDepth = 0;
    long endTime = IDTimeS + timeLimmit;

    //normal positions searched
    positionCount = 0;
    int tpositionCount;
    //count of number of quiet nodes searched
    qcount = 0;

    //search has not run out of time
    searchCutoff = false;

    std::cout << zobKey << std::endl;

    int alpha = -100000;
    int beta = 100000;

    int distance = 1;
    for(distance; distance <= depth && IDTimeS < endTime;){
        positionCount = 0;
        clock_t currentTime = clock();

        if(currentTime >= endTime){
            distance - 1;
            break;
        }


        //tBestMove = miniMaxRoot(distance, true, currentTime, timeLimmit);
        //int val = principleV(distance, alpha, beta, false, currentDepth);
        //std::string tbMove = alphaBetaRoot(distance, alpha, beta, false, currentTime, timeLimmit, currentDepth +1, true);        

        int val = alphaBeta(distance, alpha, beta, false, currentTime, timeLimmit, currentDepth +1, true, newBoard);

        //multi threading testing
        //int val = multi(distance, alpha, beta, false, currentTime, timeLimmit, currentDepth +1, true);

        //aspiration window correction
        if (val <= alpha || val >= beta) {
            alpha = -100000;    // We fell outside the window, so try again with a
            beta = 100000;      //  full-width window (and the same depth).
            continue;
        }
        //if we don't fall out of window, set alpha and beta to a window size to narrow search
        alpha = bestScore - 25;
        beta = bestScore + 25;

        //if the search is not cutoff
        if(!searchCutoff){
            //get hash of current position
            int hash = (int)(zobKey % 15485843);
            //store best move from hash table
            bestMove = transpositionT[hash].move;
            tpositionCount = positionCount;
        }
        //increment distance to travel (same as depth at max depth)
        distance++;
    }
    std::cout << std::endl;

    //make final move on bitboards
    newBoard->makeMove(bestMove);

    newBoard->drawBBA();

    clock_t IDTimeE = clock();
    //postion count and time it took to find move
    std::cout << tpositionCount << " positions searched." << std::endl;
    std::cout << (double) (IDTimeE - IDTimeS) / CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Depth of " << distance-1 << " reached."<<std::endl;
    //std::cout << zobKey << std::endl;
    std::cout << qcount << " quiet positions searched."<< std::endl;

    return bestMove;
}

int Ai_Logic::multi(int distance, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, bool allowNull)
{
    BitBoards *BB0 = new BitBoards;
    BitBoards *BB1 = new BitBoards;
    BitBoards *BB2 = new BitBoards;
    BB0->constructBoards();
    BB1->constructBoards();
    BB2->constructBoards();

    auto f1 = std::async(std::launch::async, &Ai_Logic::alphaBeta, this, distance, alpha, beta, isWhite, currentTime, timeLimmit, currentDepth, allowNull, BB0);
    auto f2 = std::async(std::launch::async, &Ai_Logic::alphaBeta, this, distance-1, alpha, beta, isWhite, currentTime, timeLimmit, currentDepth, allowNull, BB1);
    auto f3 = std::async(std::launch::async, &Ai_Logic::alphaBeta, this, distance+1, alpha, beta, isWhite, currentTime, timeLimmit, currentDepth, allowNull, BB2);

    auto val = f1.get();
    auto val2 = f2.get();
    auto val3 = f3.get();

    return val;
}

int Ai_Logic::alphaBeta(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, bool allowNull, BitBoards *BBBoard)
{
    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;

    //create unqiue hash from zobkey
    int hash = (int)(zobKey % 15485843);
    HashEntry entry = transpositionT[hash];


    //if the depth of the stored evaluation is greater and the zobrist key matches
    //don't return eval on root node
    if(entry.depth >= depth && entry.zobrist == zobKey){
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

    int score;
    if(depth == 0 || searchCutoff){
        int queitSD = 16;
        //run capture search to max depth of queitSD
        score = quiescent(alpha, beta, isWhite, currentDepth, queitSD, currentTime, timeLimmit, BBBoard);

        //add move to hash table with exact flag
        addMoveTT("0", depth, score, 3);
        return score;
    }


    //Null move heuristics, disabled if in check
    if(allowNull && depth >= depthR+1 && ! BBBoard->isInCheck(isWhite) && turns < 26){
        score = nullMoves(depth-1-depthR, -beta, -beta+1, !isWhite, currentTime, timeLimmit, currentDepth+1, BBBoard);
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
    moves = sortMoves(moves, entry, currentDepth, isWhite, BBBoard);

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
        moveToUnmake = BBBoard->makeMove(tempMove);

        //jump to other color and evaluate all moves that don't cause a cutoff if depth is greater than 1
        bestTempMove = -alphaBeta(depth-1, -beta, -alpha,  ! isWhite, currentTime, timeLimmit, currentDepth +1, true, BBBoard);

        //undo move on BB's
        BBBoard->unmakeMove(moveToUnmake);

        //if move causes a beta cutoff stop searching current branch
        if(bestTempMove >= beta){

            //add beta to transpositon table with beta flag
            addMoveTT(tempMove, depth, beta, 2);

            //push killer move to top of stack for given depth
            killerHArr[currentDepth].push(tempMove);
            //addToKillers(currentDepth, tempMove);

            return beta;
        }

        if(bestTempMove > alpha){
            //new best move
            alpha = bestTempMove;

            //if we've gained a new alpha set hash Flag equal to exact
            hashFlag = 3;
            hashMove = tempMove;
        }
    }

    //add alpha eval to hash table
    addMoveTT(hashMove, depth, alpha, hashFlag);

    return alpha;
}


std::string Ai_Logic::sortMoves(std::string moves, HashEntry entry, int currentDepth, bool isWhite, BitBoards *BBBoards)
{
    //sort moves into most valuable victim least valuable attacker order
    //with non captures following all captures
    moves = mostVVLVA(moves, isWhite, BBBoards);
    //call killer moves + add killers to front of moves if there are any
    moves = killerHe(currentDepth, moves);
    //moves = killerTest(currentDepth, moves);
    //perfrom look up from transpositon table

    if(entry.move.length() == 4 && entry.zobrist == zobKey){
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

int Ai_Logic::nullMoves(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, BitBoards *BBBoard)
{
    int score;
    //update key color
    ZKey->UpdateColor();
    //as well as indicate to transposition tables these are null move boards
    ZKey->UpdateNull();
    score = -alphaBeta(depth, alpha, beta, isWhite, currentTime, timeLimmit, currentDepth, false, BBBoard);
    ZKey->UpdateColor();
    ZKey->UpdateNull();
    //if after getting a free move the score is too good, prune this branch
    return score;
}

int Ai_Logic::quiescent(int alpha, int beta, bool isWhite, int currentDepth, int quietDepth, long currentTime, long timeLimmit, BitBoards *BBBoard)
{
    static int depth = currentDepth;
    int standingPat;

    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;

    //transposition hash quiet
    int hash = (int)(zobKey % 338207);
    HashEntry entry = transpositionTQuiet[hash];

    if(entry.depth >= quietDepth && entry.zobrist == zobKey){
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
    /*
    int bigDelta = 900; // queen value
    if (standingPat < alpha - bigDelta ) {
       return alpha;
    }
*/
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
    sortMoves(captures, entry, currentDepth, isWhite, BBBoard);

    int score;
    std::string unmake, hashMove, tempMove;
    //set hash flag equal to alpha Flag
    int hashFlag = 1;

    for(int i = 0; i < captures.length(); i+=4)
    {
        qcount ++;
        tempMove = "";
        //convert move into a single string
        tempMove += captures[i];
        tempMove += captures[i+1];
        tempMove += captures[i+2];
        tempMove += captures[i+3];
        // ~~~NEEDS WORK + Testing + stop pruning in end game
        if(!deltaPruning(tempMove, standingPat, isWhite, alpha, false, BBBoard)){
            //continue;
        }

        unmake = BBBoard->makeMove(tempMove);

        score = -quiescent(-beta, -alpha, ! isWhite, currentDepth+1, quietDepth-1, currentTime, timeLimmit, BBBoard);

        BBBoard->unmakeMove(unmake);

        if(score >= beta){
            //add beta to transpositon table with beta flag
            addTTQuiet(tempMove, currentDepth, beta, 2);

            //push killer move to top of stack for given depth
            killerHArr[currentDepth].push(tempMove);
            //addToKillers(currentDepth, tempMove);
           return beta;
        }

        if(score > alpha){
           alpha = score;
           //if we've gained a new alpha set hash Flag equal to exact
           hashFlag = 3;
           hashMove = tempMove;
        }
    }
    //add alpha eval to hash table
    addTTQuiet(hashMove, currentDepth, alpha, hashFlag);
    return alpha;
}

bool Ai_Logic::deltaPruning(std::string move, int eval, bool isWhite, int alpha, bool isEndGame, BitBoards *BBBoard)
{
    //if is end game, search all nodes
    if(isEndGame){
        return true;
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

    U64 pieceMaskE;
    int x, y, xyE;

    x = move[2]-0; y = move[3]-0;
    xyE = y*8+x;
    //create mask of move end position
    pieceMaskE += 1LL << xyE;

    int score, Delta = 200;
    //find whice piece is captured
    if(pieceMaskE & epawns){
        score = 100;
    } else if(pieceMaskE & eknights){
        score = 320;
    } else if(pieceMaskE & ebishops){
        score = 330;
    } else if(pieceMaskE & erooks){
        score = 500;
    } else if(pieceMaskE & equeens){
        score = 900;
    }

    //if score plus safety margin is less then alpha, don't bother searching node
    if(eval + score + Delta < alpha){
        return false;
    }

    return true;

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

    orderedCaptures += pawnPromotions;
    //ordered as most valuable victim, least valuable attacker ~~ Probably more effeciant order possible
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

void Ai_Logic::extractPV(int depthReached, BitBoards *BBBoard)
{

    int hash = (int)(zobKey % 15485843);
    HashEntry entry = transpositionT[hash];
    std::vector<std::string> moveVec;

    char flips[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};
    char flipsL[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

    //make move and print PV
    for(int i = 0; i < depthReached; i++){
        std::cout << flipsL[(int)entry.move[0]] << flips[(int)entry.move[1]] << flipsL[(int)entry.move[2]] << flips[(int)entry.move[3]] << ", ";
        std::string tmove = BBBoard->makeMove(entry.move);
        moveVec.push_back(tmove);
        hash = (int)(zobKey % 15485843);
        entry = transpositionT[hash];
    }

    //unmake moves
    for(int i = depthReached-1; i >= 0; i--){
        BBBoard->unmakeMove(moveVec[i]);
    }

}

void Ai_Logic::addMoveTT(std::string bestmove, int depth, long eval, int flag)
{
    //get hash of current zobrist key
    int hash = (int)(zobKey % 15485843);
    //if the depth of the current move is greater than the one it's replacing...
    if(depth >= transpositionT[hash].depth){
        //add position to the table
        transpositionT[hash].zobrist = zobKey;
        transpositionT[hash].depth = depth;
        transpositionT[hash].eval = (int)eval;
        transpositionT[hash].flag = flag;
        transpositionT[hash].move = bestmove;

    }

}

void Ai_Logic::addTTQuiet(std::string bestmove, int quietDepth, long eval, int flag)
{
    //get hash of current zobrist key
    int hash = (int)(zobKey % 338207);

    if(quietDepth >= transpositionTQuiet[hash].depth){
        //add position to the table
        transpositionTQuiet[hash].zobrist = zobKey;
        transpositionTQuiet[hash].depth = quietDepth;
        transpositionTQuiet[hash].eval = (int)eval;
        transpositionTQuiet[hash].move = bestmove;
        transpositionTQuiet[hash].flag = flag;

    }
}

std::string Ai_Logic::debug(std::string ttMove, std::string moves)
{

    std::string tempMove = ttMove, tempMove1;
    for(int j = 0; j < moves.size(); j+=4){
        tempMove1 = moves[j];
        tempMove1 += moves[j+1];
        tempMove1 += moves[j+2];
        tempMove1 += moves[j+3];
        if(tempMove == tempMove1){
            tempMove += moves;
            return tempMove;
        }
    }

    if(ttMove.length() > 0 && moves != ""){
        std::cout << zobKey << std::endl;
        newBBBoard->drawBBA();
    }

    return moves;
}

int Ai_Logic::principleV(int depth, int alpha, int beta, bool isWhite, int currentDepth, BitBoards *BBBoard)
{
    int bestScore;
    int bestMoveIndex = 0;


    if(depth == 0){
        int bestScore = eval->evalBoard(isWhite, BBBoard);

        return bestScore;
    }

    std::string moves, tempBBMove;

    moves = BBBoard->genWhosMove(isWhite);

    //Do full depth search of first move
    std::string tempMove;
    //convert move into a single string
    tempMove += moves[0];
    tempMove += moves[1];
    tempMove += moves[2];
    tempMove += moves[3];

    //make move on BB's store data to string so move can be undone
    tempBBMove = BBBoard->makeMove(tempMove);

    //jump to other color and evaluate all moves that don't cause a cutoff if depth is greater than 1
    bestScore = -principleV(depth-1, -beta, -alpha,  ! isWhite, currentDepth+1, BBBoard);

    positionCount ++;

    //undo move on BB's
    BBBoard->unmakeMove(tempBBMove);

    //NEED a mate check here

    //if move is too good to be true
    if(bestScore >= beta){
        return bestScore;
    }

    //if the moves is an improvement
    if(bestScore > alpha){   
        //if it's better but not too good to be true
        alpha = bestScore;

    }


    //Do a shallower zero window search of moves other than the first, if they're better
    //run a full PV search
    for(int i = bestMoveIndex+4; i < moves.length(); i += 4){
        int score;
        positionCount ++;
        std::string move;
        //convert move into a single string
        move += moves[i];
        move += moves[i+1];
        move += moves[i+2];
        move += moves[i+3];
        //make move
        tempBBMove = newBBBoard->makeMove(tempMove);

        //zero window search
        score = -zWSearch(depth -1, -alpha, !isWhite, BBBoard);

        //unmake move
        newBBBoard->unmakeMove(tempBBMove);

        if((score > alpha) && (score < beta)){

            bestScore = -principleV(depth-1, -beta, -alpha, !isWhite, currentDepth+1, BBBoard);

            if(score > alpha){
                bestMoveIndex = i;
                alpha = score;
            }
        }
        if(score > bestScore){

            if(score >= beta){
                return score;
            }

            bestScore = score;

            //need checkmate check
        }
    }
     return bestScore;
}

int Ai_Logic::zWSearch(int depth, int beta, bool isWhite, BitBoards *BBBoard)
{
    int score = -999999;
    //alpha = beta-1;

    if(depth == 0){
        score = eval->evalBoard(isWhite, BBBoard);
        return score;
    }

    std::string moves, tempBBMove;
    moves = BBBoard->genWhosMove(isWhite);

    for(int i = 4; i < moves.length(); i += 4){
        std::string move;
        //convert move into a single string
        move += moves[i];
        move += moves[i+1];
        move += moves[i+2];
        move += moves[i+3];

        tempBBMove = BBBoard->makeMove(move);

        score = -zWSearch(depth - 1, 1 - beta, ! isWhite, BBBoard);

        BBBoard->unmakeMove(tempBBMove);

        if(score >= beta){

            return score; // fail hard beta cutoff
        }

    }
    return beta-1; //same as alpha
}
