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

//to store pieces while testing best moves
extern std::string storePiece, storePiece1, storePiece2, storePiece3;

//best move as ai determines
//extern std::string bestMove;

//store board state so it can be undone
extern std::string board1[8][8];

extern std::string board2[8][8];

extern int turnsTemp;

//bitboards
//board showing where all pieces are + aren't
extern U64 FullTiles;
extern U64 EmptyTiles;

extern U64 BBWhitePieces;

extern U64 BBWhitePawns;
extern U64 BBWhiteRooks;
extern U64 BBWhiteKnights;
extern U64 BBWhiteBishops;
extern U64 BBWhiteQueens;
extern U64 BBWhiteKing;


extern U64 BBBlackPieces;

extern U64 BBBlackPawns;
extern U64 BBBlackRooks;
extern U64 BBBlackKnights;
extern U64 BBBlackBishops;
extern U64 BBBlackQueens;
extern U64 BBBlackKing;

//external zorbist object
extern ZobristH *ZKey;
extern U64 zobKey;

//bitboards
extern BitBoards *newBBBoard;
//array for storing objects containing zorbist key for position as well as depth and eval
extern HashEntry transpositionT[15485867];

#endif // EXTERNS_H
