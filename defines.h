#ifndef DEFINES_H
#define DEFINES_H
//#pragma once;


//use later once bugs have been ironed out
enum esqare {
    A1=0 , B1, C1, D1, E1, F1, G1, H1,
    A2=8 , B2, C2, D2, E2, F2, G2, H2,
    A3=16, B3, C3, D3, E3, F3, G3, H3,
    A4=24, B4, C4, D4, E4, F4, G4, H4,
    A5=32, B5, C5, D5, E5, F5, G5, H5,
    A6=40, B6, C6, D6, E6, F6, G6, H6,
    A7=48, B7, C7, D7, E7, F7, G7, H7,
    A8=56, B8, C8, D8, E8, F8, G8, H8
};

enum epiece {
    PIECE_EMPTY,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum ecolor {
    WHITE,
    BLACK,
    COLOR_EMPTY
};

#ifdef __linux__
#define U32 unsigned __int32
#define U16 unsigned __int16
#define U8  unsigned __int8
#define S64 signed   __int64
#define S32 signed   __int32
#define S16 signed   __int16
#define S8  signed   __int8
#else
typedef unsigned char U8;
typedef char S8;
typedef unsigned short int U16;
typedef short int S16;
typedef unsigned int U32;
typedef int S32;
typedef unsigned long long U64;
typedef long long S64;
#endif

#define TT_ALPHA 1
#define TT_BETA 2
#define TT_EXACT 3

#define SORT_KING 400000000
#define SORT_HASH 200000000
#define SORT_CAPT 100000000
#define SORT_PROM  90000000
#define SORT_KILL  80000000

#define INF 10000
#define INVALID 32767

#endif // DEFINES_H
