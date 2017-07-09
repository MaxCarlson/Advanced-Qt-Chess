#ifndef ZOBRISTH_H
#define ZOBRISTH_H
#include <string>

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

    //Update zobrist key by XOR ing rand numbers in zArray
    void UpdateKey(int start, int end, std::string moveKey);
    void UpdateColor();

    //print out how many times a number in the array is created
    //use excel to graph results
    void testDistibution();

    U64 debugKey(bool isWhite);

};

#endif // ZOBRISTH_H
