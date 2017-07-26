#ifndef EVALUATEBB_H
#define EVALUATEBB_H

#include "bitboards.h"

typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL


class evaluateBB
{
public:
    evaluateBB();

    //forms total evaluation for baord state
    int evalBoard(bool isWhite, BitBoards *BBBoard);

    //returns mate or stalemate score
    int returnMateScore(bool isWhite, BitBoards *BBBoard, int depth);

private:
    //meterial value representing wheter we call end game/mid game
    int gamePhase = 0;

    int whitePawnCount = 0;
    int blackPawnCount = 0;

    //gets rudimentry value of piece + square table value
    int getPieceValue(int location, BitBoards *BBBoard);

    //generate zone around king ///Up for debate as to how large zone should be, currently encompasses 8 tiles directly around king
    //currently includes blocking pieces in zone
    U64 generateKingZone(bool isWhite, BitBoards *BBBoard);
        U64 wKingZ;
        U64 bKingZ;

        //attack count and weight against king by side, 0 is white attacking black king
        int attCount[2];
        int attWeight[2];

//piece evaluation for mobility, attacking king squares, etc
    //mid and end game mobility by color, 0 for white
    int midGMobility[2];
    int endGMobility[2];

    void pawnEval(bool isWhite, int location, BitBoards *BBBoard);

    void evalKnight(bool isWhite, int location, BitBoards *BBBoard);

    void evalBishop(bool isWhite, int location, BitBoards *BBBoard);

    void evalRook(bool isWhite, int location, BitBoards *BBBoard);

    void evalQueen(bool isWhite, int location, BitBoards *BBBoard);


};

#endif // EVALUATEBB_H
