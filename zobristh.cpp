#include "zobristh.h"

#include <cmath>
#include <random>
#include <iostream>

std::random_device rd;

std::mt19937_64 mt(rd());

std::uniform_int_distribution<long long int> dist(std::llround(std::pow(2,61)), std::llround(std::pow(2,62)));

ZobristH::ZobristH()
{

}

U64 ZobristH::random64()
{
    long long int ranUI = dist(mt);
    //std::cout << ranUI << ",   ";
    //long long int ranUI = rand();
    return ranUI;
}

void ZobristH::zobristFill()
{
    //fill zorbist array with random unisgned 64 bit ints
    for(int color = 0; color < 2; color++){
        for(int pieceType = 0; pieceType < 6; pieceType ++){
            for(int square = 0; square < 64; square ++){
                zArray[color][pieceType][square] = random64();
            }
        }

    }
    //enpassant and castle filling below
    /*
    for (int column = 0; column < 8; column++)
    {
        zEnPassant[column] = random64();
    }
    for (int i = 0; i < 4; i++)
    {
        zCastle[i] = random64();
    }
    */
    //random is it blacks turn or not
    zBlackMove = random64();
}

U64 ZobristH::getZobristHash(bool isWhiteTurn)
{
    U64 returnZKey = 0LL;
    for (int square = 0; square < 64; square++){
        //white pieces
        //if there is a white pawn on i square
        if(((BBWhitePawns >> square) & 1) == 1)
        {
            //XOR the zkey with the U64 in the white pawns square
            //that was generated from rand64
            returnZKey ^= zArray[0][0][square];
        }
        else if(((BBWhiteRooks >> square) & 1) == 1)
        {
            returnZKey ^= zArray[0][1][square];
        }
        else if(((BBWhiteKnights >> square) & 1) == 1)
        {
            returnZKey ^= zArray[0][2][square];
        }
        else if(((BBWhiteBishops >> square) & 1) == 1)
        {
            returnZKey ^= zArray[0][3][square];
        }
        else if(((BBWhiteQueens >> square) & 1) == 1)
        {
            returnZKey ^= zArray[0][4][square];
        }
        else if(((BBWhiteKing >> square) & 1) == 1)
        {
            returnZKey ^= zArray[0][5][square];
        }
        //black pieces
        else if(((BBBlackPawns >> square) & 1) == 1)
        {
            returnZKey ^= zArray[1][0][square];
        }
        else if(((BBBlackRooks >> square) & 1) == 1)
        {
            returnZKey ^= zArray[1][1][square];
        }
        else if(((BBBlackKnights >> square) & 1) == 1)
        {
            returnZKey ^= zArray[1][2][square];
        }
        else if(((BBBlackBishops >> square) & 1) == 1)
        {
            returnZKey ^= zArray[1][3][square];
        }
        else if(((BBBlackQueens >> square) & 1) == 1)
        {
            returnZKey ^= zArray[1][4][square];
        }
        else if(((BBBlackKing >> square) & 1) == 1)
        {
            returnZKey ^= zArray[1][5][square];
        }
    }
    //EnPassant and castling stuff

    //if it isn't whites turn, XOR zobrist key with black move U64
    if(!isWhiteTurn){
        returnZKey ^= zBlackMove;
    }

    return returnZKey;
}

void ZobristH::testDistibution()
{
    const int sampleSize = 2000;
    int distArray[sampleSize] = {};
    int t = 0;
    while (t < 1500)
    {
       for (int i = 0; i < 2000; i++)
       {
           distArray[(int)(random64() % sampleSize)]++;
       }
       t++;
    }
    for (int i = 0; i < sampleSize; i++)
    {
        std::cout << distArray[i] << std::endl;
    }

}




























