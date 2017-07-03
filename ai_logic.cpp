#include "ai_logic.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "bitboards.h"


//best overall move as calced
std::string bestMove;

//holds board state before any moves or trys
std::string board1[8][8];
//holds state of initial board + 1 move
std::string board2[8][8];


Evaluate *eval = new Evaluate;

BitBoards *newBBBoard = new BitBoards;

Ai_Logic::Ai_Logic()
{

}

std::string Ai_Logic::miniMaxRoot(int depth, bool isMaximisingPlayer)
{
    //generate all possible moves for one turn
    newBBBoard->constructBoards();

    //Move timer for ai
    clock_t aiMoveTimerStart = clock();

    //Store state of the board before any moves of the Ai turn have been done
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            board1[i][j] = boardArr[i][j];
        }
    }

    //generate first possible initial moves
    //newGenMoves->genMoves();
    std::string moves = newBBBoard->genWhosMove(false); //false for is not white, change later to be more versitile
    ///FOR DEBUGGING
    std::string a[moves.length()/4];
    for(int i = 0; i < moves.length(); i+=4){
        std::string tempMove;
        tempMove += moves[i];
        tempMove += moves[i+1];
        tempMove += moves[i+2];
        tempMove += moves[i+3];
        a[i/4] = tempMove;

    }

    //sorting not neccasary on first step?????
    int numberOfMoves = moves.length()/4;

    // ai temp values for passing to testBoardValues for assessing a board position
    int aiX, aiY, aiX1, aiY1;

    //for moves to compete against
    float bestMoveValue = -9999;

    //best move to return after all calcs
    std::string bestMoveFound;

    //if small number of root moves increase search depth
    //depth = modifyDepth(depth, numberOfMoves);

    //compare moves
    for(int i = 0; i < moves.length(); i+=4){
        std::string tempMove;
        //grab ints out of move
        aiX = (int)moves[i]-0;
        aiY = (int)moves[i+1]-0;
        aiX1 = (int)moves[i+2]-0;
        aiY1 = (int)moves[i+3]-0;
        //convert move into a single string
        tempMove += moves[i];
        tempMove += moves[i+1];
        tempMove += moves[i+2];
        tempMove += moves[i+3];

        //make move on BB ~~!!~~ later get rid of make move on array to increase speed
        std::string tempBBMove = newBBBoard->makeMove(tempMove);

        //change board to move in set of first moves
        boardArr[aiY1][aiX1] = boardArr[aiY][aiX];
        boardArr[aiY][aiX] = " ";

        //Store state of board after one of the first moves
        //so it can be un-done at the end of the recursive miniMax function,
        //each time to try a new move from the inital moves (possible_moves)
        for(int k = 0; k < 8; k++){
            for(int j = 0; j < 8; j++){
                board2[k][j] = boardArr[k][j];
            }
        }

        //test it's value and store it and test if white or black,
        float tempValue = miniMax(depth -1, -100000, 100000, ! isMaximisingPlayer, numberOfMoves);

        //undo move on BB
        newBBBoard->unmakeMove(tempBBMove);

        //Change board to state it was in before all testing of turn
        undo_move1();

        //if move is better then the best one store it
        if(tempValue >= bestMoveValue){
            bestMoveValue = tempValue;
            bestMoveFound = tempMove;

        }

    }

    clock_t aiEndMoveTImer = clock();

    std::cout << positionCount << std::endl;
    std::cout << (double) (aiEndMoveTImer - aiMoveTimerStart) / CLOCKS_PER_SEC << " seconds" << std::endl;
    possible_moves.clear();
    positionCount = 0;

    //make BB move final
    newBBBoard->makeMove(bestMoveFound);

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

    newBBBoard->drawBBA();
    std::cout << std::endl;

    return bestMoveFound;

}

