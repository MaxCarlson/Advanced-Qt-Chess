#ifndef ZOBRISTH_H
#define ZOBRISTH_H
#include "externs.h"

typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL

class ZobristH
{
public:
    ZobristH();

    U64 zArray[2][6][64];
    //U64 zEnPassasnt[8]; ~~restore once implemented
    //U64 zCastle[4];
    U64 zBlackMove;

    //generate unsigned 64 bit ints for hash mapping
    U64 random64();

    void zobristFill();

    U64 getZobristHash(bool isWhiteTurn);

    void testDistibution();

};

#endif // ZOBRISTH_H
