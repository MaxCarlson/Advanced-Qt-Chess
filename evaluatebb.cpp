#include "evaluatebb.h"
#include "externs.h"

const U64 full  = 0xffffffffffffffffULL;

/*
piece values
  P = 100
  N = 320
  B = 330
  R = 500
  Q = 900
  K = 19000
*/

evaluateBB::evaluateBB()
{

}

int evaluateBB::evalBoard(bool isWhite)
{
    int totalEvaualtion = 0;

    for(int i = 0; i < 64; i++){
        totalEvaualtion += getPieceValue(i);
    }

    if(isWhite){
        return totalEvaualtion;
    } else {
        return -totalEvaualtion;
    }

}

int evaluateBB::returnMateScore(bool isWhite)
{
    if(isWhite){
        return 32000;
    } else {
        return 32000;
    }
}

//white piece square lookup tables
int wPawnsSqT[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

int wKnightsSqT[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

int wBishopsSqT[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

int wRooksSqT[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

int wQueensSqt[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

int wKingMidSqT[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

int wKingEndSqT[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

//black piece square lookup tables
int bPawnSqT[64] = {
     0,  0,   0,   0,   0,   0,  0,   0,
     5, 10,  10, -20, -20,  10, 10,   5,
     5, -5, -10,   0,   0, -10, -5,   5,
     0,  0,   0,  20,  20,   0,  0,   0,
     5,  5,  10,  25,  25,  10,  5,   5,
    10, 10,  20,  30,  30,  20,  10, 10,
    50, 50,  50,  50,  50,  50,  50, 50,
     0,  0,   0,   0,   0,   0,   0,  0,

};

int bKnightSqT[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50,

};

int bBishopsSqT[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -20, -10, -10, -10, -10, -10, -10, -20,

};

int bRookSqT[64] = {
     0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5, 10, 10, 10, 10, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0,

};

int bQueenSqT[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10,   0,   0,  0,  0,   5,   0, -10,
    -10,   0,   5,  5,  5,   5,   5, -10,
    -5,    0,   5,  5,  5,   5,   0,   0,
    -5,    0,   5,  5,  5,   5,   0,  -5,
    -10,   0,   5,  5,  5,   5,   0, -10,
    -10,   0,   0,  0,  0,   0,   0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20,
};

int bKingMidSqT[64]= {
     20,  30,  10,   0,   0,  10,  30,  20,
     20,  20,   0,   0,   0,   0,  20,  20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
};

int bKingEndSqT[64] = {
    -50, -30, -30, -30, -30, -30, -30, -50,
    -30, -30,   0,   0,   0,   0, -30, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -20, -10,   0,   0, -10, -20, -30,
    -50, -40, -30, -20, -20, -30, -40, -50,
};

int evaluateBB::getPieceValue(int location)
{
    //create an empty board then shift a 1 over to the current i location
    U64 pieceLocation = 0LL;
    pieceLocation += 1LL<<location;

    //white pieces
    if(BBWhitePieces & pieceLocation){
        if(pieceLocation & BBWhitePawns){
            return 100 + wPawnsSqT[location];
        } else if(pieceLocation & BBWhiteRooks){
            return 500 + wRooksSqT[location];
        } else if(pieceLocation & BBWhiteKnights){
            return 320 + wKnightsSqT[location];
        } else if(pieceLocation & BBWhiteBishops){
            return 330 + wBishopsSqT[location];
        } else if(pieceLocation & BBWhiteQueens){
            return 900 + wQueensSqt[location];
        } else if(pieceLocation & BBWhiteKing){
            //If both sides have no queens use king end game board
            if((BBWhiteQueens | BBBlackQueens) & full){
                return 19000 + wKingEndSqT[location];
            }
            //if end game conditions fail use mid game king board
            return 19000 + wKingMidSqT[location];

        }
    } else if (BBBlackPieces & pieceLocation) {
        if(pieceLocation & BBBlackPawns ){
            return -100 -bPawnSqT[location];
        } else if(pieceLocation & BBBlackRooks){
            return -500 -bRookSqT[location];
        } else if(pieceLocation & BBBlackKnights){
            return -320 -bKnightSqT[location];
        } else if(pieceLocation & BBBlackBishops){
            return -330 -bBishopsSqT[location];
        } else if(pieceLocation & BBBlackQueens){
            return -900 -bQueenSqT[location];
        } else if(pieceLocation & BBBlackKing){
            if((BBBlackQueens | BBWhiteQueens) & full){
                return -19000 -bKingEndSqT[location];
            }
         return -19000 -bKingMidSqT[location];
        }
    }
    return 0;
}

