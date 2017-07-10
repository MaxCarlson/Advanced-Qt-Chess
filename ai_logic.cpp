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

bool searchCutoff = false;

Ai_Logic::Ai_Logic()
{
    //generate all possible moves for one turn
    newBBBoard->constructBoards();
    //once opponent move is made update the zorbist hash key
    ZKey->getZobristHash(true);
    //update zobrsit hash to correct color
    ZKey->UpdateColor();
}

std::string Ai_Logic::iterativeDeep(int depth)
{

    //iterative deepening start time
    clock_t IDTimeS = clock();

    //time limmit in miliseconds
    int timeLimmit = 11500;

    long endTime = IDTimeS + timeLimmit;

    searchCutoff = false;

    positionCount = 0;
    std::string bestMove, tBestMove;

    std::cout << zobKey << std::endl;

    int distance = 1;

    for(distance; distance <= depth && IDTimeS < endTime; distance++){
        positionCount = 0;
        clock_t currentTime = clock();
        if(currentTime >= endTime){
            //distance -= 1;
            break;
        }

        tBestMove = miniMaxRoot(distance, true, currentTime, endTime - currentTime);

        //if the search is not cutoff
        if(!searchCutoff){
            bestMove = tBestMove;
        } else {
            //distance -= 1;
            break;
        }
    }
    //std::cout << zobKey << std::endl;

    //make final move on bitboards
    newBBBoard->makeMove(bestMove);

    newBBBoard->drawBBA();

    clock_t IDTimeE = clock();
    //postion count and time it took to find move
    std::cout << positionCount << " positions searched." << std::endl;
    std::cout << (double) (IDTimeE - IDTimeS) / CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Depth of " << distance-1 << " reached."<<std::endl;
    //std::cout << zobKey << std::endl;

    //push best move of this iteration to stack
    PVMoves[depth] = bestMove;

    return bestMove;
}

std::string Ai_Logic::miniMaxRoot(int depth, bool isMaximisingPlayer, long currentTime, long timeLimmit)
{
    int currentDepth = 1;

    //for moves to compete against
    int bestMoveValue = -999999, tempValue;

    std::string bestMoveFound, moves, tempBBMove;

    if(PVMoves[currentDepth].length() == 4){
        moves = PVMoves[currentDepth];
    }

    //create unqiue hash from zobkey
    int hash = (int)(zobKey % 15485867);
    HashEntry entry = transpositionT[hash];

    //if the depth of the stored evaluation is greater/= and the zobrist key matches
    if(entry.depth >= currentDepth && entry.zobrist == zobKey){
        //return either the eval, the beta, or the alpha depending on flag
        if(entry.flag == 3){
            moves = entry.move;
        }
    }

    //generate first possible initial moves
    moves += newBBBoard->genWhosMove(false); //false for is not white, change later to be more versitile

    //compare moves
    for(int i = 0; i < moves.length(); i+=4){
        positionCount ++;
        std::string tempMove;
        //convert move into a single string
        tempMove += moves[i];
        tempMove += moves[i+1];
        tempMove += moves[i+2];
        tempMove += moves[i+3];

        //make move on BB
        tempBBMove = newBBBoard->makeMove(tempMove);

        //perform alpha beta search
        tempValue = -alphaBeta(depth -1, -100000, 100000, isMaximisingPlayer, currentTime, timeLimmit, currentDepth +1);

        //tempValue = -principleV(depth -1, -100000, 100000, isMaximisingPlayer);
        //tempValue = miniMax(depth -1, -100000, 100000, ! isMaximisingPlayer, currentTime, timeLimmit);

        //undo move on BB
        newBBBoard->unmakeMove(tempBBMove);

        //if move is better then the best one store it
        if(tempValue >= bestMoveValue){
            bestMoveValue = tempValue;
            bestMoveFound = tempMove;

        }

    }

    if(ZKey->debugKey(false) != zobKey){
        std::cout << ZKey->debugKey(false) << std::endl;
        std::cout << zobKey << std::endl;
    }

    //add best move to transpositon table
    addMoveTT(bestMoveFound, depth, tempValue, 3);

    PVMoves[currentDepth] = bestMoveFound;

    return bestMoveFound;

}

