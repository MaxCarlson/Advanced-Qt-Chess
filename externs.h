#ifndef EXTERNS_H
#define EXTERNS_H

typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL

#include <vector>
#include <string>
#include <unordered_map>
#include "zobristh.h"
#include "bitboards.h"
#include "hashentry.h"


//chess board represented
extern std::string boardArr[8][8];

//string of pieces
extern std::string blackPieces[7];
extern std::string whitePieces[7];

//half turns
extern int turns;

//values for computing if a move is valid
extern int tempx, tempy, tempx2, tempy2;

//king safety arrays
extern bool whiteSafe[8][8];
extern bool blackSafe[8][8];

extern int count, numberOfPieceMoved;

// is ai on
extern int aiOn;

extern int turnsTemp;

//array holding U64 numbers for changing zobrist keys
extern U64 zArray[2][6][64];
//used to change color of move
extern U64 zBlackMove;
//test ~~ used to indicate a NULL move state
extern U64 zNullMove;

//array for storing objects containing zorbist key for position as well as depth and eval
extern HashEntry transpositionT[15485843];
extern HashEntry transpositionTQuiet[338207];
extern HashEntry transpositionEval[5021983];

#endif // EXTERNS_H