float Ai_Logic::miniMax(float depth, float alpha, float beta, bool isMaximisingPlayer, int numberOfMoves)
{
    positionCount ++;
    int whiteMoves = 0;


    if(depth <= 0){
        return - eval->evaluateBoard(depth, numberOfMoves);
    }

    //moveGeneration *newGenMoves = new moveGeneration;
    std::string moves;

    if(isMaximisingPlayer == true){
       moves = newBBBoard->genWhosMove(false); //still just using bool values for what should be white or black dependant
    } else{
       moves = newBBBoard->genWhosMove(true);
    }


    //sort the best six moves into the first six slots of possible moves, improvmes speed by about 30% avg
    //moves = sortMoves(moves, isMaximisingPlayer);

    numberOfMoves = moves.length()/4;

    int x, y, x1, y1;

    if(isMaximisingPlayer == true){
        float bestTempMove = -99999;
        for(int i = 0; i < moves.length(); i+=4){
            //change board accoriding to i possible move
            std::string tempMove;
            //grab ints out of move
            x = (int)moves[i]-0;
            y = (int)moves[i+1]-0;
            x1 = (int)moves[i+2]-0;
            y1 = (int)moves[i+3]-0;
            //convert move into a single string
            tempMove += moves[i];
            tempMove += moves[i+1];
            tempMove += moves[i+2];
            tempMove += moves[i+3];


            //piece recovery for undo function
            std::string piece1 = boardArr[y][x], piece2 = boardArr[y1][x1];

            //make move on BB ~~!!~~ later get rid of make move on array to increase speed
            std::string tempBBMove = newBBBoard->makeMove(tempMove);

            //set board to test move value and move piece
            boardArr[y1][x1] = boardArr[y][x];
            boardArr[y][x] = " ";

            //recursively test best move
            bestTempMove = std::max(bestTempMove, miniMax(depth-1, alpha, beta,  ! isMaximisingPlayer, numberOfMoves));

            //undo move by passing it coordinates and pieces moved
            undoMove(x, y, x1, y1, piece1, piece2);

            //undo move on BB
            newBBBoard->unmakeMove(tempBBMove);

            //alpha beta pruning
            alpha = std::max(alpha, bestTempMove);

            if(beta <= alpha){

                //future_possible_moves.clear();
                return bestTempMove;
            }

        }


        //future_possible_moves.clear();
        return bestTempMove;


    } else {

        float bestTempMove = 99999;
        for(int i = 0; i <  moves.length(); i+=4){
            //whiteMoves ++;
            std::string tempMove;
            //grab ints out of move
            x = (int)moves[i]-0;
            y = (int)moves[i+1]-0;
            x1 = (int)moves[i+2]-0;
            y1 = (int)moves[i+3]-0;
            //convert move into a single string
            tempMove += moves[i];
            tempMove += moves[i+1];
            tempMove += moves[i+2];
            tempMove += moves[i+3];

            //piece recovery for undo function
            std::string piece1 = boardArr[y][x], piece2 = boardArr[y1][x1];

            //make move on BB ~~!!~~ later get rid of make move on array to increase speed
            std::string tempBBMove = newBBBoard->makeMove(tempMove);

            //set board to test move value and move piece
            boardArr[y1][x1] = boardArr[y][x];
            boardArr[y][x] = " ";

            //recursively test best move
            bestTempMove = std::min(bestTempMove, miniMax(depth-1, alpha, beta, ! isMaximisingPlayer, numberOfMoves));

            //undo move by passing it coordinates and pieces moved
            undoMove(x, y, x1, y1, piece1, piece2);

            //undo move on BB
            newBBBoard->unmakeMove(tempBBMove);

            //alpha beta pruning
            beta = std::min(beta, bestTempMove);

            if(beta <= alpha){

                //std::cout << whiteMoves << std::endl;
                //future_possible_moves.clear();
                return bestTempMove;
            }

        }

        //std::cout << whiteMoves << std::endl;
        //future_possible_moves.clear();
        return bestTempMove;

    }


}

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



void Ai_Logic::undoMove(int x, int y, int x1, int y1 , std::string piece1, std::string piece2){ //
    boardArr[y][x] = piece1;
    boardArr[y1][x1] = piece2;
}

void Ai_Logic::undo_move1(){
    //for undoing all test moves
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            boardArr[i][j] = board1[i][j];
        }
    }
}





