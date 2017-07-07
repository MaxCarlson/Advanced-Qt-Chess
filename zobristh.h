#ifndef ZOBRISTH_H
#define ZOBRISTH_H


typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL

class ZobristH
{
public:
    ZobristH();




    //generate unsigned 64 bit ints for hash mapping
    U64 random64();
    //fill zorbist array with random 64bit numbers
    void zobristFill();
    //get zorbist key by XOR ing all pieces random numbers with Zkey
    U64 getZobristHash(bool isWhiteTurn);

    void testDistibution();

};

#endif // ZOBRISTH_H
