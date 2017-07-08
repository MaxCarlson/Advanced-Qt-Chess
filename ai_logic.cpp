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

}

std::string Ai_Logic::miniMaxRoot(int depth, bool isMaximisingPlayer)
{

    //generate all possible moves for one turn
    newBBBoard->constructBoards();

    std::cout << zobKey << std::endl;

    //Move timer for ai
    clock_t aiMoveTimerStart = clock();

    //generate first possible initial moves
    std::string tmoves = newBBBoard->genWhosMove(false); //false for is not white, change later to be more versitile
    std::string moves, tempBBMove;

    //sorting not neccasary on first step?????
    int numberOfMoves = moves.length()/4;

    //for moves to compete against
    long bestMoveValue = -999999, tempValue;


    //best move to return after all calcs
    std::string bestMoveFound;

    //lookup whether board position has been input into TTable if so add it to front of moves
    //moves = lookUpTTable(depth);
    //moves += tmoves;

    moves = debug(lookUpTTable(depth), tmoves);

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
    addMoveTT(tempBBMove, depth, tempValue);

    //postion count and time it took to find move
    clock_t aiEndMoveTImer = clock();    
    std::cout << positionCount << std::endl;
    std::cout << (double) (aiEndMoveTImer - aiMoveTimerStart) / CLOCKS_PER_SEC << " seconds" << std::endl;
    possible_moves.clear();
    positionCount = 0;

    //make BB move final
    newBBBoard->makeMove(bestMoveFound);

    /*
    ////TESTTEST
    if(FullTiles & EmptyTiles){
        std::cout << "Full and empty off again here!!!" << std::endl;
         newBBBoard->drawBB(FullTiles);
         newBBBoard->drawBB(EmptyTiles);
         newBBBoard->drawBB(FullTiles & EmptyTiles);
         newBBBoard->drawBBA();
    }

    if(EmptyTiles & BBWhiteRooks){
        std::cout << "wrooks off again here!!!" << std::endl;
        newBBBoard->drawBB(BBWhiteRooks);
        newBBBoard->drawBB(EmptyTiles);
    }

    if(EmptyTiles & BBBlackRooks){
        std::cout << "brooks off again here!!!" << std::endl;
        newBBBoard->drawBB(BBBlackRooks);
        newBBBoard->drawBB(EmptyTiles);
    }
    if(EmptyTiles & BBWhitePieces){
        std::cout << "whitepieces off again here!!!" << std::endl;
        newBBBoard->drawBB(BBWhitePieces);
        newBBBoard->drawBB(EmptyTiles);
    }
    if(EmptyTiles & BBBlackPieces){
        std::cout << "blackpieces off again here!!!" << std::endl;
        newBBBoard->drawBB(BBBlackPieces);
        newBBBoard->drawBB(EmptyTiles);
    }
    */


    newBBBoard->drawBBA();
    std::cout << std::endl;
    //update zobrist hash to correct color
    ZKey->UpdateColor();

    return bestMoveFound;

}

long Ai_Logic::miniMax(int depth, long alpha, long beta, bool isMaximisingPlayer, int numberOfMoves)
{
    positionCount ++;

    if(depth <= 0){
        return - eval->evalBoard();
    }

    std::string moves, tmoves, tempBBMove;

    if(isMaximisingPlayer == true){
       moves = newBBBoard->genWhosMove(false); //still just using bool values for what should be white or black dependant
    } else{
       moves = newBBBoard->genWhosMove(true);
    }
    //possible sorting later??

    if(isMaximisingPlayer == true){
        long bestTempMove = -999999;
        //push  killer (better) moves to (eventually just near) front of move list
        //in order to get a high cutoff as fast as possible
        tmoves = killerHe(depth, moves, false);

        //update zobrist hash to correct color
        ZKey->UpdateColor();
        //lookup whether board position has been input into TTable if so add it to front of moves
        //moves = lookUpTTable(depth);
        //moves += tmoves;
        //debug
        moves = debug(lookUpTTable(depth), tmoves);

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
                //add best move to transpositon table
                addMoveTT(tempBBMove, depth, bestTempMove);
                //update zobrist hash to correct color
                ZKey->UpdateColor();

                //push killer move to top of stack for given depth
                killerHArr[depth].push(tempMove);
                return bestTempMove;
            }
        }
        //add best move to transpositon table
        addMoveTT(tempBBMove, depth, bestTempMove);
        //update zobrist hash to correct color
        ZKey->UpdateColor();

        return bestTempMove;

    } else {
        long bestTempMove = 999999;

        tmoves = killerHe(depth, moves, false);
        //update zobrist hash to correct color
        ZKey->UpdateColor();

        //lookup whether board position has been input into TTable if so add it to front of moves
        //moves = lookUpTTable(depth);
        //moves += tmoves;
        //debug
        moves = debug(lookUpTTable(depth), tmoves);

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
                //add best move to transpositon table
                addMoveTT(tempBBMove, depth, bestTempMove);
                //update zobrist hash to correct color
                ZKey->UpdateColor();

                killerHArr[depth].push(tempMove);
                return bestTempMove;
            }
        }
        //add best move to transpositon table
        addMoveTT(tempBBMove, depth, bestTempMove);
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

void Ai_Logic::addMoveTT(std::string bestmove, int depth, long eval)
{
    //get hash of current zobrist key
    int hash = (int)(zobKey % 1048583);
    //testing
    //newBBBoard->drawBBA();

    if(transpositionT[hash].depth <= depth){

        //add position to the table
        transpositionT[hash].zobrist = zobKey;
        transpositionT[hash].depth = depth;
        transpositionT[hash].eval = (int)eval;
        transpositionT[hash].move = bestmove;
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

/*
std::vector<std::string> Ai_Logic::sortMoves(std::vector<std::string> moves, bool isMaximisingPlayer)
{
    //float vector to hold scores
    std::vector<float> score;
    int x, y, x1, y1;
    int tmp = moves.size();

    for(int i = 0; i < tmp; i++){
        //change board accoriding to i possible move
        std::string tempMove = moves[i];
        x = (int)tempMove[0]-'0';
        y = (int)tempMove[1]-'0';
        x1 = (int)tempMove[2]-'0';
        y1 = (int)tempMove[3]-'0';

        //piece recovery for undo function
        std::string piece1 = boardArr[y][x], piece2 = boardArr[y1][x1];

        //set board to test move value and move piece
        boardArr[y1][x1] = boardArr[y][x];
        boardArr[y][x] = " ";

        //evaluate board and push it to the float vector
        score.push_back(eval->evaluateBoard(1, 0));

        //undo the move
        undoMove(x, y, x1, y1, piece1, piece2);

    }
    std::vector<std::string> newListA, newListB=moves, returnVec;


    //first few moves only
    for(int i = 0; i <std::min(6, tmp); i++){
        float max;
        int maxLocation=0;
        //set high low score based on what's good for player being analysed
        if(isMaximisingPlayer == false){
            max = -100000;
        } else{
            max = 100000;
        }
        for(int j = 0; j < tmp; j++){
            //if score is better than max record it's location and remove it from being searched
            if(score[j] > max && isMaximisingPlayer == false){
                max = score[j];
                maxLocation = j;
                score[j] = -100000;
            } else if (score[j] < max && isMaximisingPlayer == true){
                max = score[j];
                maxLocation = j;
                score[j] = 100000;
            }
        }
        newListA.push_back(moves[maxLocation]);
        newListB.erase(newListB.begin()+maxLocation);
    }
    newListA.insert(newListA.end(), newListB.begin(), newListB.end());

    return newListA;

}
*/

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







