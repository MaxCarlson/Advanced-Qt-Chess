#ifndef BITBOARDS_H
#define BITBOARDS_H


typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL

#include <string>
#include <cstdio>
#include <ctime>
#include <iostream>
#include "externs.h"
#include <cmath>
#include <random>
#include <iostream>
#include "zobristh.h"



class BitBoards
{

public:
    BitBoards();

    //builds boards through reading an array
    void constructBoards();

    //makes a move and returns a string that allows unmaking (both make and unmake change zobrist keys by move and color)
    std::string makeMove(std::string move, ZobristH *zobrist);

    //unmakes move
    void unmakeMove(std::string moveKey, ZobristH *zobrist);

    //start of move gen, used to point to correct color generation
    std::string genWhosMove(bool isWhite);

    //helper method for faster move gen -- finds number of trailing zeros -- Implement in all moves later
    int trailingZeros(U64 i);

    //helper funtction to draw out bitboards like chess boards
    void drawBB(U64 board);

    //draw out bitboards like a full chessboard array
    void drawBBA();
    //still needs lots of work, possible to generatae only captures more effeciantly than parsing already gened moves?
    std::string generateCaptures(bool isWhite, bool removeVarIfTryingToUseLater);

    //used in MVVLVA sorting !!! NEED to include pawn promotions
    std::string generateCaptures(bool isWhite);

    //used for validating player move
    bool isPlayerMoveValid(int x, int y, int x1, int y1, bool isWhite);

    bool isInCheck(bool isWhite);

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

    //return bitboards of movement in one direction
    U64 northOne(U64 b);
    U64 southOne(U64 b);
    U64 eastOne(U64 b);
    U64 noEaOne (U64 b);
    U64 soEaOne (U64 b);
    U64 westOne (U64 b);
    U64 soWeOne (U64 b);
    U64 noWeOne (U64 b);

    //slider move functions
    //up + down + left + right without friendly / enemy check
    U64 horizVert(int s);

    //both diagonals
    U64 DAndAntiDMoves(int s);

private:

    //unsafe area checking for b/w
        U64 unsafeForWhite(U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishops, U64 bqueens, U64 bking);
        U64 unsafeForBlack(U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishop, U64 bqueens, U64 bking);
        //only unsafe for king -- sliders travel "through" king so other side is unsafe
        U64 kingDangerSquares(U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 bpawns, U64 brooks, U64 bknights, U64 bbishop, U64 bqueens, U64 BOrWKing);
        //pinned piece check at move gen start to pass to pieces so as not to gen invalid moves
        U64 pinnedBB(U64 rooks, U64 bishops, U64 queens, U64 king);
            //helper functions for isolated ray calcs
            U64 up(U64 piece);
            U64 down(U64 piece);
            U64 right(U64 piece);
            U64 left(U64 piece);
            U64 upright(U64 piece);
            U64 downright(U64 piece);
            U64 downleft(U64 piece);
            U64 upleft(U64 piece);

    //our pieces and enemy pieces -- for finding moves for pinned pieces along pin ray
    std::string pinnedMoves(U64 pinned, U64 opawns, U64 orooks, U64 obishops, U64 oqueens, U64 oking, U64 erooks, U64 ebishops, U64 equeens, U64 ourPieces, bool isWhite);
    std::string pinnedCaptures(U64 pinned, U64 opawns, U64 orooks, U64 obishops, U64 oqueens, U64 oking, U64 erooks, U64 ebishops, U64 equeens, U64 ourPieces, U64 enemyPieces, bool isWhite);
        //pinned pieces movements
        std::string pinnedPawnCaptures(U64 opawns, U64 enemyPieces, U64 mrays, bool isWhite);
        std::string pinnedPawnPushes(U64 opawns, U64 EmptyTiles, U64 mrays, bool isWhite);
        std::string pinnedBishopMoves(U64 obishops, U64 ourPieces, U64 mrays);
        std::string pinnedRookMoves(U64 orooks, U64 ourPieces, U64 mrays);
        std::string pinnedQueenMoves(U64 oqueens, U64 ourPieces, U64 mrays);
        //special make move to determine if the pinned moves are legal or not
        std::string makePinnedMovesLegal(bool isWhite, std::string moves, U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishops, U64 bqueens, U64 bking);
    //remove pinned pieces from normal piece generation but not from white pieces or all piece baords. reinstate after all moves genned
    std::string removePinnedPieces(U64 pinnedBB, bool whiteOrBlack);
    void restorePinnedPieces(std::string pieces, bool wOrB);

    //moves
    //process moves for each color into a string of moves
        std::string possibleMovesW(U64 whitepieces, U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishops, U64 bqueens, U64 bking);
        std::string possibleMovesB(U64 blackpieces, U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishops, U64 bqueens, U64 bking);

            //find out if piece is white or black
            bool isWhite(U64 pieceMoving);
            //is this a capture move if so, which piece is captured
            char isCapture(U64 landing, bool isWhite);
            //if move is capture update appropiate board and store capture coordinates
            std::string updateCapture(U64 landing);
        //takes a string input of move that contains piece data, coordinate data, and capture data

            //undos capture if needed           //LOOKS LIKE THERE MIGHT BE AN ERROR IN SWITCH STATMENTS TOO MANY CASES TRIED, GO FIX LATER
            void undoCapture(U64 location, char piece, char whiteOrBlack);

        //pawn moves
        std::string possibleWP(U64 wpawns, U64 EmptyTiles, U64 blackking);
        std::string possibleBP(U64 bpawns, U64 EmptyTiles, U64 whiteking);
        //other piece moves

        std::string possibleR(U64 wOrBrooks, U64 wOrBpieces, U64 oppositeking);
        std::string possibleB(U64 wOrBbishops, U64 wOrBpieces, U64 oppositeking);
        std::string possibleN(U64 wOrBknights, U64 wOrBpieces, U64 oppositeking);
        std::string possibleQ(U64 wOrBqueens, U64 wOrBpieces, U64 oppositeking);
        std::string possibleK(U64 wOrBking, U64 wOrBpieces, U64 kingSafeLessKing);

        //helper function for calculating sliders
        U64 ReverseBits(U64 input);

    //king and check
    //create a BB of pieces putting king in check
    U64 checkersBB(U64 ourKing, bool isWhite);
    //are we in double check
    bool isDoubleCheck(U64 attackers);
    //generate moves if we're in check
    std::string genInCheckMoves(U64 attacker, U64 ourKing, bool isWhite);
       //helper for piece move gen
       std::string genTakeOnlys(U64 attacker, U64 ourKing, bool isWhite);
       std::string genBlockOrTakes(U64 attacker, U64 ourKing, bool isWhite, char piece); ///NOTE MAKE CODE MORE VERSATILE BY MAKING St MOVE GEN FUNCTIONS TAKE A CAPTURE OR BLOCK MASK


};

#endif // BITBOARDS_H
