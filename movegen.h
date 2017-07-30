#ifndef MOVEGEN_H
#define MOVEGEN_H

typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL

#include <iostream>
#include <string>

#include "move.h"

class Pieces;
class BitBoards;
class ZobristH;


class MoveGen
{
public:
    MoveGen();

    //array of move objects by ply then number of moves
    Move moveAr[256];

    bool isWhite;
    int moveCount = 0;

    void generatePsMoves(bool capturesOnly, int ply);
    void clearMove(int ply, int numMoves);
    void constructBoards();
    void grab_boards(BitBoards *BBBoard, bool wOrB);

    bool isAttacked(U64 pieceLoc, bool wOrB);

    Move movegen_sort(int ply);

    void reorderMoves();

    //void unmakeMove(std::string moveKey, ZobristH *zobrist);
    //std::string makeMove(Move move, ZobristH *zobrist);

    //bitboards
        U64 FullTiles;
        U64 EmptyTiles;
        U64 BBWhitePieces;
        U64 BBWhitePawns;
        U64 BBWhiteRooks;
        U64 BBWhiteKnights;
        U64 BBWhiteBishops;
        U64 BBWhiteQueens;
        U64 BBWhiteKing;
        U64 BBBlackPieces;
        U64 BBBlackPawns;
        U64 BBBlackRooks;
        U64 BBBlackKnights;
        U64 BBBlackBishops;
        U64 BBBlackQueens;
        U64 BBBlackKing;

        const U64 FileABB = 0x0101010101010101ULL;
        const U64 FileBBB = FileABB << 1;
        const U64 FileCBB = FileABB << 2;
        const U64 FileDBB = FileABB << 3;
        const U64 FileEBB = FileABB << 4;
        const U64 FileFBB = FileABB << 5;
        const U64 FileGBB = FileABB << 6;
        const U64 FileHBB = FileABB << 7;
        //totally full bitboard
        const U64 full  = 0xffffffffffffffffULL;
        const U64 Totallyempty = 0LL;
        //files to keep pieces from moving left or right off board
        const U64 notAFile = 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080
        const U64 notHFile = 0xfefefefefefefefe; // ~0x0101010101010101
        const U64 rank4 = 1095216660480L;
        const U64 rank5=4278190080L;
        const U64 rank6 = rank5 >> 8;
        const U64 rank7 = rank6 >> 8;
        const U64 rank8 = rank7 >> 8;
        //ugh
        const U64 rank3 = rank4 << 8;
        const U64 rank2 = rank3 << 8;
        const U64 rank1 = rank2 << 8;
        //board for knight moves that can be shifted
        const U64 KNIGHT_SPAN=43234889994L;
        //files for keeping knight moves from wrapping
        const U64 FILE_AB=FileABB + FileBBB;
        const U64 FILE_GH=FileGBB + FileHBB;
        //Bitboard of all king movements that can then be shifted
        const U64 KING_SPAN=460039L;

        int trailingZeros(U64 i);
        U64 horizVert(int s);
        U64 DAndAntiDMoves(int s);
        U64 ReverseBits(U64 input);
        U64 noWeOne(U64 b);
        U64 soWeOne(U64 b);
        U64 westOne(U64 b);
        U64 soEaOne(U64 b);
        U64 noEaOne(U64 b);
        U64 eastOne(U64 b);
        U64 southOne(U64 b);
        U64 northOne(U64 b);


        void drawBBA();
        void drawBB(U64 board);
private:

        //assigns a score to moves and adds them to the move array
        void movegen_push(int x, int y, int x1, int y1, char piece, char captured, char flag, int ply);
        bool blind(Move move, int pieceVal, int captureVal);

        char whichPieceCaptured(U64 landing);


        //psuedo legal move gen

        void possibleWP(U64 wpawns, U64 blackking, bool capturesOnly, int ply);
        void possibleBP(U64 bpawns, U64 whiteking, bool capturesOnly, int ply);
        void possibleN(int location, U64 friends, U64 enemys, U64 oppositeking, U64 capturesOnly, int ply);
        void possibleB(int location, U64 friends, U64 enemys, U64 oppositeking, U64 capturesOnly, int ply);
        void possibleR(int location, U64 friends, U64 enemys, U64 oppositeking, U64 capturesOnly, int ply);
        void possibleQ(int location, U64 friends, U64 enemys, U64 oppositeking, U64 capturesOnly, int ply);
        void possibleK(int location, U64 friends, U64 enemys, U64 capturesOnly, int ply);


        //void undoCapture(U64 location, char piece, char whiteOrBlack);

};

#endif // MOVEGEN_H
