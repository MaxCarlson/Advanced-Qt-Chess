#include "evaluate.h"



int knight_adj[9] = { 25, 26, 26, 27, 28,  30,  30,  35, 40 };
//int knight_adj[9] = { -20, -16, -12, -8, -4,  0,  4,  8, 12};

int rook_adj[9] =   { 58, 57, 55, 53, 47, 45, 44, 43, 42 };
//int rook_adj[9] =   {  15,  12,   9,  6,  3,  0, -3, -6, -9};

int bishop_adj[9] = { 40, 39, 38, 37, 35, 32, 31, 31, 30 };

int nWhitePawns, nBlackPawns, nWhiteRooks, nBlackRooks, nWhiteKnights, nBlackKnights, nWhiteBishops, nBlackBishops;

Evaluate::Evaluate()
{

}

float Evaluate::evaluateBoard(float depth, int NumberOfMoves)
{
    //finding score of board
    float totalEvaluation = 0;
    //piece counts set to zero
    nWhitePawns = 0, nBlackPawns = 0, nWhiteRooks = 0, nBlackRooks = 0, nWhiteKnights = 0, nBlackKnights = 0, nWhiteBishops = 0, nBlackBishops = 0;


    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            totalEvaluation += getPieceValue(boardArr[i][j], i, j);
        }
    }

    //knight score number of pawn adjusts
    if(nWhiteKnights > 0){
        totalEvaluation += nWhiteKnights*knight_adj[nWhitePawns];
    }
    if(nBlackKnights > 0){
        totalEvaluation -= nBlackKnights*knight_adj[nBlackPawns];
    }

    //rook adjust based on pawn number
    if(nWhiteRooks > 0){
        totalEvaluation += nWhiteRooks*rook_adj[nWhitePawns];
    }
    if(nBlackRooks > 0){
        totalEvaluation -= nBlackRooks*rook_adj[nBlackPawns];
    }


    //bishop adjusts
    if(nWhiteBishops >= 2){
        totalEvaluation += 3.5;
        totalEvaluation += nWhiteBishops*bishop_adj[nWhitePawns];
    } else {
        totalEvaluation += nWhiteBishops*bishop_adj[nWhitePawns];
    }
    if(nBlackBishops >= 2){
        totalEvaluation -= 3.5;
        totalEvaluation -= nBlackBishops*bishop_adj[nBlackPawns];
    } else {
        totalEvaluation -= nBlackBishops*bishop_adj[nBlackPawns];
    }


    //add value for current mobility of position
    totalEvaluation *= (float)1+(0.01)*NumberOfMoves;

    //weight a higher depth more
    totalEvaluation *= 0.95*(depth+1);

    //return board eval
    return totalEvaluation;
}

float Evaluate::getPieceValue(std::string piece, int x, int y)
{
    //get value of piece on board
    if(piece == " "){
        return 0;
    }
    float absoluteValue = getAbsoluteValue(piece, x, y);

    //return negetive value if piece black, positive if white
    if(piece == "P" || piece == "R" || piece == "N" || piece == "B" || piece == "Q" || piece == "K"){
        //count number of pawns
        if(piece == "P"){
            nWhitePawns+=1;
            absoluteValue += evalPawn(x, y, absoluteValue, piece);
        } else if(piece == "R"){
            nWhiteRooks+=1;

        } else if(piece=="N"){
            nWhiteKnights+=1;

        } else if(piece=="B"){
            nWhiteBishops+=1;

        }else if(piece=="Q"){


        }else if(piece=="K"){

        }

        return absoluteValue;

    } else {
        if(piece == "p"){
            absoluteValue += evalPawn(x, y, absoluteValue, piece);
            nBlackPawns+=1;
        }else if(piece == "r"){

            nBlackRooks+=1;
        } else if(piece=="n"){

            nBlackKnights+=1;
        } else if(piece=="b"){

            nBlackBishops+=1;
        }else if(piece=="q"){


        } else if(piece=="k"){

        }
        return -absoluteValue;
    }

}

float Evaluate::evalPawn(int x, int y, float score, std::string piece)
{
    if(x == 0 || x == 7){
        score-=1.5;
    }
    return score;
}


