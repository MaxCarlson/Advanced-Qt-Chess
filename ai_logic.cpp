#include "ai_logic.h"
#include "zobristh.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "bitboards.h"
#include "tile.h"



//best overall move as calced
std::string bestMove;

//holds board state before any moves or trys
std::string board1[8][8];
//holds state of initial board + 1 move
std::string board2[8][8];

//evaluation object
evaluateBB *eval = new evaluateBB;






Ai_Logic::Ai_Logic()
{
    //generate all possible moves for one turn
    newBBBoard->constructBoards();
    //once opponent move is made update the zorbist hash key
    ZKey->getZobristHash(true);
    //update zobrsit hash to correct color
    ZKey->UpdateColor();
}

std::string Ai_Logic::miniMaxRoot(int depth, bool isMaximisingPlayer)
{

    //Move timer for ai
    clock_t aiMoveTimerStart = clock();

    //generate first possible initial moves
    std::string tmoves = newBBBoard->genWhosMove(false); //false for is not white, change later to be more versitile
    std::string moves, tempBBMove;

    //for moves to compete against
    long bestMoveValue = -999999, tempValue;

    //best move to return after all calcs
    std::string bestMoveFound;

    //lookup whether board position has been input into TTable if so add it to front of moves
    //moves = lookUpTTable(depth);
    //moves += tmoves;
    std::cout << zobKey << std::endl;

    moves = debug(lookUpTTable(depth), tmoves);

    int numberOfMoves = moves.length()/4;

    //compare moves
    for(int i = 0; i < moves.length(); i+=4){
        std::string tempMove;
        //convert move into a single string
        tempMove += moves[i];
        tempMove += moves[i+1];
        tempMove += moves[i+2];
        tempMove += moves[i+3];

        //make move on BB
        tempBBMove = newBBBoard->makeMove(tempMove);

        //test it's value and store it and test if white or black,
        tempValue = miniMax(depth -1, -100000, 100000, ! isMaximisingPlayer, numberOfMoves);

        //undo move on BB
        newBBBoard->unmakeMove(tempBBMove);

        //if move is better then the best one store it
        if(tempValue >= bestMoveValue){
            bestMoveValue = tempValue;
            bestMoveFound = tempMove;

        }

    }

    //add best move to transpositon table
    //addMoveTT(tempBBMove, depth, tempValue, 3);

    //postion count and time it took to find move
    clock_t aiEndMoveTImer = clock();    
    std::cout << positionCount << std::endl;
    std::cout << (double) (aiEndMoveTImer - aiMoveTimerStart) / CLOCKS_PER_SEC << " seconds" << std::endl;
    possible_moves.clear();
    positionCount = 0;

    std::cout << zobKey << std::endl;
    //make BB move final
    newBBBoard->makeMove(bestMoveFound);

    newBBBoard->drawBBA();
    std::cout << std::endl;
    //update zobrist hash to correct color
    ZKey->UpdateColor();

    return bestMoveFound;

}

