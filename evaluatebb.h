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

    int whitePawnCount = 0;
    int blackPawnCount = 0;

    //gets rudimentry value of piece + square table value
    int getPieceValue(int location, BitBoards *BBBoard);


};

#endif // EVALUATEBB_H