//value spots for pieces
float pawnEvalWhite[8][8] =
{
    {0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
    {5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0},
    {1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0},
    {0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5},
    {0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0},
    {0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5},
    {0.5,  1.0, 1.0,  -2.0, -2.0,  1.0,  1.0,  0.5},
    {0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0}
};

float pawnEvalBlack[8][8] =
{
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0},
    {0.5, 1.0, 1.0,-2.0,-2.0, 1.0, 1.0,0.5},
    {0.5,-0.5,-1.0, 0.0, 0.0,-1.0,-0.5,0.5},
    {0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0,0.0},
    {0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5,0.5},
    {1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0,1.0},
    {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0,5.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0}

};


float knightEval[8][8] =
{
    {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
    {-4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0},
    {-3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0},
    {-3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0},
    {-3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0},
    {-3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0},
    {-4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0},
    {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}
};

float bishopEvalWhite[8][8] = {
    { -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},
    { -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0},
    { -1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0},
    { -1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0},
    { -1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0},
    { -1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0},
    { -1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0},
    { -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}
};

float bishopEvalBlack[8][8] = {
    {-2.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-2.0},
    {-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5,-1.0},
    {-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,-1.0},
    {-1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0,-1.0},
    {-1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5,-1.0},
    {-1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0,-1.0},
    {-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,-1.0},
    {-2.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-2.0}
};

float rookEvalWhite[8][8] = {
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
    {  0.5,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  0.5},
    { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
    { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
    { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
    { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
    { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
    {  0.0,   0.0, 0.0,  0.5,  0.5,  0.0,  0.0,  0.0}
};

float rookEvalBlack[8][8] = {
    { 0.0,0.0,0.0,0.5,0.5,0.0,0.0, 0.0},
    {-0.5,0.0,0.0,0.0,0.0,0.0,0.0,-0.5},
    {-0.5,0.0,0.0,0.0,0.0,0.0,0.0,-0.5},
    {-0.5,0.0,0.0,0.0,0.0,0.0,0.0,-0.5},
    {-0.5,0.0,0.0,0.0,0.0,0.0,0.0,-0.5},
    {-0.5,0.0,0.0,0.0,0.0,0.0,0.0,-0.5},
    { 0.5,1.0,1.0,1.0,1.0,1.0,1.0, 0.5},
    { 0.0,0.0,0.0,0.0,0.0,0.0,0.0, 0.0}
};

float evalQueen[8][8] = {
    { -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0},
    { -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0},
    { -1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0},
    { -0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5},
    {  0.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5},
    { -1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0},
    { -1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0},
    { -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0}
};

float kingEvalWhite[8][8] = {

    { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
    { -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
    { -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
    {  2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0},
    {  2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0}
};

float kingEvalBlack[8][8] = {

    {2.0 , 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0},
    {2.0 , 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0},
    {-1.0,-2.0,-2.0,-2.0,-2.0,-2.0,-2.0,-1.0},
    {-2.0,-3.0,-3.0,-4.0,-4.0,-3.0,-3.0,-2.0},
    {-3.0,-4.0,-4.0,-5.0,-5.0,-4.0,-4.0,-3.0},
    {-3.0,-4.0,-4.0,-5.0,-5.0,-4.0,-4.0,-3.0},
    {-3.0,-4.0,-4.0,-5.0,-5.0,-4.0,-4.0,-3.0},
    {-3.0,-4.0,-4.0,-5.0,-5.0,-4.0,-4.0,-3.0}
};

float Evaluate::getAbsoluteValue(std::string piece, int x, int y)
{
    //find which piece it is and return its board value
    //black
    if(piece == "p"){
        return 10 + pawnEvalBlack[y][x];
    } else if (piece =="r"){
        //adjusts for knight and rook done in evaluate board due to needing pawn numbers for value
        return 0 + rookEvalBlack[y][x];
    }else if (piece =="n"){
        return 0 + knightEval[y][x];
    }else if (piece =="b"){
        return 0 + bishopEvalBlack[y][x];
    }else if (piece =="q"){
        return 90 + evalQueen[y][x];
    }else if (piece =="k"){
        return 900 + kingEvalBlack[y][x];
    //white
    }else if(piece == "P"){
        return 10 + pawnEvalWhite[y][x];
    } else if (piece =="R"){
        return 0 + rookEvalWhite[y][x] ;
    }else if (piece =="N"){
        return 0 + knightEval[y][x];
    }else if (piece =="B"){
        return 0 + bishopEvalWhite[y][x];
    }else if (piece =="Q"){
        return 90 + evalQueen[y][x];
    }else if (piece =="K"){
        return 900 + kingEvalWhite[y][x];
    } else {
        return 0;
    }
}