long Ai_Logic::miniMax(int depth, long alpha, long beta, bool isMaximisingPlayer, int numberOfMoves)
{
    positionCount ++;

    //update zobrist hash to correct color
    ZKey->UpdateColor();

    //create unqiue hash from zobkey
    int hash = (int)(zobKey % 1048583);
    HashEntry entry = transpositionT[hash];
    //if the depth of the stored evaluation is greater

    if(entry.depth >= depth){
        //if the zobrist key matches
        if(entry.zobrist == zobKey){
            //return either the eval, the beta, or the alpha
            if(entry.flag == 3){
                return entry.eval;
            }
            if(entry.flag == 2 && entry.eval >= beta){
                return beta;
            }
            if(entry.flag == 1 && entry.eval <= alpha){
                return alpha;
            }
        }
    }


    if(depth == 0){
        long score = - eval->evalBoard();
        //update zobrist hash to correct color
        ZKey->UpdateColor();
        //add move to hash table with exact flag
        addMoveTT("0", depth, score, 3);
        return score;
    }

    std::string moves, tmoves, tempBBMove;

    if(isMaximisingPlayer == true){
       moves = newBBBoard->genWhosMove(false); //still just using bool values for what should be white or black dependant
    } else{
       moves = newBBBoard->genWhosMove(true);
    }


    if(isMaximisingPlayer == true){
        long bestTempMove = -999999;
        //push  killer (better) moves to (eventually just near) front of move list
        //in order to get a high cutoff as fast as possible
        moves = killerHe(depth, moves, false);

        for(int i = 0; i < moves.length(); i+=4){
            //change board accoriding to i possible move
            std::string tempMove;
            //convert move into a single string
            tempMove += moves[i];
            tempMove += moves[i+1];
            tempMove += moves[i+2];
            tempMove += moves[i+3];

            //make move on BB's store data to string so move can be undone
            tempBBMove = newBBBoard->makeMove(tempMove);

            //jump to other color and evaluate all moves that don't cause a cutoff if depth is greater than 1
            bestTempMove = std::max(bestTempMove, miniMax(depth-1, alpha, beta,  ! isMaximisingPlayer, numberOfMoves));

            //undo move on BB's
            newBBBoard->unmakeMove(tempBBMove);

            //alpha beta pruning
            alpha = std::max(alpha, bestTempMove);

             //if move causes a beta cutoff (is bad for us) stop searching current branch
            if(beta <= alpha){
                //add beta eval to transpositon table
                addMoveTT(tempBBMove, depth, bestTempMove, 2);
                //update zobrist hash to correct color
                ZKey->UpdateColor();

                //push killer move to top of stack for given depth
                killerHArr[depth].push(tempMove);
                return bestTempMove;
            }
        }
        //add alpha eval to hash table
        addMoveTT(tempBBMove, depth, bestTempMove, 1);
        //update zobrist hash to correct color
        ZKey->UpdateColor();

        return bestTempMove;

    } else {
        long bestTempMove = 999999;

        moves = killerHe(depth, moves, false);
        //debug(lookUpTTable(depth), moves);

        for(int i = 0; i <  moves.length(); i+=4){
            std::string tempMove;
            //convert move into a single string
            tempMove += moves[i];
            tempMove += moves[i+1];
            tempMove += moves[i+2];
            tempMove += moves[i+3];

            //make move on BB
            tempBBMove = newBBBoard->makeMove(tempMove);

            //recursively test best move
            bestTempMove = std::min(bestTempMove, miniMax(depth-1, alpha, beta, ! isMaximisingPlayer, numberOfMoves));

            //undo move on BB
            newBBBoard->unmakeMove(tempBBMove);

            //alpha beta pruning
            beta = std::min(beta, bestTempMove);

            if(beta <= alpha){
                //add beta eval to transpositon table
                addMoveTT(tempBBMove, depth, bestTempMove, 2);
                //update zobrist hash to correct color
                ZKey->UpdateColor();

                killerHArr[depth].push(tempMove);
                return bestTempMove;
            }
        }
        //add alpha eval to transpositon table
        addMoveTT(tempBBMove, depth, bestTempMove, 1);
        //update zobrist hash to correct color
        ZKey->UpdateColor();

        return bestTempMove;

    }

}

std::string Ai_Logic::killerHe(int depth, std::string moves, bool isWhite)
{
    std::string cutoffs, tempMove, tempMove1;
    int size = killerHArr[depth].size();

    //if no killer moves, return the same list taken
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

long Ai_Logic::nullMovePruning(int depth, long alpha, long beta, bool isMaximisingPlayer)
{
    long move = miniMax(depth-2, alpha, beta, ! isMaximisingPlayer, 0);
    return move;
}

void Ai_Logic::addMoveTT(std::string bestmove, int depth, long eval, int flag)
{
    //get hash of current zobrist key
    int hash = (int)(zobKey % 1048583);

    if(transpositionT[hash].depth <= depth){
        //add position to the table
        transpositionT[hash].zobrist = zobKey;
        transpositionT[hash].depth = depth;
        transpositionT[hash].eval = (int)eval;
        transpositionT[hash].move = bestmove;
        transpositionT[hash].flag = flag;
    }

}

std::string Ai_Logic::lookUpTTable(int depth)
{
    //NEED TO ALTER LATER TO SKIP EVALUATING BEST MOVE AND USE ITS EVAL
    std::string tmove, move;
    //get hash of current zobrist key
    int hash = (int)(zobKey % 1048583);
    if(transpositionT[hash].depth >= depth){
        //if zobkey at both locations match (ie is same board position) return best move
        if(transpositionT[hash].zobrist == zobKey){
            tmove = transpositionT[hash].move;
            move += tmove[0];
            move += tmove[1];
            move += tmove[2];
            move += tmove[3];
            return move;
        }
    }
    return "";
}

long Ai_Logic::lookUpTTEval(int depth)
{
    int hash = (int)(zobKey % 1048583);
    long eval;
    if(transpositionT[hash].depth >= depth){
        //if zobkey at both locations match (ie is same board position) return best move
        if(transpositionT[hash].zobrist == zobKey){
            eval = transpositionT[hash].eval;
            return eval;
        }
    }
    return 999999;
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

int Ai_Logic::modifyDepth(int depth, int numberOfMoves)
{
    if(numberOfMoves <= 15){
        depth += 1;
    }
    if(numberOfMoves <= 6){
        depth += 1;
    }
    return depth;
}







