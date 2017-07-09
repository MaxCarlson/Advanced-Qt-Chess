#include "zobristh.h"
#include "externs.h"

#include <cmath>
#include <random>
#include <iostream>


std::random_device rd;

std::mt19937_64 mt(rd());

std::uniform_int_distribution<U64> dist(std::llround(std::pow(2,61)), std::llround(std::pow(2,62)));

U64 zArray[2][6][64];
//U64 zEnPassasnt[8]; ~~restore once implemented
//U64 zCastle[4];
U64 zBlackMove;
//actual key reprenting game state
U64 zobKey;

ZobristH::ZobristH()
{

}

U64 ZobristH::random64()
{
    //get random 64 bit integer to seed zorbist arrays
    U64 ranUI = dist(mt);

    //std::cout << ranUI << ",   ";
    //U64 ranUI = rand() * 924032403297933277;
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
    /*
    //enpassant and castle filling below

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

void ZobristH::UpdateKey(int start, int end, std::string moveKey)
{
    //gather piece, capture, and w or b info from movekey
    char piece = moveKey[4];
    char captured = moveKey[5];
    char wB = moveKey[6];

    //if a piece was captured XOR that location with randomkey at array location end
    if(captured == 'P' || captured == 'R' || captured == 'N' || captured == 'B' || captured == 'Q'){
        if(captured == 'P'){
            zobKey ^= zArray[0][0][end];
        } else if(captured == 'R'){
            zobKey ^= zArray[0][1][end];
        } else if(captured == 'N'){
            zobKey ^= zArray[0][2][end];
        } else if(captured == 'B'){
            zobKey ^= zArray[0][3][end];
        } else if(captured == 'Q'){
            zobKey ^= zArray[0][4][end];
        }
    } else if (captured == 'p' || captured == 'r' || captured == 'n' || captured == 'b' || captured == 'q'){
        if(captured == 'p'){
            zobKey ^= zArray[1][0][end];
        } else if(captured == 'r'){
            zobKey ^= zArray[1][1][end];
        } else if(captured == 'n'){
            zobKey ^= zArray[1][2][end];
        } else if(captured == 'b'){
            zobKey ^= zArray[1][3][end];
        } else if(captured == 'q'){
            zobKey ^= zArray[1][4][end];
        }

    }

    //XOR zobKey with zArray number at piece start end then end location
    //if piece is white..
    if(wB == 'w') {
        if(piece == 'P'){
            zobKey ^= zArray[0][0][start];
            zobKey ^= zArray[0][0][end];
        } else if(piece == 'R'){
            zobKey ^= zArray[0][1][start];
            zobKey ^= zArray[0][1][end];
        } else if(piece == 'N'){
            zobKey ^= zArray[0][2][start];
            zobKey ^= zArray[0][2][end];
        } else if(piece == 'B'){
            zobKey ^= zArray[0][3][start];
            zobKey ^= zArray[0][3][end];
        } else if(piece == 'Q'){
            zobKey ^= zArray[0][4][start];
            zobKey ^= zArray[0][4][end];
        } else if(piece == 'K'){
            zobKey ^= zArray[0][5][start];
            zobKey ^= zArray[0][5][end];
        }
    //black
    } else if (wB == 'b'){

        if(piece == 'p'){
            zobKey ^= zArray[1][0][start];
            zobKey ^= zArray[1][0][end];
        } else if(piece == 'r'){
            zobKey ^= zArray[1][1][start];
            zobKey ^= zArray[1][1][end];
        } else if(piece == 'n'){
            zobKey ^= zArray[1][2][start];
            zobKey ^= zArray[1][2][end];
        } else if(piece == 'b'){
            zobKey ^= zArray[1][3][start];
            zobKey ^= zArray[1][3][end];
        } else if(piece == 'q'){
            zobKey ^= zArray[1][4][start];
            zobKey ^= zArray[1][4][end];
        } else if(piece == 'k'){
            zobKey ^= zArray[1][5][start];
            zobKey ^= zArray[1][5][end];
        }

    }
}

void ZobristH::UpdateColor()
{
    zobKey ^= zBlackMove;
}

U64 ZobristH::getZobristHash(bool isWhiteTurn)
{
    U64 returnZKey = 0LL;
    for (int square = 0; square < 64; square++){
        //if tile is empty skip to next i
        /*
        if(((EmptyTiles >> square) & 1) == 1){
            continue;
        }
        */
        //white and black pawns
        //if there is a white pawn on i square
        if(((BBWhitePawns >> square) & 1) == 1)
        {
            //XOR the zkey with the U64 in the white pawns square
            //that was generated from rand64
            returnZKey ^= zArray[0][0][square];
        }
        else if(((BBBlackPawns >> square) & 1) == 1)
        {
            returnZKey ^= zArray[1][0][square];
        }
        //white pieces
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
    //EnPassant and castling stuff add later

    /*
    //if it isn't whites turn, XOR zobrist key with black move U64
    if(!isWhiteTurn){
        returnZKey ^= zBlackMove;
    }
    */

    zobKey = returnZKey;

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

U64 ZobristH::debugKey(bool isWhite)
{
    U64 returnZKey = 0LL;
    for (int square = 0; square < 64; square++){
        //if tile is empty skip to next i
        /*
        if(((EmptyTiles >> square) & 1) == 1){
            continue;
        }
        */
        //white and black pawns
        //if there is a white pawn on i square
        if(((BBWhitePawns >> square) & 1) == 1)
        {
            //XOR the zkey with the U64 in the white pawns square
            //that was generated from rand64
            returnZKey ^= zArray[0][0][square];
        }
        else if(((BBBlackPawns >> square) & 1) == 1)
        {
            returnZKey ^= zArray[1][0][square];
        }
        //white pieces
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
    //EnPassant and castling stuff add later


    //if it isn't whites turn, XOR zobrist key with black move U64
    if(isWhite == false){
        returnZKey ^= zBlackMove;
    }

    return returnZKey;
}




