int Ai_Logic::alphaBeta(int depth, long alpha, long beta, bool isMaximisingPlayer, long currentTime, long timeLimmit, int currentDepth)
{
    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;

    //create unqiue hash from zobkey
    int hash = (int)(zobKey % 15485867);
    HashEntry entry = transpositionT[hash];


    //if the depth of the stored evaluation is greater and the zobrist key matches
    if(entry.depth >= depth && entry.zobrist == zobKey){
        //return either the eval, the beta, or the alpha depending on flag
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

    if(elapsedTime >= timeLimmit){
        searchCutoff = true;
    }

    if(depth == 0 || searchCutoff){
        //evaluate board position
        int score = eval->evalBoard();

        //add move to hash table with exact flag
        addMoveTT("0", depth, score, 3);
        return score;
    }

    std::string moves, tempBBMove, pv;
    int bestTempMove;

    //generate normal moves
    moves = newBBBoard->genWhosMove(isMaximisingPlayer);
    /*
    //find out if there is a pv move for current depth
    if(PVMoves[currentDepth].length() == 4){
        pv = PVMoves[currentDepth];
    }
    */

    //push pv to front and after pv killer (better) moves to  move list
    //in order to get a high cutoff as fast as possible
    moves = killerHe(currentDepth, moves, isMaximisingPlayer, pv);

    for(int i = 0; i < moves.length(); i+=4){
        positionCount ++;
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
        bestTempMove = -alphaBeta(depth-1, -beta, -alpha,  ! isMaximisingPlayer, currentTime, timeLimmit, currentDepth +1);

        //undo move on BB's
        newBBBoard->unmakeMove(tempBBMove);
        //if move causes a beta cutoff (is bad for us) stop searching current branch
        if(bestTempMove >= beta){
            //add beta eval to transpositon table
            addMoveTT(tempBBMove, depth, bestTempMove, 2);

            //push killer move to top of stack for given depth
            killerHArr[currentDepth].push(tempMove);
            return beta;
        }

        if(bestTempMove > alpha){
            //add alpha eval to hash table
            addMoveTT(tempBBMove, depth, bestTempMove, 1);
            //new best move
            alpha = bestTempMove;
            //store principal variation
            PVMoves[currentDepth] = tempMove;
        }
    }

    return alpha;
}

std::string Ai_Logic::killerHe(int depth, std::string moves, bool isWhite, std::string PV)
{
    std::string cutoffs, tempMove, tempMove1;
    int size = killerHArr[depth].size();

    //if no killer moves, return the same list taken
    if(size == 0){
        return PV+moves;
    }

    cutoffs = PV;

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

}
/*
int Ai_Logic::quiescent(int alpha, int beta)
{
    //NEEDS SERIOUS WORK
    eval = -eval->evalBoard();

    if(eval >= beta){
        return beta;
    }
    if(eval > alpha){
        alpha = eval;
    }

    //Generate captues ~~ NEED to implement
    std::string captures;

    for(int i = 0; i < captures.length(); i+= 4){
        std::string move;
        move += captures[i];
        move += captures[i+1];
        move += captures[i+2];
        move += captures[i+3];
        std::string tempMove = newBBBoard->makeMove(move);
        eval;
    }


}
*/
void Ai_Logic::addMoveTT(std::string bestmove, int depth, long eval, int flag)
{
    //get hash of current zobrist key
    int hash = (int)(zobKey % 15485867);

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
    int hash = (int)(zobKey % 15485867);
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
    int hash = (int)(zobKey % 15485867);
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

int Ai_Logic::principleV(int depth, int alpha, int beta, bool isWhite)
{
    int bestScore;
    int bestMoveIndex = 0;


    if(depth == 0){
        int bestScore = eval->evalBoard();
        return bestScore;
    }

    std::string moves, tempBBMove;

    moves = newBBBoard->genWhosMove(isWhite);


    //Do full depth search of first move
    std::string tempMove;
    //convert move into a single string
    tempMove += moves[0];
    tempMove += moves[1];
    tempMove += moves[2];
    tempMove += moves[3];

    //make move on BB's store data to string so move can be undone
    tempBBMove = newBBBoard->makeMove(tempMove);

    //jump to other color and evaluate all moves that don't cause a cutoff if depth is greater than 1
    bestScore = -principleV(depth-1, -beta, -alpha,  ! isWhite);

    positionCount ++;

    //undo move on BB's
    newBBBoard->unmakeMove(tempBBMove);

    //NEED a mate check here

    //if the moves is an improvement
    if(bestScore > alpha){
        //if move is too good to be true
        if(bestScore >= beta){
            return bestScore;
        }
        //if it's better but not too good to be true
        alpha = bestScore;
    }


    //Do a shallower zero window search of moves other than the first, if they're better
    //run a full PV search
    for(int i = bestMoveIndex+4; i < moves.length(); i += 4){
        int score = -999999;
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
        score = -zWSearch(depth -1, -alpha, !isWhite);

        //unmake move
        newBBBoard->unmakeMove(tempBBMove);

        if((score > alpha) && (score < beta)){

            bestScore = -principleV(depth-1, -beta, -alpha, !isWhite);

            if(score > alpha){
                bestMoveIndex = i;
                alpha = score;
            }
        }
        if(score != -999999 && score > bestScore){

            if(score >= beta){
                return score;
            }

            bestScore = score;

            //need checkmate check
        }
    }
     return bestScore;
}

int Ai_Logic::zWSearch(int depth, int beta, bool isWhite)
{
    int score = -999999;
    //alpha = beta-1;

    if(depth == 0){
        score = -eval->evalBoard();
        return score;
    }

    std::string moves, tempBBMove;
    moves = newBBBoard->genWhosMove(isWhite);

    for(int i = 4; i < moves.length(); i += 4){
        std::string move;
        //convert move into a single string
        move += moves[i];
        move += moves[i+1];
        move += moves[i+2];
        move += moves[i+3];

        tempBBMove = newBBBoard->makeMove(move);

        score = -zWSearch(depth - 1, 1 - beta, ! isWhite);

        newBBBoard->unmakeMove(tempBBMove);

        if(score >= beta){

            return score; // fail hard beta cutoff
        }

    }
    return beta-1; //same as alpha
}

/*
long Ai_Logic::miniMax(int depth, long alpha, long beta, bool isMaximisingPlayer, long currentTime, long timeLimmit)
{
    //iterative deeping timer stuff
    clock_t time = clock();
    long elapsedTime = time - currentTime;

    //update zobrist hash to correct color
    //ZKey->UpdateColor();

    //create unqiue hash from zobkey
    int hash = (int)(zobKey % 15485867);
    HashEntry entry = transpositionT[hash];

    //if the depth of the stored evaluation is greater and the zobrist key matches
    if(entry.depth >= depth && entry.zobrist == zobKey){
        //return either the eval, the beta, or the alpha depending on flag
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
    if(elapsedTime >= timeLimmit){
        searchCutoff = true;
    }

    if(depth == 0 || searchCutoff){
        //evaluate board position
        long score = - eval->evalBoard();

        //add move to hash table with exact flag
        addMoveTT("0", depth, score, 3);
        return score;
    }

    std::string moves, tempBBMove;

    moves = newBBBoard->genWhosMove(!isMaximisingPlayer);


    if(isMaximisingPlayer == true){
        long bestTempMove = -999999;
        //push  killer (better) moves to (eventually just near) front of move list
        //in order to get a high cutoff as fast as possible
        moves = killerHe(depth, moves, false);

        if(ZKey->debugKey(false) != zobKey){
            std::cout << ZKey->debugKey(false) << std::endl;
            std::cout << zobKey << std::endl;
        }

        for(int i = 0; i < moves.length(); i+=4){
            positionCount ++;
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
            bestTempMove = std::max(bestTempMove, miniMax(depth-1, alpha, beta,  ! isMaximisingPlayer, currentTime, timeLimmit));

            //undo move on BB's
            newBBBoard->unmakeMove(tempBBMove);

            //alpha beta pruning
            alpha = std::max(alpha, bestTempMove);

             //if move causes a beta cutoff (is bad for us) stop searching current branch
            if(beta <= alpha){
                //add beta eval to transpositon table
                addMoveTT(tempBBMove, depth, bestTempMove, 2);

                //push killer move to top of stack for given depth
                killerHArr[depth].push(tempMove);
                return bestTempMove;
            }
        }
        //add alpha eval to hash table
        addMoveTT(tempBBMove, depth, bestTempMove, 1);

        return bestTempMove;

    } else {
        long bestTempMove = 999999;

        moves = killerHe(depth, moves, false);
        //debug(lookUpTTable(depth), moves);

        if(ZKey->debugKey(true) != zobKey){
            std::cout << ZKey->debugKey(true) << std::endl;
            std::cout << zobKey << std::endl;
        }

        for(int i = 0; i <  moves.length(); i+=4){
            positionCount ++;
            std::string tempMove;
            //convert move into a single string
            tempMove += moves[i];
            tempMove += moves[i+1];
            tempMove += moves[i+2];
            tempMove += moves[i+3];

            //make move on BB
            tempBBMove = newBBBoard->makeMove(tempMove);

            //recursively test best move
            bestTempMove = std::min(bestTempMove, miniMax(depth-1, alpha, beta, ! isMaximisingPlayer, currentTime, timeLimmit));

            //undo move on BB
            newBBBoard->unmakeMove(tempBBMove);

            //alpha beta pruning
            beta = std::min(beta, bestTempMove);

            if(beta <= alpha){
                //add beta eval to transpositon table
                addMoveTT(tempBBMove, depth, bestTempMove, 2);

                killerHArr[depth].push(tempMove);
                return bestTempMove;
            }
        }
        //add alpha eval to transpositon table
        addMoveTT(tempBBMove, depth, bestTempMove, 1);

        return bestTempMove;

    }

}





*/
