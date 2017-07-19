#include "bitboards.h"

#include <iostream>
#include "externs.h"

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
extern const U64 notAFile = 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080
extern const U64 notHFile = 0xfefefefefefefefe; // ~0x0101010101010101


extern const U64 rank4 = 1095216660480L;
extern const U64 rank5=4278190080L;
extern const U64 rank6 = rank5 >> 8;
extern const U64 rank7 = rank6 >> 8;
extern const U64 rank8 = rank7 >> 8;
//ugh
extern const U64 rank3 = rank4 << 8;
extern const U64 rank2 = rank3 << 8;
extern const U64 rank1 = rank2 << 8;


//board for knight moves that can be shifted
extern const U64 KNIGHT_SPAN=43234889994L;

    //files for keeping knight moves from wrapping
extern const U64 FILE_AB=FileABB + FileBBB;
extern const U64 FILE_GH=FileGBB + FileHBB;

//Bitboard of all king movements that can then be shifted
extern const U64 KING_SPAN=460039L;

extern const U64 RankMasks8[8] =/*from rank1 to rank8*/
    {
        0xFFL, 0xFF00L, 0xFF0000L, 0xFF000000L, 0xFF00000000L, 0xFF0000000000L, 0xFF000000000000L, 0xFF00000000000000L
    };
extern const U64 FileMasks8[8] =/*from fileA to FileH*/
{
    0x101010101010101L, 0x202020202020202L, 0x404040404040404L, 0x808080808080808L,
    0x1010101010101010L, 0x2020202020202020L, 0x4040404040404040L, 0x8080808080808080L
};

extern const U64 DiagonalMasks8[15] =/*from top left to bottom right*/
{
0x1L, 0x102L, 0x10204L, 0x1020408L, 0x102040810L, 0x10204081020L, 0x1020408102040L,
0x102040810204080L, 0x204081020408000L, 0x408102040800000L, 0x810204080000000L,
0x1020408000000000L, 0x2040800000000000L, 0x4080000000000000L, 0x8000000000000000L
};

extern const U64 AntiDiagonalMasks8[15] =/*from top right to bottom left*/
{
0x80L, 0x8040L, 0x804020L, 0x80402010L, 0x8040201008L, 0x804020100804L, 0x80402010080402L,
0x8040201008040201L, 0x4020100804020100L, 0x2010080402010000L, 0x1008040201000000L,
0x804020100000000L, 0x402010000000000L, 0x201000000000000L, 0x100000000000000L
};

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


std::string chessBoard [8][8]= {
    {"r", "n", "b", "q", "k", "b", "n", "r"},
    {"p", "p", "p", "p", "p", "p", "p", "p"},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {"P", "P", "P", "P", "P", "P", "P", "P"},
    {"R", "N", "B", "Q", "K", "B", "N", "R"},
              };


BitBoards::BitBoards()
{

}
/*
std::string BitBoards::generatePsMoves()
{
    std::string moves;

    //moves += possiblePW(BBWhitePawns, EmptyTiles);
    //moves += possiblePB(BBBlackPawns, EmptyTiles);
    //U64 a = horizVert(20);
    //U64 a = DAndAntiDMoves(20);
   // std::string a = possibleWN(BBWhiteKnights, BBWhitePieces, BBBlackKing);
    //std::string b = possibleBN(BBBlackKnights, BBBlackPieces, BBWhiteKing);
    //std::string d = possibleK(BBWhiteKing, BBWhitePieces);
    //U64 a = unsafeForBlack(BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    //U64 b = unsafeForWhite(BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    //std::string f = possibleMovesW(BBWhitePieces, BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    //std::string g = possibleMovesB(BBBlackPieces, BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    //U64 a = pinnedBB(BBBlackRooks, BBBlackBishops, BBBlackQueens, BBWhiteKing);
    std::string f = possibleMovesW(BBWhitePieces, BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    int c = 5;


    for(int i = 0; i < 64; i++){
        if(a & (1ULL << i)){
            std::cout<< 1 <<", ";
        } else {
            std::cout << 0 << ", ";
        }
        if((i+1)%8 == 0){
            std::cout<< std::endl;
        }
    }


}
*/
std::string BitBoards::genWhosMove(bool isWhite)
{
    std::string moves;
    if(isWhite == true){

        moves = possibleMovesW(BBWhitePieces, BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    } else {

        moves = possibleMovesB(BBBlackPieces, BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    }

    return moves;
}

//move generation functions for white and black
std::string BitBoards::possibleMovesW(U64 whitepieces, U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishops, U64 bqueens, U64 bking)
{
    U64 empty = ~FullTiles;
    U64 pinned, kingSafeLessKing, unsafeTiles, checkers;

    std::string moveList, removedPinned;

    //generate unsafe tiles for in check checking
    unsafeTiles = unsafeForWhite(wpawns, wrooks, wknights, wbishops, wqueens, wking, bpawns, brooks, bknights, bbishops, bqueens, bking);

    //if king is in check
    if(wking & unsafeTiles){
        //find out if it's double check
        checkers = checkersBB(wking, true);

        //if we're in double check only generate king moves
        if(isDoubleCheck(checkers)){
            //generate king safety array without king in it, pass to king move gen (blank board in place of our king)
            kingSafeLessKing = unsafeForWhite(wpawns, wrooks, wknights, wbishops, wqueens, 0LL, bpawns, brooks, bknights, bbishops, bqueens, bking);

            //generates legal king moves
            moveList += possibleK(wking, whitepieces, kingSafeLessKing);

            return moveList;
        }
        //if we're only in single check, generate moves that either,..
        //take out checking piece or block it's ray if it's a ray piece
        moveList += genInCheckMoves(checkers, wking, true);
        //generate king safety array without king in it, pass to king move gen (blank board in place of our king)
        kingSafeLessKing = unsafeForWhite(wpawns, wrooks, wknights, wbishops, wqueens, 0LL, bpawns, brooks, bknights, bbishops, bqueens, bking);
        //generates legal king moves
        moveList += possibleK(wking, whitepieces, kingSafeLessKing);

        return moveList;
    }

    //generate pinned BB and remove pieces from it for sepperate move gen ~~ opposite piece color aside from king
    pinned = pinnedBB(brooks, bbishops, bqueens, wking);
    moveList += pinnedMoves(pinned, wpawns, wrooks, wbishops, wqueens, wking, brooks, bbishops, bqueens, whitepieces, true);
    //test all pinned moves against king safety to be sure they're legal
    moveList = makePinnedMovesLegal(true, moveList, wpawns, wrooks, wknights, wbishops, wqueens, wking, bpawns, brooks, bknights, bbishops, bqueens, bking);
    //remove pinned pieces from normal piece generation and store into string so can be restored
    removedPinned = removePinnedPieces(pinned, true);

    //standard move gen without pinned pieces
    moveList += possibleWP(wpawns, empty, bking);
    moveList += possibleR(wrooks, whitepieces, bking);
    moveList += possibleN(wknights, whitepieces, bking);
    moveList += possibleB(wbishops, whitepieces, bking);
    moveList += possibleQ(wqueens, whitepieces, bking);

    //generate king safety array without king in it, pass to king move gen (blank board in place of our king)
    kingSafeLessKing = unsafeForWhite(wpawns, wrooks, wknights, wbishops, wqueens, 0LL, bpawns, brooks, bknights, bbishops, bqueens, bking);

    //generates legal king moves
    moveList += possibleK(wking, whitepieces, kingSafeLessKing);

    //restore pinned pieces to master BB's
    restorePinnedPieces(removedPinned, true);

    return moveList;
}

std::string BitBoards::possibleMovesB(U64 blackpieces, U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishops, U64 bqueens, U64 bking)
{

    //FullTiles = wpawns | wrooks | wknights | wbishops | wqueens | wking | bpawns | brooks | bknights | bbishops | bqueens | bking;
    U64 empty =~ FullTiles;
    //U64 EmptyTiles = ~FullTiles;
    U64 pinned, kingSafeLessKing, unsafeTiles, checkers;

    std::string moveList, removedPinned;

    //generate unsafe tiles for in check checking
    unsafeTiles = unsafeForBlack(wpawns, wrooks, wknights, wbishops, wqueens, wking, bpawns, brooks, bknights, bbishops, bqueens, bking);

    //if king is in check
    if(bking & unsafeTiles){
        //find out if it's double check
        checkers = checkersBB(bking, false);
        //if we're in double check only generate king moves
        if(isDoubleCheck(checkers)){
            //generate king safety array without king in it, pass to king move gen (blank board in place of our king)
            kingSafeLessKing = unsafeForBlack(wpawns, wrooks, wknights, wbishops, wqueens, wking, bpawns, brooks, bknights, bbishops, bqueens, 0LL);
            //generates legal king moves
            moveList += possibleK(bking, blackpieces, kingSafeLessKing);

            return moveList;
        }
        //if we're only in single check, generate moves that either,..
        //take out checking piece or block it's ray if it's a ray piece
        moveList += genInCheckMoves(checkers, bking, false);
        //generate king safety array without king in it, pass to king move gen (blank board in place of our king)

        kingSafeLessKing = unsafeForBlack(wpawns, wrooks, wknights, wbishops, wqueens, wking, bpawns, brooks, bknights, bbishops, bqueens, 0LL);
        //generates legal king moves
        moveList += possibleK(bking, blackpieces, kingSafeLessKing);   

        return moveList;
    }

    //generate pinned BB and remove pieces from it for sepperate move gen ~~ opposite piece color aside from king
    pinned = pinnedBB(wrooks, wbishops, wqueens, bking);
    moveList += pinnedMoves(pinned, bpawns, brooks, bbishops, bqueens, bking, wrooks, wbishops, wqueens, blackpieces, false);

    //test all pinned moves against king safety to be sure they're legal
    moveList = makePinnedMovesLegal(false, moveList, wpawns, wrooks, wknights, wbishops, wqueens, wking, bpawns, brooks, bknights, bbishops, bqueens, bking);

    //remove pinned pieces from normal piece generation and store into string so can be restored
    removedPinned = removePinnedPieces(pinned, false);

    //standard move gen without pinned pieces
    moveList += possibleBP(bpawns, empty, wking);
    moveList += possibleR(brooks, blackpieces, wking);
    moveList += possibleN(bknights, blackpieces, wking);
    moveList += possibleB(bbishops, blackpieces, wking);
    moveList += possibleQ(bqueens, blackpieces, wking);

    //generate king safety array without king in it, pass to king move gen (blank board in place of our king)
    kingSafeLessKing = unsafeForBlack(wpawns, wrooks, wknights, wbishops, wqueens, wking, bpawns, brooks, bknights, bbishops, bqueens, 0LL);

    //generates legal king moves
    moveList += possibleK(bking, blackpieces, kingSafeLessKing);

    //restore pinned pieces to master BB's
    restorePinnedPieces(removedPinned, false);
    //duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC; //for testing
    //std::cout<<"printf: "<< duration <<'\n';

    return moveList;
}

//king safety stuff
U64 BitBoards::unsafeForWhite(U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishops, U64 bqueens, U64 bking)
{
    U64 unsafe, temp, temp1;

    temp = FullTiles;
    temp1 = EmptyTiles;
    EmptyTiles = ~FullTiles;

    FullTiles = wpawns | wrooks | wknights | wbishops | wqueens | wking | bpawns | brooks | bknights | bbishops | bqueens | bking;

    //pawn
    unsafe = ((bpawns << 7) &~ FileHBB); //pawn capture right
    unsafe |= ((bpawns << 9) &~ FileABB); // left
    //drawBB(unsafe);

    U64 possibles;
    //knights
    U64 i = bknights &~(bknights - 1);

    while (i != 0){
        int iLocation = trailingZeros(i);
        if(iLocation > 18){
            possibles = KNIGHT_SPAN << (iLocation - 18);
        } else {
            possibles = KNIGHT_SPAN >> (18 - iLocation);
        }

        if(iLocation % 8 < 4){
            possibles &= ~ FILE_GH;
        } else {
            possibles &= ~ FILE_AB;
        }
        unsafe |= possibles;
        bknights &= ~i;
        i = bknights &~ (bknights - 1);
    }
    //bishops / queens
    U64 QB = bqueens | bbishops;
    i = QB &~ (QB-1);
    while(i != 0){
        int iLocation = trailingZeros(i);
        possibles = DAndAntiDMoves(iLocation);
        unsafe |= possibles;
        QB &= ~i;
        i = QB &~ (QB-1);
    }

    //rooks / queens
    U64 QR = bqueens | brooks;
    i = QR &~ (QR-1);
    while(i != 0){
        int iLocation = trailingZeros(i);
        possibles = horizVert(iLocation);
        unsafe |= possibles;
        QR &= ~i;
        i = QR &~ (QR-1);
    }

    //king
    int iLocation = trailingZeros(bking);
    if(iLocation > 9){
        possibles = KING_SPAN << (iLocation-9);
    } else {
        possibles = KING_SPAN >> (9 - iLocation);
    }

    if(iLocation % 8 < 4){
        possibles &= ~ FILE_GH;
    } else {
        possibles &= ~ FILE_AB;
    }
    unsafe |= possibles;
    //drawBB(unsafe);

    //resest full tiles so when passed an empty king king isn't removed from full BB
    FullTiles = temp;
    EmptyTiles = temp1;

    return unsafe;
}

U64 BitBoards::unsafeForBlack(U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishop, U64 bqueens, U64 bking)
{

    U64 unsafe, temp, temp1;
    temp = FullTiles;
    temp1 = EmptyTiles;
    EmptyTiles = ~FullTiles;

    FullTiles = wpawns | wrooks | wknights | wbishops | wqueens | wking | bpawns | brooks | bknights | bbishop | bqueens | bking;

    //pawn
    unsafe = ((wpawns >> 7) &~ FileABB); //pawn capture right
    unsafe |= ((wpawns >> 9) &~ FileHBB); // left
    //drawBB(unsafe);


    U64 possibles;
    //knights
    U64 i = wknights &~(wknights - 1);

    while (i != 0){
        int iLocation = trailingZeros(i);
        if(iLocation > 18){
            possibles = KNIGHT_SPAN << (iLocation - 18);
        } else {
            possibles = KNIGHT_SPAN >> (18 - iLocation);
        }

        if(iLocation % 8 < 4){
            possibles &= ~ FILE_GH;
        } else {
            possibles &= ~ FILE_AB;
        }
        unsafe |= possibles;
        wknights &= ~i;
        i = wknights &~ (wknights - 1);
    }
    //bishops / queens
    U64 QB = wqueens | wbishops;
    i = QB &~ (QB-1);
    while(i != 0){
        int iLocation = trailingZeros(i);
        possibles = DAndAntiDMoves(iLocation);
        unsafe |= possibles;
        QB &= ~i;
        i = QB &~ (QB-1);
    }

    //rooks / queens
    U64 QR = wqueens | wrooks;
    i = QR &~ (QR-1);
    while(i != 0){
        int iLocation = trailingZeros(i);
        possibles = horizVert(iLocation);
        unsafe |= possibles;
        QR &= ~i;
        i = QR &~ (QR-1);
    }

    //king
    int iLocation = trailingZeros(wking);
    if(iLocation > 9){
        possibles = KING_SPAN << (iLocation-9);
    } else {
        possibles = KING_SPAN >> (9 - iLocation);
    }

    if(iLocation % 8 < 4){
        possibles &= ~ FILE_GH;
    } else {
        possibles &= ~ FILE_AB;
    }
    unsafe |= possibles;
    //resest full tiles so when passed an empty king king isn't removed from full BB permanently
    FullTiles = temp;
    EmptyTiles = temp1;

    return unsafe;
}

U64 BitBoards::checkersBB(U64 ourKing, bool isWhite)
{


    U64 attackers;
    int ourKingLocation = trailingZeros(ourKing);
    //use super piece at king position to find number of checkers
    if(isWhite == true){
        //knights done first due to qwirk
        if(ourKingLocation > 18){
            attackers = KNIGHT_SPAN<<(ourKingLocation-18);
        } else {
            attackers = KNIGHT_SPAN>>(18-ourKingLocation);
        }
        if(ourKingLocation % 8 < 4){
            attackers &= ~FILE_GH & BBBlackKnights;
        } else {
            attackers &= ~FILE_AB & BBBlackKnights;
        }
        //black pawn captures from king position -- using opposite pawn directions
        //capture right
        attackers |= noEaOne(ourKing) & BBBlackPawns;
        //capture left
        attackers |= noWeOne(ourKing) & BBBlackPawns;
        //rook from super piece
        attackers |= horizVert(ourKingLocation) & BBBlackRooks;
        //bishops
        attackers |= DAndAntiDMoves(ourKingLocation) & BBBlackBishops;
        //queens
        attackers |= DAndAntiDMoves(ourKingLocation) + horizVert(ourKingLocation) & BBBlackQueens;


    } else {
        //knights done first due to qwirk
        if(ourKingLocation > 18){
            attackers = KNIGHT_SPAN<<(ourKingLocation-18);
        } else {
            attackers = KNIGHT_SPAN>>(18-ourKingLocation);
        }
        if(ourKingLocation % 8 < 4){
            attackers &= ~FILE_GH & BBWhiteKnights;
        } else {
            attackers &= ~FILE_AB & BBWhiteKnights;
        }
        //capture right
        attackers = soEaOne(ourKing) & BBWhitePawns;
        //capture left
        attackers |= soWeOne(ourKing) & BBWhitePawns;
        //rook from super piece
        attackers |= horizVert(ourKingLocation) & BBWhiteRooks;
        //bishops
        attackers |= DAndAntiDMoves(ourKingLocation) & BBWhiteBishops;
        //queens
        attackers |= DAndAntiDMoves(ourKingLocation) + horizVert(ourKingLocation) & BBWhiteQueens;
    }

    //return BB containing locations of attackers
    return attackers;
}

bool BitBoards::isDoubleCheck(U64 attackers)
{
    U64 j = attackers &~(attackers-1);
    int counter = 0;
    while(j != 0){
        attackers &= ~j;
        j = attackers &~ (attackers-1);
        counter ++;
    }
    if(counter > 1){
        return true;
    }
    return false;
}

std::string BitBoards::genInCheckMoves(U64 attacker, U64 ourKing, bool isWhite)
{


    char enemy;
    std::string moves;
    //figure out which king of piece is putting us in check
    if(isWhite == true){
        if(attacker & BBBlackPawns) {
            enemy = 'p';
        } else if (attacker & BBBlackRooks ){
            enemy = 'r';
        } else if (attacker & BBBlackKnights){
            enemy = 'n';
        }else if (attacker & BBBlackBishops){
            enemy = 'b';
        }else if (attacker & BBBlackQueens){
            enemy = 'q';
        }
        switch(enemy){
            case 'p':
                moves = genTakeOnlys(attacker, ourKing, isWhite);
            case 'r':
                moves = genBlockOrTakes(attacker, ourKing, isWhite, 'r');
            case 'n':
                moves = genTakeOnlys(attacker, ourKing, isWhite);
            case 'b':
                moves = genBlockOrTakes(attacker, ourKing, isWhite, 'b');
            case 'q':
                moves = genBlockOrTakes(attacker, ourKing, isWhite, 'q');

        }
    } else {
        if(attacker & BBWhitePawns) {
            enemy = 'P';
        } else if (attacker & BBWhiteRooks ){
            enemy = 'R';
        } else if (attacker & BBWhiteKnights){
            enemy = 'N';
        }else if (attacker & BBWhiteBishops){
            enemy = 'B';
        }else if (attacker & BBWhiteQueens){
            enemy = 'Q';
        }
        switch(enemy){
            case 'P':
                moves = genTakeOnlys(attacker, ourKing, isWhite);
            case 'R':;
                moves = genBlockOrTakes(attacker, ourKing, isWhite, 'R');
            case 'N':
                moves = genTakeOnlys(attacker, ourKing, isWhite);
            case 'B':
                moves = genBlockOrTakes(attacker, ourKing, isWhite, 'B');
            case 'Q':
                moves = genBlockOrTakes(attacker, ourKing, isWhite, 'Q');

        }

    }


    return moves;
    //if ray piece store ray between king and checker if ray piece

    //generate moves for pieces and see if ray can be interrupted or piece captured
}

std::string BitBoards::genTakeOnlys(U64 attacker, U64 ourKing, bool isWhite)
{

    std::string list;

    U64 friendlys, moves, pawns, rooks, knights, bishops, queens;

    //pawn captures for white
    if(isWhite == true){
        //get correct boards to use in friendly fire cals and move gen
        friendlys = BBWhitePieces;
        pawns = BBWhitePawns; rooks = BBWhiteRooks;
        knights = BBWhiteKnights; bishops = BBWhiteBishops;
        queens = BBWhiteQueens;
        //capture right
        U64 PAWN_MOVES = noEaOne(pawns) & BBBlackPieces & attacker;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                list+=i%8-1;
                list+=i/8+1;
                list+=i%8;
                list+=i/8;

            }
        }

        //capture left
        PAWN_MOVES = noWeOne(pawns) & BBBlackPieces & attacker;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                list+=i%8+1;
                list+=i/8+1;
                list+=i%8;
                list+=i/8;

            }
        }

    } else {
        friendlys = BBBlackPieces;
        pawns = BBBlackPawns; rooks = BBBlackRooks;
        knights = BBBlackKnights; bishops = BBBlackBishops;
        queens = BBBlackQueens;

        //pawns for black
        //capture right
        U64 PAWN_MOVES = soEaOne(pawns) & BBWhitePieces & attacker;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                list+=i%8-1;
                list+=i/8-1;
                list+=i%8;
                list+=i/8;

            }
        }

        //capture left
        PAWN_MOVES = soWeOne(pawns) & BBWhitePieces & attacker;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                list+=i%8+1;
                list+=i/8-1;
                list+=i%8;
                list+=i/8;

            }
        }

    }
    //rook captures
    for(int i = 0; i < 64; i++){
        if(((rooks>>i) &1) == 1){
            //map moves that don't collide with friendlys and take piece putting us in check
            moves = horizVert(i) & ~friendlys & attacker;

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    //knight captures
    //loop through and find knights
    for(int i = 0; i < 64; i++){
        if(((knights>>i) &1) == 1){
            //use the knight span board which holds possible knight moves
            //and apply a shift to the knights current pos
            if(i > 18){
                moves = KNIGHT_SPAN<<(i-18);
            } else {
                moves = KNIGHT_SPAN>>(18-i);
            }

            //making sure the moves don't warp around to other side once shifter
            //as well as friendly and illegal king capture check
            if(i % 8 < 4){
                moves &= ~FILE_GH & ~friendlys & attacker;
            } else {
                moves &= ~FILE_AB & ~friendlys & attacker;
            }

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    //bishops
    //loop through and find bishops
    for(int i = 0; i < 64; i++){
        if(((bishops>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = DAndAntiDMoves(i) & ~friendlys & attacker;
            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    //queens
    //loop through and find queens
    for(int i = 0; i < 64; i++){
        if(((queens>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = DAndAntiDMoves(i) + horizVert(i) & ~friendlys & attacker;

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    return list;
}

std::string BitBoards::genBlockOrTakes(U64 attacker, U64 ourKing, bool isWhite, char piece)
{
    //king rays and enemy rays
    U64 uB, dB, rB, lB, nEB, nWB, sWB, sEB, tempRay, oneP = 0LL, mrays = 0LL, pawns, rooks, knights, bishops, queens, friendlys, moves;
    std::string list;

    //calculate move king rays
    //king up ray
    uB = up(ourKing);
    //king down ray
    dB = down(ourKing);
    //king right ray
    rB = right(ourKing);
    //king ray left
    lB = left(ourKing);
    //king NE ray
    nEB = upright(ourKing);
    //king SE ray
    sEB = downright(ourKing);
    //king SW ray
    sWB = downleft(ourKing);
    //king NW ray
    nWB = upleft(ourKing);

    //find location of attacker
    int eLocation = trailingZeros(attacker);
    oneP += (1ULL << eLocation);

    //generate a ray between king and checker
    if(piece == 'r' || piece == 'q' || piece == 'R' || piece == 'Q'){
        tempRay = up(oneP); // up qb ray
        mrays |= dB & tempRay; // merge with down k
        tempRay = down(oneP); //down qb
        mrays |= uB & tempRay; // merge with up k
        tempRay = right(oneP); //right qb
        mrays |= lB & tempRay; // left k
        tempRay = left(oneP); // left qb
        mrays |= rB & tempRay; // right k
    }
    if (piece == 'b' || piece == 'q' || piece == 'B' || piece == 'Q'){
        tempRay = upright(oneP); //up right qb
        mrays |= sWB & tempRay; //down left k
        tempRay = downright(oneP);
        mrays |= nWB & tempRay; // up left
        tempRay = downleft(oneP);
        mrays |= nEB & tempRay;
        tempRay = upleft(oneP);
        mrays |= sEB & tempRay;
    }



    if(isWhite == true){
        //get correct boards to use in friendly fire cals and move gen
        friendlys = BBWhitePieces;
        pawns = BBWhitePawns; rooks = BBWhiteRooks;
        knights = BBWhiteKnights; bishops = BBWhiteBishops;
        queens = BBWhiteQueens;
        //capture right
        U64 PAWN_MOVES = noEaOne(pawns) & BBBlackPieces & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                list+=i%8-1;
                list+=i/8+1;
                list+=i%8;
                list+=i/8;

            }
        }

        //capture left
        PAWN_MOVES = noWeOne(pawns) & BBBlackPieces & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                list+=i%8+1;
                list+=i/8+1;
                list+=i%8;
                list+=i/8;

            }
        }

    } else {
        friendlys = BBBlackPieces;
        pawns = BBBlackPawns; rooks = BBBlackRooks;
        knights = BBBlackKnights; bishops = BBBlackBishops;
        queens = BBBlackQueens;
        //capture right
        U64 PAWN_MOVES = soEaOne(pawns) & BBWhitePieces & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                list+=i%8-1;
                list+=i/8-1;
                list+=i%8;
                list+=i/8;

            }
        }

        //capture left
        PAWN_MOVES = soWeOne(pawns) & BBWhitePieces & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                list+=i%8+1;
                list+=i/8-1;
                list+=i%8;
                list+=i/8;

            }
        }
    }


    //rook blocks or captures
    for(int i = 0; i < 64; i++){
        if(((rooks>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = horizVert(i) & ~friendlys & mrays;

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    //knight blocks or captures
    //loop through and find knights
    for(int i = 0; i < 64; i++){
        if(((knights>>i) &1) == 1){
            //use the knight span board which holds possible knight moves
            //and apply a shift to the knights current pos
            if(i > 18){
                moves = KNIGHT_SPAN<<(i-18);
            } else {
                moves = KNIGHT_SPAN>>(18-i);
            }

            //making sure the moves don't warp around to other side once shifter
            //as well as friendly and illegal king capture check
            if(i % 8 < 4){
                moves &= ~FILE_GH & ~friendlys & mrays;
            } else {
                moves &= ~FILE_AB & ~friendlys & mrays;
            }

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    //bishops blocks or captures
    //loop through and find bishops
    for(int i = 0; i < 64; i++){
        if(((bishops>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = DAndAntiDMoves(i) & ~friendlys & mrays;
            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    //queens blocks or captures
    //loop through and find queens
    for(int i = 0; i < 64; i++){
        if(((queens>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = DAndAntiDMoves(i) + horizVert(i) & ~friendlys & mrays;

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    return list;


}

//normal move stuff
std::string BitBoards::makeMove(std::string move)
{
    std::string savedMove;
    bool wOrB;
    int x, y, x1, y1, xyI, xyE;
    //inital spot piece mask and end spot mask
    U64 pieceMaskI = 0LL, pieceMaskE = 0LL;
    //for normal moves
    if(move[3] != 'Q'){
        //parse string move and change to ints
        x = move[0] - 0;
        y = move[1] - 0;
        x1 = move[2] - 0;
        y1 = move[3] - 0;
        xyI = y*8+x, xyE = y1*8+x1;
        pieceMaskI += 1LL<< xyI;
        pieceMaskE += 1LL << xyE;
        wOrB = isWhite(pieceMaskI);
    //for pawn promotions
    } else {
        x = move[0] - 0;
        y = move[1] - 0;
        xyI = y*8+x;
        pieceMaskI += 1LL<< xyI;
        wOrB = isWhite(pieceMaskI);
        //if promotion is white pawn
        if(wOrB == true){
            y1 = 0;
            //if it is a non capture forward promotion
            if(move[2] == 'F'){
                x1 = x;
                xyE = y1*8+x1;
                pieceMaskE += 1LL << xyE;
            //capture promotion
            } else {
                x1 = move[2]-0;
                xyE = y1*8+x1;
                pieceMaskE += 1LL << xyE;
            }
        //black promotions
        } else {
            y1 = 7;
            if(move[2] == 'F'){
                x1 = x;
                xyE = y1*8+x1;
                pieceMaskE += 1LL << xyE;
            } else {
                x1 = move[2]-0;
                xyE = y1*8+x1;
                pieceMaskE += 1LL << xyE;
            }
        }

    }


    //store coordiantes for undoing move
    //final order is x, y, x1, y1, piece moved, piece captured (0 if none)
    savedMove += x;
    savedMove += y;
    savedMove += x1;
    savedMove += y1;

    //find BB that contains correct piece, remove piece from it's starting pos
    //on piece BB, add piece to string savedMove, if it's a capture add piece to be captured,

    //white pieces
    if(wOrB == true){
        if(BBWhitePawns & pieceMaskI){
            //remove piece from starting loc
            BBWhitePawns &= ~pieceMaskI;
            //remove piece from color BB
            BBWhitePieces &= ~pieceMaskI;
            //remove piece from full tiles
            FullTiles &= ~pieceMaskI;
            //adds piece to move to be returned in order to undo move
            savedMove += "P";
            //removes piece from capture location if capture and returns piece char
            savedMove += isCapture(pieceMaskE, wOrB);

            if(move[3] != 'Q'){
                //add piece to landing spot
                BBWhitePawns |= pieceMaskE;

            //if it's a pawn promotion
            } else {
                //add queen to landing spot
                BBWhiteQueens |= pieceMaskE;
                //add promotion data to capture string
                savedMove += "O";
            }
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
        } else if (BBWhiteRooks & pieceMaskI){
            //remove piece, test/save capture
            BBWhiteRooks &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "R";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBWhiteRooks |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        } else if (BBWhiteKnights & pieceMaskI){
            BBWhiteKnights &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "N";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBWhiteKnights |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        } else if (BBWhiteBishops & pieceMaskI){
            BBWhiteBishops &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "B";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBWhiteBishops |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        } else if (BBWhiteQueens & pieceMaskI){
            BBWhiteQueens &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "Q";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBWhiteQueens |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        } else if (BBWhiteKing & pieceMaskI){
            BBWhiteKing &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "K";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBWhiteKing |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        }
    //black pieces
    } else {
        if(BBBlackPawns & pieceMaskI){
            BBBlackPawns &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "p";
            savedMove += isCapture(pieceMaskE, wOrB);

            if(move[3] != 'Q'){
                //add piece to landing spot
                BBBlackPawns |= pieceMaskE;

            //if it's a pawn promotion
            } else {
                //add queen to landing spot
                BBBlackQueens |= pieceMaskE;
                //add promotion data to capture string
                savedMove += "O";
            }
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        } else if (BBBlackRooks & pieceMaskI){
            BBBlackRooks &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "r";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBBlackRooks |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        } else if (BBBlackKnights & pieceMaskI){
            BBBlackKnights &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "n";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBBlackKnights |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        } else if (BBBlackBishops & pieceMaskI){
            BBBlackBishops &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "b";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBBlackBishops |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        } else if (BBBlackQueens & pieceMaskI){
            BBBlackQueens &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "q";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBBlackQueens |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;

        } else if (BBBlackKing & pieceMaskI){
            BBBlackKing &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "k";
            savedMove += isCapture(pieceMaskE, wOrB);
            //add piece
            BBBlackKing |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
        }
    }



    if(wOrB == true){
        savedMove += 'w';
    } else {
        savedMove += 'b';
    }

    //correct empty tiles to opposite of full tiles
    EmptyTiles &= ~pieceMaskE;
    EmptyTiles |= pieceMaskI;
    //EmptyTiles &= ~FullTiles
    //drawBBA();

    //Update zobrist hash
    ZKey->UpdateKey(xyI, xyE, savedMove);
    //update zobrist hash with color change
    ZKey->UpdateColor();

    return savedMove;


}

bool BitBoards::isWhite(U64 pieceMoving)
{
    //figure out if piece is white or black
    U64 wmask = BBWhitePieces;

    if(wmask & pieceMoving){
        return true;
    } else {
        return false;
    }
}

char BitBoards::isCapture(U64 landing, bool isWhite)
{
    //if is white doing the capturing
    if(isWhite == true){
        if(landing & BBBlackPieces){
            if(landing & BBBlackPawns){
                //remove captured piece from piece BB
                BBBlackPawns &= ~landing;
                //remove piece from color BB
                BBBlackPieces &= ~landing;
                return 'p';
            } else if (landing & BBBlackRooks){
                BBBlackRooks &= ~landing;
                BBBlackPieces &= ~landing;
                return 'r';
            } else if (landing & BBBlackKnights){
                BBBlackKnights &= ~landing;
                BBBlackPieces &= ~landing;
                return 'n';
            } else if (landing & BBBlackBishops){
                BBBlackBishops &= ~landing;
                BBBlackPieces &= ~landing;
                return 'b';
            } else if (landing & BBBlackQueens){
                BBBlackQueens &= ~landing;
                BBBlackPieces &= ~landing;
                return 'q';
            }
        } else {
            return '0';
        }
    } else {
        if(landing & BBWhitePieces){
            if(landing & BBWhitePawns){
                BBWhitePawns &= ~landing;
                BBWhitePieces &= ~landing;
                return 'P';
            } else if (landing & BBWhiteRooks){
                BBWhiteRooks &= ~landing;
                BBWhitePieces &= ~landing;
                return 'R';
            } else if (landing & BBWhiteKnights){
                BBWhiteKnights &= ~landing;
                BBWhitePieces &= ~landing;
                return 'N';
            } else if (landing & BBWhiteBishops){
                BBWhiteBishops &= ~landing;
                BBWhitePieces &= ~landing;
                return 'B';
            } else if (landing & BBWhiteQueens){
                BBWhiteQueens &= ~landing;
                BBWhitePieces &= ~landing;
                return 'Q';
            }
        } else {
            return '0';
        }
    }
    return '0';
}

void BitBoards::unmakeMove(std::string moveKey)
{

    //parse string move and change to ints
    int x = moveKey[0] - 0;
    int y = moveKey[1] - 0;
    int x1 = moveKey[2] - 0;
    int y1 = moveKey[3] - 0;
    int xyI = y*8+x, xyE = y1*8+x1;
    //inital spot piece mask and end spot mask
    U64 pieceMaskI = 0LL, pieceMaskE = 0LL;
    pieceMaskI += 1LL<< xyI;
    pieceMaskE += 1LL << xyE;

    //store piece moved and captured and promotion if was one
    char wOrB, promotion = 'X';

    //for normal moves
    char pieceMoved = moveKey[4], pieceCaptured = moveKey[5];

    if(moveKey[6] != 'O'){
        wOrB = moveKey[6];
    //for promotions
    } else {
        promotion = moveKey[6];
        wOrB = moveKey[7];
    }


    if(wOrB == 'w'){
        switch(pieceMoved){
            case 'P':
            //if move not a promotion
            if(promotion == 'X'){
                //remove piece from where it landed
                BBWhitePawns &= ~pieceMaskE;
            //promotion unmake
            } else {
                BBWhiteQueens &= ~pieceMaskE;
            }
            //put it back where it started
            BBWhitePawns |= pieceMaskI;
            //change color boards same way
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'R':
            BBWhiteRooks &= ~pieceMaskE;
            BBWhiteRooks |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'N':
            BBWhiteKnights &= ~pieceMaskE;
            BBWhiteKnights |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'B':
            BBWhiteBishops &= ~pieceMaskE;
            BBWhiteBishops |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'Q':
            BBWhiteQueens &= ~pieceMaskE;
            BBWhiteQueens |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'K':
            BBWhiteKing &= ~pieceMaskE;
            BBWhiteKing |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;
        }
    } else if(wOrB == 'b'){
        switch(pieceMoved){
            case 'p':
            if(promotion == 'X'){
                //remove piece from where it landed
                BBBlackPawns &= ~pieceMaskE;
            //promotion unmake
            } else {
                BBBlackQueens &= ~pieceMaskE;
            }
            //put it back where it started
            BBBlackPawns |= pieceMaskI;
            //change color boards same way
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'r':
            BBBlackRooks &= ~pieceMaskE;
            BBBlackRooks |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'n':
            BBBlackKnights &= ~pieceMaskE;
            BBBlackKnights |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'b':
            BBBlackBishops &= ~pieceMaskE;
            BBBlackBishops |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'q':
            BBBlackQueens &= ~pieceMaskE;
            BBBlackQueens |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'k':
            BBBlackKing &= ~pieceMaskE;
            BBBlackKing |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

        }
    }

    //correct full tiles and run unmake capture function
    //if a piece has been captured ///Might have errors in calling unmake
    //even when a piece hasn't been captured
    if(wOrB == 'w'){
        if(pieceCaptured == '0' || pieceCaptured == 0){
            FullTiles &= ~pieceMaskE;
            FullTiles |= pieceMaskI;
        } else{           
            undoCapture(pieceMaskE, pieceCaptured, 'b');
            FullTiles |= pieceMaskI;
        }
    } else {
        if(pieceCaptured == '0' || pieceCaptured == 0){
            FullTiles &= ~pieceMaskE;
            FullTiles |= pieceMaskI;
        } else{
            undoCapture(pieceMaskE, pieceCaptured, 'w');
            FullTiles |= pieceMaskI;
        }
    }


    //correct empty tiles to opposite of full tiles
    EmptyTiles = ~FullTiles;

    //update zobrist hash
    ZKey->UpdateKey(xyI, xyE, moveKey);
    //update zobrist hash with color change
    ZKey->UpdateColor();

}

void BitBoards::undoCapture(U64 location, char piece, char whiteOrBlack)
{

    if(whiteOrBlack == 'w'){
        switch(piece){
            case 'P':
                //restore piece to both piece board and color board
                //no need to change FullTiles as captured piece was already there
                BBWhitePawns |= location;
                BBWhitePieces |= location;
                break;
            case 'R':
                BBWhiteRooks |= location;
                BBWhitePieces |= location;
                break;
            case 'N':
                BBWhiteKnights |= location;
                BBWhitePieces |= location;
                break;
            case 'B':
                BBWhiteBishops |= location;
                BBWhitePieces |= location;
                break;
            case 'Q':
                BBWhiteQueens |= location;
                BBWhitePieces |= location;
                break;
            default:

                std::cout << "UNDO CAPTURE ERROR" << std::endl;

        }
    } else if (whiteOrBlack == 'b') {
        switch(piece){
            case 'p':
                //restore piece to both piece board and color board
                //no need to change FullTiles as captured piece was already there
                BBBlackPawns |= location;
                BBBlackPieces |= location;
                break;
            case 'r':
                BBBlackRooks |= location;
                BBBlackPieces |= location;
                break;
            case 'n':
                BBBlackKnights |= location;
                BBBlackPieces |= location;
                break;
            case 'b':
                BBBlackBishops |= location;
                BBBlackPieces |= location;
                break;
            case 'q':
                BBBlackQueens |= location;
                BBBlackPieces |= location;
                break;
            default:

                std::cout << "UNDO CAPTURE ERROR" << std::endl;
        }
    } else {

        std::cout << "UNDO CAPTURE ERROR" << std::endl;
    }
}

//lots of pinned piece functions - subset of moves
std::string BitBoards::makePinnedMovesLegal(bool isWhite,std::string moves, U64 wpawns, U64 wrooks, U64 wknights, U64 wbishops, U64 wqueens, U64 wking, U64 bpawns, U64 brooks, U64 bknights, U64 bbishops, U64 bqueens, U64 bking)
{
    U64 kingSafe;
    std::string move, legalMoves, toUndo;
    //loop through moves and make them, test legal, unmake
    for(int i = 0; i < moves.length()/4; i+=4){
        move = "";
        move += moves[i];
        move += moves[i+1];
        move += moves[i+2];
        move += moves[i+3];
        //make the move
        toUndo = makeMove(move);        
        //test if move is legal
        if(isWhite == true){

            kingSafe = unsafeForWhite(wpawns, wrooks, wknights, wbishops, wqueens, wking, bpawns, brooks,  bknights, bbishops, bqueens, bking);

            if(kingSafe & wking){
                //not legal move
            } else {
                legalMoves += move;
            }
        } else {
            kingSafe = unsafeForBlack(wpawns, wrooks, wknights, wbishops, wqueens, wking, bpawns, brooks,  bknights, bbishops, bqueens, bking);

            if(kingSafe & bking){
                //not legal move
            } else {
                legalMoves += move;
            }
        }

        //undo move
        unmakeMove(toUndo);

    }

    return legalMoves;

}

std::string BitBoards::removePinnedPieces(U64 pinnedBB, bool whiteOrBlack)
{

    std::string pieces;
    U64 i, pinnedPiece;
    int iLocation;
    if(whiteOrBlack == true){
        if(pinnedBB & BBWhitePawns){
            pinnedPiece = pinnedBB & BBWhitePawns;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                //find location of piece and append string with coordinates (x,y) and piece type
                iLocation = trailingZeros(i);
                pieces += 'P';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            //append just piece board so no more moves are generated outside pinned move gen
            BBWhitePawns &= ~pinnedBB;
        } else if(pinnedBB & BBWhiteRooks){
            pinnedPiece = pinnedBB & BBWhiteRooks;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                iLocation = trailingZeros(i);
                pieces += 'R';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            BBWhiteRooks &= ~pinnedBB;
        } else if(pinnedBB & BBWhiteKnights){
            pinnedPiece = pinnedBB & BBWhiteKnights;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                iLocation = trailingZeros(i);
                pieces += 'N';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            BBWhiteKnights &= ~pinnedBB;
        } else if(pinnedBB & BBWhiteBishops){
            pinnedPiece = pinnedBB & BBWhiteBishops;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                iLocation = trailingZeros(i);
                pieces += 'B';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            BBWhiteBishops &= ~pinnedBB;
        } else if(pinnedBB & BBWhiteQueens){
            pinnedPiece = pinnedBB & BBWhiteQueens;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                iLocation = trailingZeros(i);
                pieces += 'Q';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            BBWhiteQueens &= ~pinnedBB;
        }
    //black
    } else if (whiteOrBlack == false){
        if(pinnedBB & BBBlackPawns){
            pinnedPiece = pinnedBB & BBBlackPawns;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                //find location of piece and append string with coordinates (x,y) and piece type
                iLocation = trailingZeros(i);
                pieces += 'p';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            BBBlackPawns &= ~pinnedBB;
        } else if(pinnedBB & BBBlackRooks){
            pinnedPiece = pinnedBB & BBBlackRooks;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                //find location of piece and append string with coordinates (x,y) and piece type
                iLocation = trailingZeros(i);
                pieces += 'r';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            BBBlackRooks &= ~pinnedBB;
        } else if(pinnedBB & BBBlackKnights){
            pinnedPiece = pinnedBB & BBBlackKnights;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                //find location of piece and append string with coordinates (x,y) and piece type
                iLocation = trailingZeros(i);
                pieces += 'n';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            BBBlackKnights &= ~pinnedBB;
        } else if(pinnedBB & BBBlackBishops){
            pinnedPiece = pinnedBB & BBBlackBishops;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                //find location of piece and append string with coordinates (x,y) and piece type
                iLocation = trailingZeros(i);
                pieces += 'b';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            BBBlackBishops &= ~pinnedBB;
        } else if(pinnedBB & BBBlackQueens){
            pinnedPiece = pinnedBB & BBBlackQueens;
            i = pinnedPiece &~ (pinnedPiece-1);
            while(i != 0){
                //find location of piece and append string with coordinates (x,y) and piece type
                iLocation = trailingZeros(i);
                pieces += 'q';
                pieces += iLocation%8;
                pieces += iLocation/8;
                pinnedPiece &= ~i;
                i = pinnedPiece &~(pinnedPiece-1);
            }
            BBBlackQueens &= ~pinnedBB;
        }
    }


    return pieces;

}

void BitBoards::restorePinnedPieces(std::string pieces, bool wOrB)
{
    //TEST
    if(BBBlackKing == 0LL){
        std::cout << "king removed here" << std::endl;
    }

    int x, y, xy;
    if(wOrB == true){
        for(int i = 0; i < pieces.length(); i += 3){
            x = pieces[i+1] - 0;
            y = pieces[i+2] - 0;
            xy = y*8 + x;
            switch(pieces[i]){
                case 'P':
                    BBWhitePawns += 1LL << xy;
                    continue;
                case 'R':
                    BBWhiteRooks += 1LL << xy;
                    continue;
                case 'N':
                    BBWhiteKnights += 1LL << xy;
                    continue;
                case 'B':
                    BBWhiteBishops += 1LL << xy;
                    continue;
                case 'Q':
                    BBWhiteQueens += 1LL << xy;
                    continue;

            }
        }
    } else {
        for(int i = 0; i < pieces.length(); i += 3){
            x = pieces[i+1] - 0;
            y = pieces[i+2] - 0;
            xy = y*8 + x;
            switch(pieces[i]){
                case 'p':
                    BBBlackPawns += 1LL << xy;
                    continue;
                case 'r':
                    BBBlackRooks += 1LL << xy;
                    continue;
                case 'n':
                    BBBlackKnights += 1LL << xy;
                    continue;
                case 'b':
                    BBBlackBishops += 1LL << xy;
                    continue;
                case 'q':
                    BBBlackQueens += 1LL << xy;
                    continue;

            }
        }
    }

}

//first function calc pieces pinned between sliders and king second calcs pinned piece moves
U64 BitBoards::pinnedBB(U64 rooks, U64 bishops, U64 queens, U64 king)
{
    U64 pinned  = 0LL, uB, dB, rB, lB, nEB, nWB, sWB, sEB, tempRay, oneP;
    //function for finding pinned pieces and later removing them from normal move pool
    //in order to generate their moves only along their pinned ray

    //NOTE!!! return for loops to while(i!=0) loops for faster code

    //king up ray
    uB = up(king);
    //king down ray
    dB = down(king);
    //king right ray
    rB = right(king);
    //king ray left
    lB = left(king);
    //king NE ray
    nEB = upright(king);
    //king SE ray
    sEB = downright(king);
    //king SW ray
    sWB = downleft(king);
    //king NW ray
    nWB = upleft(king);


    //rooks / queens
    U64 QR = queens | rooks;
    for(int j = 0; j < 64; j++){
        oneP = 0LL;
        if( QR & (1ULL << j)){
            oneP += (1ULL << j);
            //calculate horiz and vert rays
            tempRay = up(oneP); // up qb ray
            pinned |= dB & tempRay; // merge with down k
            tempRay = down(oneP); //down qb
            pinned |= uB & tempRay; // merge with up k
            tempRay = right(oneP); //right qb
            pinned |= lB & tempRay; // left k
            tempRay = left(oneP); // left qb
            pinned |= rB & tempRay; // right k
        }

    }

    //bishops / queens
    U64 QB = queens | bishops;
    for(int j = 0; j < 64; j++){
        oneP = 0LL;
        if( QB & (1ULL << j)){
            oneP += (1ULL << j);
            //calculate all diagonal moves
            tempRay = upright(oneP); //up right qb
            pinned |= sWB & tempRay; //down left k
            tempRay = downright(oneP);
            pinned |= nWB & tempRay; // up left
            tempRay = downleft(oneP);
            pinned |= nEB & tempRay;
            tempRay = upleft(oneP);
            pinned |= sEB & tempRay;
        }
    }


    return pinned;
}

std::string BitBoards::pinnedMoves(U64 pinned, U64 opawns, U64 orooks, U64 obishops, U64 oqueens, U64 oking, U64 erooks, U64 ebishops, U64 equeens, U64 ourPieces, bool isWhite)
{
    //store full tiles so we can restore it to real later
    U64 tempStoreTiles = FullTiles;
    //king rays and enemy rays
    U64 uB, dB, rB, lB, nEB, nWB, sWB, sEB, tempRay, oneP, mrays = 0LL;
    //BB mask without pinned pieces in order to find possible moves
    FullTiles = FullTiles & ~pinned;

    std::string moves;

    //calculate move rays from enemy to king wihtout pinned pieces in the way

    //king up ray
    uB = up(oking);
    //king down ray
    dB = down(oking);
    //king right ray
    rB = right(oking);
    //king ray left
    lB = left(oking);
    //king NE ray
    nEB = upright(oking);
    //king SE ray
    sEB = downright(oking);
    //king SW ray
    sWB = downleft(oking);
    //king NW ray
    nWB = upleft(oking);


    //rooks / queens
    U64 QR = equeens | erooks;
    for(int j = 0; j < 64; j++){
        oneP = 0LL;
        if( QR & (1ULL << j)){
            oneP += (1ULL << j);
            //calculate horiz and vert rays
            tempRay = up(oneP); // up qb ray
            mrays |= dB & tempRay; // merge with down k
            tempRay = down(oneP); //down qb
            mrays |= uB & tempRay; // merge with up k
            tempRay = right(oneP); //right qb
            mrays |= lB & tempRay; // left k
            tempRay = left(oneP); // left qb
            mrays |= rB & tempRay; // right k
        }

    }


    //bishops / queens
    U64 QB = equeens | ebishops;
    for(int j = 0; j < 64; j++){
        oneP = 0LL;
        if( QB & (1ULL << j)){
            oneP += (1ULL << j);
            //calculate all diagonal moves
            tempRay = upright(oneP); //up right qb
            mrays |= sWB & tempRay; //down left k
            tempRay = downright(oneP);
            mrays |= nWB & tempRay; // up left
            tempRay = downleft(oneP);
            mrays |= nEB & tempRay;
            tempRay = upleft(oneP);
            mrays |= sEB & tempRay;
        }
    }


    //restore accurate full tiles
    FullTiles = tempStoreTiles;


    U64 enemyPieces = ~ourPieces & FullTiles, empty = ~FullTiles;

    //only check pinned pieces
    //pawns
    opawns = opawns & pinned;
    if(opawns != 0){
        moves += pinnedPawnCaptures(opawns, enemyPieces, mrays, isWhite);
        moves += pinnedPawnPushes(opawns, empty, mrays, isWhite);
    }
    //bishops
    obishops = obishops & pinned;
    if(obishops != 0){
        moves += pinnedBishopMoves(obishops, ourPieces, mrays);
    }
    //rooks
    orooks = orooks & pinned;
    if(orooks != 0){
        moves += pinnedRookMoves(orooks, ourPieces, mrays);
    }
    //queens
    oqueens = oqueens & pinned;
    if(oqueens != 0){
        moves += pinnedQueenMoves(oqueens, ourPieces, mrays);
    }

    return moves;

}

std::string BitBoards::pinnedCaptures(U64 pinned, U64 opawns, U64 orooks, U64 obishops, U64 oqueens, U64 oking, U64 erooks, U64 ebishops, U64 equeens, U64 ourPieces, U64 enemyPieces, bool isWhite)
{
    //store full tiles so we can restore it to real later
    U64 tempStoreTiles = FullTiles;
    //king rays and enemy rays
    U64 uB, dB, rB, lB, nEB, nWB, sWB, sEB, tempRay, oneP, mrays = 0LL;
    //BB mask without pinned pieces in order to find possible moves
    FullTiles = FullTiles & ~pinned;

    std::string moves;

    //calculate move rays from enemy to king wihtout pinned pieces in the way

    //king up ray
    uB = up(oking);
    //king down ray
    dB = down(oking);
    //king right ray
    rB = right(oking);
    //king ray left
    lB = left(oking);
    //king NE ray
    nEB = upright(oking);
    //king SE ray
    sEB = downright(oking);
    //king SW ray
    sWB = downleft(oking);
    //king NW ray
    nWB = upleft(oking);


    //rooks / queens
    U64 QR = equeens | erooks;
    for(int j = 0; j < 64; j++){
        oneP = 0LL;
        if( QR & (1ULL << j)){
            oneP += (1ULL << j);
            //calculate horiz and vert rays
            tempRay = up(oneP); // up qb ray
            mrays |= dB & tempRay; // merge with down k
            tempRay = down(oneP); //down qb
            mrays |= uB & tempRay; // merge with up k
            tempRay = right(oneP); //right qb
            mrays |= lB & tempRay; // left k
            tempRay = left(oneP); // left qb
            mrays |= rB & tempRay; // right k
        }

    }

    //bishops / queens
    U64 QB = equeens | ebishops;
    for(int j = 0; j < 64; j++){
        oneP = 0LL;
        if( QB & (1ULL << j)){
            oneP += (1ULL << j);
            //calculate all diagonal moves
            tempRay = upright(oneP); //up right qb
            mrays |= sWB & tempRay; //down left k
            tempRay = downright(oneP);
            mrays |= nWB & tempRay; // up left
            tempRay = downleft(oneP);
            mrays |= nEB & tempRay;
            tempRay = upleft(oneP);
            mrays |= sEB & tempRay;
        }
    }

    //restore accurate full tiles
    FullTiles = tempStoreTiles;

    //only check pinned pieces
    //pawns
    opawns = opawns & pinned;
    if(opawns != 0){
        moves += pinnedPawnCaptures(opawns, enemyPieces, mrays, isWhite);
    }
    //bishops
    obishops = obishops & pinned;
    if(obishops != 0){
        moves += pinnedBishopMoves(obishops, ourPieces, mrays);
    }
    //rooks
    orooks = orooks & pinned;
    if(orooks != 0){
        moves += pinnedRookMoves(orooks, ourPieces, mrays);
    }
    //queens
    oqueens = oqueens & pinned;
    if(oqueens != 0){
        moves += pinnedQueenMoves(oqueens, ourPieces, mrays);
    }

    return moves;
}

std::string BitBoards::pinnedPawnCaptures(U64 opawns, U64 enemyPieces, U64 mrays, bool isWhite)
{
    std::string moves;
    //pinned pawn captures
    U64 PAWN_MOVES;
    if(isWhite == true){
        //capture right
        PAWN_MOVES = noEaOne(opawns) & enemyPieces & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                moves+=i%8-1;
                moves+=i/8+1;
                moves+=i%8;
                moves+=i/8;

            }
        }

        //capture left
        PAWN_MOVES = noWeOne(opawns) & enemyPieces & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                moves+=i%8+1;
                moves+=i/8+1;
                moves+=i%8;
                moves+=i/8;

            }
        }
    } else {
        //capture right
        PAWN_MOVES = soEaOne(opawns) & enemyPieces & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                moves+=i%8-1;
                moves+=i/8-1;
                moves+=i%8;
                moves+=i/8;

            }
        }

        //capture left
        PAWN_MOVES = soWeOne(opawns) & enemyPieces & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                moves+=i%8+1;
                moves+=i/8-1;
                moves+=i%8;
                moves+=i/8;

            }
        }
        return moves;
    }
}

std::string BitBoards::pinnedPawnPushes(U64 opawns, U64 EmptyTiles, U64 mrays, bool isWhite)
{
    std::string moves;
    U64 PAWN_MOVES;
    if(isWhite == true){
        //forward one
        U64 PAWN_MOVES = northOne(opawns) & EmptyTiles & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                moves+=i%8;
                moves+=i/8+1;
                moves+=i%8;
                moves+=i/8;

            }
        }

        //forward two
        PAWN_MOVES = (opawns>>16) & EmptyTiles &(EmptyTiles>>8) &rank4 & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                moves+=i%8;
                moves+=i/8+2;
                moves+=i%8;
                moves+=i/8;

            }
        }

    } else {
        //forward one
        U64 PAWN_MOVES = southOne(opawns) & EmptyTiles & mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                //list+=""+(i/8+1)+(i%8)+(i/8)+(i%8);
                moves+=i%8;
                moves+=i/8-1;
                moves+=i%8;
                moves+=i/8;

            }
        }

        //forward two
        PAWN_MOVES = (opawns<<16) & EmptyTiles &(EmptyTiles<<8) & rank5 &mrays;
        for(int i = 0; i < 64; i++){
            if(((PAWN_MOVES>>i)&1)==1){
                moves+=i%8;
                moves+=i/8-2;
                moves+=i%8;
                moves+=i/8;

            }
        }

    }
    return moves;

}

std::string BitBoards::pinnedBishopMoves(U64 obishops, U64 ourPieces, U64 mrays)
{
    std::string list;
    U64 moves;
    //loop through and find bishops
    for(int i = 0; i < 64; i++){
        if(((obishops>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = DAndAntiDMoves(i) & ~ourPieces & mrays;
            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    return list;
}

std::string BitBoards::pinnedRookMoves(U64 orooks, U64 ourPieces, U64 mrays)
{
    std::string list;
    U64 moves;

    //loop through and find rooks
    for(int i = 0; i < 64; i++){
        if(((orooks>>i) &1) == 1){
            //map moves that don't collide with friendlys and only are along pin lines
            moves = horizVert(i) & ~ourPieces  & mrays;

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    return list;
}

std::string BitBoards::pinnedQueenMoves(U64 oqueens, U64 ourPieces, U64 mrays)
{
    std::string list;
    U64 moves;
    //loop through and find bishops
    for(int i = 0; i < 64; i++){
        if(((oqueens>>i) &1) == 1){
            //map moves that don't collide with friendlys and is only along pinned ray
            moves = DAndAntiDMoves(i) + horizVert(i) & ~ourPieces & mrays;

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    return list;
}


//pawn moves
std::string BitBoards::possibleWP(U64 wpawns, U64 EmptyTiles, U64 blackking)
{
    std::string list= "";

    //forward one
    U64 PAWN_MOVES = northOne(wpawns) & EmptyTiles;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8;
            list+=i/8+1;
            list+=i%8;
            list+=i/8;

        }
    }

    //forward two
    PAWN_MOVES = (wpawns>>16) & EmptyTiles &(EmptyTiles>>8) &rank4;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8;
            list+=i/8+2;
            list+=i%8;
            list+=i/8;

        }
    }

    //capture right
    PAWN_MOVES = noEaOne(wpawns) & BBBlackPieces & ~blackking;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8-1;
            list+=i/8+1;
            list+=i%8;
            list+=i/8;

        }
    }

    //capture left
    PAWN_MOVES = noWeOne(wpawns) & BBBlackPieces & ~blackking;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8+1;
            list+=i/8+1;
            list+=i%8;
            list+=i/8;

        }
    }

//Pawn promotions moving forward one
    PAWN_MOVES = northOne(wpawns) & EmptyTiles & rank8;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8;
            list+=i/8+1;
            list+="F";
            list+="Q";

        }
    }
    //pawn capture promotions
    //capture right
    PAWN_MOVES = noEaOne(wpawns) & BBBlackPieces & ~blackking & rank8;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8-1;
            list+=i/8+1;
            list+=i%8;
            list+="Q";

        }
    }

    //capture left
    PAWN_MOVES = noWeOne(wpawns) & BBBlackPieces & ~blackking & rank8;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8+1;
            list+=i/8+1;
            list+=i%8;
            list+="Q";

        }
    }

    return list;
}

std::string BitBoards::possibleBP(U64 bpawns, U64 EmptyTiles, U64 whiteking)
{
    std::string list= "";

    //forward one
    U64 PAWN_MOVES = southOne(bpawns) & EmptyTiles;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8;
            list+=i/8-1;
            list+=i%8;
            list+=i/8;

        }
    }

    //forward two
    PAWN_MOVES = (bpawns<<16) & EmptyTiles &(EmptyTiles<<8) & rank5;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8;
            list+=i/8-2;
            list+=i%8;
            list+=i/8;

        }
    }

    //capture right
    PAWN_MOVES = soEaOne(bpawns) & BBWhitePieces & ~whiteking;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8-1;
            list+=i/8-1;
            list+=i%8;
            list+=i/8;

        }
    }

    //capture left
    PAWN_MOVES = soWeOne(bpawns) & BBWhitePieces & ~whiteking;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8+1;
            list+=i/8-1;
            list+=i%8;
            list+=i/8;

        }
    }

//promotions
    PAWN_MOVES = southOne(bpawns) & EmptyTiles & rank1;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8;
            list+=i/8-1;
            list+='F';
            list+='Q';

        }
    }
    //capture promotions
    //capture right
    PAWN_MOVES = soEaOne(bpawns) & BBWhitePieces & ~whiteking & rank1;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8-1;
            list+=i/8-1;
            list+=i%8;
            list+='Q';

        }
    }

    //capture left
    PAWN_MOVES = soWeOne(bpawns) & BBWhitePieces & ~whiteking & rank1;
    for(int i = 0; i < 64; i++){
        if(((PAWN_MOVES>>i)&1)==1){
            list+=i%8+1;
            list+=i/8-1;
            list+=i%8;
            list+='Q';

        }
    }

    //drawBB(EmptyTiles);
    return list;
}

//other piece moves
std::string BitBoards::possibleR(U64 wOrBrooks, U64 wOrBpieces, U64 oppositeking)
{
    std::string list;
    U64 moves;
    //loop through and find rooks
    for(int i = 0; i < 64; i++){
        if(((wOrBrooks>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = horizVert(i) & ~wOrBpieces & ~oppositeking;

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }

    return list;
}

std::string BitBoards::possibleN(U64 wOrBknights, U64 wOrBpieces, U64 oppositeking)
{
    std::string list;
    U64 moves;
    //loop through and find knights
    for(int i = 0; i < 64; i++){
        if(((wOrBknights>>i) &1) == 1){
            //use the knight span board which holds possible knight moves
            //and apply a shift to the knights current pos
            if(i > 18){
                moves = KNIGHT_SPAN<<(i-18);
            } else {
                moves = KNIGHT_SPAN>>(18-i);
            }

            //making sure the moves don't warp around to other side once shifter
            //as well as friendly and illegal king capture check
            if(i % 8 < 4){
                moves &= ~FILE_GH & ~wOrBpieces & ~oppositeking;
            } else {
                moves &= ~FILE_AB & ~wOrBpieces & ~oppositeking;
            }

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }

    return list;
}

std::string BitBoards::possibleB(U64 wOrBbishops, U64 wOrBpieces, U64 oppositeking)
{

    std::string list;
    U64 moves;
    //loop through and find bishops
    for(int i = 0; i < 64; i++){
        if(((wOrBbishops>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = DAndAntiDMoves(i) & ~wOrBpieces & ~oppositeking;
            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }
    return list;

}

std::string BitBoards::possibleQ(U64 wOrBqueens, U64 wOrBpieces, U64 oppositeking)
{
    std::string list;
    U64 moves;
    //loop through and find bishops
    for(int i = 0; i < 64; i++){
        if(((wOrBqueens>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = DAndAntiDMoves(i) + horizVert(i) & ~wOrBpieces & ~oppositeking;

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }

    return list;
}

std::string BitBoards::possibleK(U64 wOrBking, U64 wOrBpieces, U64 kingSafeLessKing)
{
    std::string list;
    U64 moves;

    //use safety of king board without king in it, so king can't move to a "safe" area
    // opposite a potential ray piece

    int i = trailingZeros(wOrBking);
    if(i > 9){
        moves = KING_SPAN << (i-9);

    } else {
        moves = KING_SPAN >> (9-i);
    }

    if(i % 8 < 4){
        moves &= ~FILE_GH & ~wOrBpieces;

    } else {
        moves &= ~FILE_AB & ~wOrBpieces;
    }
    //check king unsafe board against king moves
    //removing places the king would be unsafe from the moves
    moves &= ~kingSafeLessKing;

    U64 j = moves &~(moves-1);


    while(j != 0){
        int index = trailingZeros(j);

        list += i % 8;
        list += i / 8;
        list += index % 8;
        list += index / 8;

        moves &= ~j;
        j = moves &~ (moves-1);
    }

return list;

}

//implement into other MOVE GEN ASIDE FROM KINGS, MUCH FASTER THAN for 64 loop
int BitBoards::trailingZeros(U64 i)
{
    //find the first one and number of zeros after it
    if (i == 0) return 64;
    U64 x = i;
    U64 y;
    int n = 63;
    y = x << 32; if (y != 0) { n -= 32; x = y; }
    y = x << 16; if (y != 0) { n -= 16; x = y; }
    y = x <<  8; if (y != 0) { n -=  8; x = y; }
    y = x <<  4; if (y != 0) { n -=  4; x = y; }
    y = x <<  2; if (y != 0) { n -=  2; x = y; }
    return (int) ( n - ((x << 1) >> 63));
}

U64 BitBoards::horizVert(int s)
{
    //convert slider location to 64 bit binary
    U64 binaryS = 1LL << s;

    //left and right moves
    U64 possibilitiesHorizontal = (FullTiles - 2 * binaryS) ^ ReverseBits(ReverseBits(FullTiles) - 2 * ReverseBits(binaryS));
    //moves up and down
    U64 possibilitiesVertical = ((FullTiles & FileMasks8[s % 8]) - (2 * binaryS)) ^ ReverseBits(ReverseBits(FullTiles & FileMasks8[s % 8]) - (2 * ReverseBits(binaryS)));

    //NOTE need to & against friendlys. Incomplete mask of moves
    return (possibilitiesHorizontal & RankMasks8[s / 8]) | (possibilitiesVertical & FileMasks8[s % 8]);

}

U64 BitBoards::DAndAntiDMoves(int s)
{
    U64 binaryS = 1LL <<s;

    U64 possibilitiesDiagonal = ((FullTiles & DiagonalMasks8[(s / 8) + (s % 8)]) - (2 * binaryS)) ^ ReverseBits(ReverseBits(FullTiles & DiagonalMasks8[(s / 8) + (s % 8)]) - (2 * ReverseBits(binaryS)));

    U64 possibilitiesAntiDiagonal = ((FullTiles & AntiDiagonalMasks8[(s / 8) + 7 - (s % 8)]) - (2 * binaryS)) ^ ReverseBits(ReverseBits(FullTiles & AntiDiagonalMasks8[(s / 8) + 7 - (s % 8)]) - (2 * ReverseBits(binaryS)));

    return (possibilitiesDiagonal & DiagonalMasks8[(s / 8) + (s % 8)]) | (possibilitiesAntiDiagonal & AntiDiagonalMasks8[(s / 8) + 7 - (s % 8)]);
}

U64 BitBoards::ReverseBits(U64 input)
{
    //literally reverse bits in U64's
    U64 output = input;
    for (int i = sizeof(input) * 8-1; i; --i)
    {
        output <<= 1;
        input  >>= 1;
        output |=  input & 1;
    }
    return output;
}

void BitBoards::constructBoards()
{
    FullTiles = 0LL;

    BBWhitePawns = 0LL;
    BBWhitePieces = 0LL;
    BBWhiteRooks = 0LL;
    BBWhiteKnights = 0LL;
    BBWhiteBishops = 0LL;
    BBWhiteQueens = 0LL;
    BBWhiteKing = 0LL;

    BBBlackPieces = 0LL;
    BBBlackPawns = 0LL;
    BBBlackRooks = 0LL;
    BBBlackKnights = 0LL;
    BBBlackBishops = 0LL;
    BBBlackQueens = 0LL;
    BBBlackKing = 0LL;

    //seed bitboards
    for(int i = 0; i < 64; i++){
        if(boardArr[i/8][i%8] == "P"){
            BBWhitePawns += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        } else if(boardArr[i/8][i%8] == "R"){
            BBWhiteRooks += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "N"){
            BBWhiteKnights += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "B"){
            BBWhiteBishops += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "Q"){
            BBWhiteQueens += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "K"){
            BBWhiteKing += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        } else if(boardArr[i/8][i%8] == "p"){
            BBBlackPawns += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        } else if(boardArr[i/8][i%8] == "r"){
            BBBlackRooks += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "n"){
            BBBlackKnights += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "b"){
            BBBlackBishops += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "q"){
            BBBlackQueens += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "k"){
            BBBlackKing += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        } //else if (boardArr[i/8][i%8] == " "){
          //  EmptyTiles += 1LL<<i;
       // }

    }


    //mark empty tiles with 1's
    EmptyTiles = ~FullTiles;
    //drawBB(EmptyTiles);
    std::cout << std::endl;

}

void BitBoards::drawBB(U64 board)
{
    for(int i = 0; i < 64; i++){
        if(board & (1ULL << i)){
            std::cout<< 1 <<", ";
        } else {
            std::cout << 0 << ", ";
        }
        if((i+1)%8 == 0){
            std::cout<< std::endl;
        }
    }
    std::cout<< std::endl;
}

void BitBoards::drawBBA()
{
    for(int i = 0; i < 64; i++){
        if((i)%8 == 0){
            std::cout<< std::endl;
        }
        if(BBWhitePawns & (1ULL<<i)){
            std::cout << "P" << ", ";
        }
        if(BBWhiteRooks & (1ULL<<i)){
            std::cout << "R" << ", ";
        }
        if(BBWhiteKnights & (1ULL<<i)){
            std::cout << "N" << ", ";
        }
        if(BBWhiteBishops & (1ULL<<i)){
            std::cout << "B" << ", ";
        }
        if(BBWhiteQueens & (1ULL<<i)){
            std::cout << "Q" << ", ";
        }
        if(BBWhiteKing & (1ULL<<i)){
            std::cout << "K" << ", ";
        }
        if(BBBlackPawns & (1ULL<<i)){
            std::cout << "p" << ", ";
        }
        if(BBBlackRooks & (1ULL<<i)){
            std::cout << "r" << ", ";
        }
        if(BBBlackKnights & (1ULL<<i)){
            std::cout << "n" << ", ";
        }
        if(BBBlackBishops & (1ULL<<i)){
            std::cout << "b" << ", ";
        }
        if(BBBlackQueens & (1ULL<<i)){
            std::cout << "q" << ", ";
        }
        if(BBBlackKing & (1ULL<<i)){
            std::cout << "k" << ", ";
        }
        if(EmptyTiles & (1ULL<<i)){
            std::cout << " " << ", ";
        }
    }
    std::cout << std::endl << std::endl;;
}
//unused function currently
std::string BitBoards::generateCaptures(bool isWhite, bool removeVarIfTryingToUseLater){
//Generates mostly captures except in check or double check in which case it generates all moves
    U64 moves, enemys, friends, knights, pawns, bishops, rooks, queens, king, epawns, erooks, eknights, ebishops, equeens, eking, unsafeTiles, kingSafeLessKing, checkers, pinned;
    std::string list;
    if(isWhite){
        enemys = BBBlackPieces & ~BBBlackKing;
        friends = BBWhitePieces;
        pawns = BBWhitePawns;
        knights = BBWhiteKnights;
        bishops = BBWhiteBishops;
        rooks = BBWhiteRooks;
        queens = BBWhiteQueens;
        king = BBWhiteKing;

        epawns = BBBlackPawns;
        eknights = BBBlackKnights;
        ebishops = BBBlackBishops;
        erooks = BBBlackRooks;
        equeens = BBBlackQueens;
        eking = BBBlackKing;
        //generate unsafe tiles for in check checking
        unsafeTiles = unsafeForWhite(pawns, rooks, knights, bishops, queens, king, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    } else {
        enemys = BBWhitePieces & ~BBWhiteKing;
        friends = BBBlackPieces;
        pawns = BBBlackPawns;
        knights = BBBlackKnights;
        bishops = BBBlackBishops;
        rooks = BBBlackRooks;
        queens = BBBlackQueens;
        king = BBBlackKing;

        epawns = BBWhitePawns;
        eknights = BBWhiteKnights;
        ebishops = BBWhiteBishops;
        erooks = BBWhiteRooks;
        equeens = BBWhiteQueens;
        eking = BBWhiteKing;

        unsafeTiles = unsafeForBlack(BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, pawns, rooks, knights, bishops, queens, king);
    }

    //if king is in check
    if(king & unsafeTiles){
        //find out if it's double check
        checkers = checkersBB(king, isWhite);

        //if we're in double check only generate king moves
        if(isDoubleCheck(checkers)){
            return list;
            //generate king safety array without king in it, pass to king move gen (blank board in place of our king)
            if(isWhite){
                kingSafeLessKing = unsafeForWhite(pawns, rooks, knights, bishops, queens, 0LL, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
            } else {
                kingSafeLessKing = unsafeForBlack(BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, pawns, rooks, knights, bishops, queens, 0LL);
            }

            //generates legal king moves
            list += possibleK(king, friends, kingSafeLessKing);

            return list;
        }
        return list;
        //if we're only in single check, generate moves that either,..
        //take out checking piece or block it's ray if it's a ray piece
        list += genInCheckMoves(checkers, king, isWhite);
        //generate king safety array without king in it, pass to king move gen (blank board in place of our king)
        if(isWhite){
            kingSafeLessKing = unsafeForWhite(pawns, rooks, knights, bishops, queens, 0LL, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
        } else {
            kingSafeLessKing = unsafeForBlack(BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, pawns, rooks, knights, bishops, queens, 0LL);
        }
        //generates legal king moves
        list += possibleK(king, friends, kingSafeLessKing);

        return list;
    }

    //generate pinned BB and remove pieces from it for sepperate move gen ~~ opposite piece color aside from king
    pinned = pinnedBB(erooks, ebishops, equeens, king);
    //custom bitboard to ensure pinned piece move generation only accepts landing on the enemy
    U64 ourCaps = friends | EmptyTiles;
    list += pinnedCaptures(pinned, pawns, rooks, bishops, queens, king, erooks, ebishops, equeens, ourCaps, enemys, isWhite);
    //test all pinned moves against king safety to be sure they're legal
    list = makePinnedMovesLegal(isWhite, list, pawns, rooks, knights, bishops, queens, king, epawns, erooks, eknights, ebishops, equeens, eking);
    //remove pinned pieces from normal piece generation and store into string so can be restored
    std::string removedPinned = removePinnedPieces(pinned, isWhite);

    //white pawn captures + set enemy mask + set friends and pieces masks
    if(isWhite){

        //capture right
        moves = noEaOne(pawns) & enemys;
        for(int i = 0; i < 64; i++){
            if(((moves>>i)&1)==1){
                list+=i%8-1;
                list+=i/8+1;
                list+=i%8;
                list+=i/8;

            }
        }

        //capture left
        moves = noWeOne(pawns) & enemys;
        for(int i = 0; i < 64; i++){
            if(((moves>>i)&1)==1){
                list+=i%8+1;
                list+=i/8+1;
                list+=i%8;
                list+=i/8;

            }
        }
    //black pawn captures
    } else {
        //capture right
        moves = soEaOne(pawns) & enemys;
        for(int i = 0; i < 64; i++){
            if(((moves>>i)&1)==1){
                list+=i%8-1;
                list+=i/8-1;
                list+=i%8;
                list+=i/8;

            }
        }

        //capture left
        moves = soWeOne(pawns) & enemys;
        for(int i = 0; i < 64; i++){
            if(((moves>>i)&1)==1){
                list+=i%8+1;
                list+=i/8-1;
                list+=i%8;
                list+=i/8;

            }
        }
    }

    //knight captures
    for(int i = 0; i < 64; i++){
        if(((knights>>i) &1) == 1){
            //use the knight span board which holds possible knight moves
            //and apply a shift to the knights current pos
            if(i > 18){
                moves = KNIGHT_SPAN<<(i-18);
            } else {
                moves = KNIGHT_SPAN>>(18-i);
            }

            //making sure the moves don't warp around to other side once shifter
            //as well as friendly and illegal king capture check
            if(i % 8 < 4){
                moves &= ~FILE_GH & ~friends & enemys;
            } else {
                moves &= ~FILE_AB & ~friends & enemys;
            }

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }

    //bishops captures
    for(int i = 0; i < 64; i++){
        if(((bishops>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = DAndAntiDMoves(i) & ~friends & enemys;
            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }

    //rook captures
    for(int i = 0; i < 64; i++){
        if(((rooks>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = horizVert(i) & ~friends & enemys;
            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }

    //queen captures
    for(int i = 0; i < 64; i++){
        if(((queens>>i) &1) == 1){
            //map moves that don't collide with friendlys and doesn't illegaly take black king
            moves = DAndAntiDMoves(i) + horizVert(i) & ~friends & enemys;

            for(int j = 0; j < 64; j++){
                if(((moves>>j) &1) == 1){
                    list+=i%8;
                    list+=i/8;
                    list+=j%8;
                    list+=j/8;
                }
            }
        }
    }

    //create unsafe board for king
    if(isWhite){
        kingSafeLessKing = unsafeForWhite(pawns, rooks, knights, bishops, queens, 0LL, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    } else {
        kingSafeLessKing = unsafeForBlack(BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, pawns, rooks, knights, bishops, queens, 0LL);
    }

    //restore pinned pieces to master BB's
    restorePinnedPieces(removedPinned, isWhite);

    //king captures
    int i = trailingZeros(king);
    if(i > 9){
        moves = KING_SPAN << (i-9);

    } else {
        moves = KING_SPAN >> (9-i);
    }

    if(i % 8 < 4){
        moves &= ~FILE_GH & ~friends &enemys;

    } else {
        moves &= ~FILE_AB & ~friends &enemys;
    }
    //check king unsafe board against king moves
    //removing places the king would be unsafe from the moves
    moves &= ~kingSafeLessKing;

    U64 j = moves &~(moves-1);

    while(j != 0){
        int index = trailingZeros(j);

        list += i % 8;
        list += i / 8;
        list += index % 8;
        list += index / 8;

        moves &= ~j;
        j = moves &~ (moves-1);
    }

    return list;
}

std::string BitBoards::generateCaptures(bool isWhite)
{
    //parses normal moves generated for captures and promotions
    std::string moves = genWhosMove(isWhite);

    U64 enemys;
    std::string tempMove, captures;

    //create bitboard of enemys
    if(isWhite){
        enemys = BBBlackPieces & ~BBBlackKing;
    } else {
        enemys = BBWhitePieces & ~BBWhiteKing;
    }

    int x1, y1, xyE;
    U64 pieceMaskE;
    //parse through moves for promotions
    for(int i = 0; i < moves.length(); i += 4){
        if(moves[i+3] == 'Q'){
            captures += moves[i];
            captures += moves[i+1];
            captures += moves[i+2];
            captures += moves[i+3];
        }
    }


    //search through all moves from turn for captures
    for(int i = 0; i < moves.length(); i += 4){
        pieceMaskE = 0LL;

        tempMove = "";
        tempMove += moves[i];
        tempMove += moves[i+1];
        tempMove += moves[i+2];
        tempMove += moves[i+3];
        //find number representing board  xy
        x1 = tempMove[2]-0; y1 = tempMove[3]-0;
        xyE = y1*8+x1;
        //create mask of move end position
        pieceMaskE += 1LL << xyE;
        //if move ends on an enemy, it's a capture
        if(pieceMaskE & enemys){
            captures += tempMove;
        }
    }

    return captures;

}

bool BitBoards::isPlayerMoveValid(int x, int y, int x1, int y1, bool isWhite)
{
    //gen moves all moves and compare against player move
    std::string pMoves = genWhosMove(isWhite);

    std::string move;
    move += (char)x;
    move += (char)y;
    move += (char)x1;
    move += (char)y1;
    //scan through all possible moves and see if players move is valid
    for(int i = 0; i < pMoves.length(); i += 4){
        std::string tMove;
        tMove += pMoves[i];
        tMove += pMoves[i+1];
        tMove += pMoves[i+2];
        tMove += pMoves[i+3];
        if(tMove == move){
            return true;
        }
    }
    return false;
}

bool BitBoards::isInCheck(bool isWhite)
{
    U64 knights, pawns, bishops, rooks, queens, king, unsafeTiles;
    std::string list;
    if(isWhite){
        pawns = BBWhitePawns;
        knights = BBWhiteKnights;
        bishops = BBWhiteBishops;
        rooks = BBWhiteRooks;
        queens = BBWhiteQueens;
        king = BBWhiteKing;
        //generate unsafe tiles for in check checking
        unsafeTiles = unsafeForWhite(pawns, rooks, knights, bishops, queens, king, BBBlackPawns, BBBlackRooks, BBBlackKnights, BBBlackBishops, BBBlackQueens, BBBlackKing);
    } else {
        pawns = BBBlackPawns;
        knights = BBBlackKnights;
        bishops = BBBlackBishops;
        rooks = BBBlackRooks;
        queens = BBBlackQueens;
        king = BBBlackKing;
        unsafeTiles = unsafeForBlack(BBWhitePawns, BBWhiteRooks, BBWhiteKnights, BBWhiteBishops, BBWhiteQueens, BBWhiteKing, pawns, rooks, knights, bishops, queens, king);
    }
    if(unsafeTiles & king){
        return true;
    } else {
        return false;
    }
}

//single ray direction functions -- mostly for pinned piece calcs
U64 BitBoards::up(U64 piece)
{
    U64 upB = piece;
    for(int i = 0; i < 8; i++){
        if((upB >> 8) & FullTiles){
            upB = upB | (upB >> 8);
            break;
        }
        upB = upB | (upB >> 8);
    }
    return upB;
}

U64 BitBoards::down(U64 piece)
{
    U64 dB = piece;
    for(int i = 0; i < 8; i++){
        if((dB << 8) & FullTiles){
            dB = dB | (dB << 8);
            break;
        }
        dB = dB | (dB << 8);
    }
    return dB;
}

U64 BitBoards::right(U64 piece)
{
    U64 rB = piece;
    for(int i = 0; i < 8; i++){
        if ((rB << 1) & FileABB){
            break;
        }
        if((rB << 1) & FullTiles){
            rB = rB | (rB << 1);
            break;
        }
        rB = rB | (rB << 1);
    }
    return rB;
}

U64 BitBoards::left(U64 piece)
{
    U64 lB = piece;
    for(int i = 0; i < 8; i++){
        if ((lB >> 1) & FileHBB){
            break;
        }
        if((lB >> 1) & FullTiles){
            lB = lB | (lB >> 1);
            break;
        }
        lB = lB | (lB >> 1);
    }
    return lB;
}

U64 BitBoards::upright(U64 piece)
{
    U64 nEB = piece;
    for(int i = 0; i < 8; i++){
        if((nEB >> 7) & FileABB){
            break;
        }
        if((nEB >> 7) & FullTiles){
            nEB |= nEB >> 7;
            break;
        }
        nEB |= nEB >> 7;
    }
    return nEB;
}

U64 BitBoards::downright(U64 piece)
{
    U64 sEB = piece;
    for(int i = 0; i < 8; i++){
        if((sEB << 9) & FileABB){
            break;
        }
        if((sEB << 9) & FullTiles){
            sEB |= sEB << 9;
            break;
        }
        sEB |= sEB << 9;
    }
    return sEB;
}

U64 BitBoards::downleft(U64 piece)
{
    U64 sWB = piece;
    for(int i = 0; i < 8; i++){
        if((sWB << 7) & FileHBB){
            break;
        }
        if((sWB << 7) & FullTiles){
            sWB |= sWB << 7;
            break;
        }
        sWB |= sWB << 7;
    }
    return sWB;
}

U64 BitBoards::upleft(U64 piece)
{
    U64 nWB = piece;
    for(int i = 0; i < 8; i++){
        if((nWB >> 9) & FileHBB){
            break;
        }
        if((nWB >> 9) & FullTiles){
            nWB |= nWB >> 9;
            break;
        }
        nWB |= nWB >> 9;
    }
    return nWB;
}

U64 BitBoards::northOne(U64 b)
{
    return b >> 8;
}

U64 BitBoards::southOne(U64 b)
{
    return b << 8;
}

U64 BitBoards::eastOne (U64 b)
{
    return (b << 1) & notHFile;
}

U64 BitBoards::noEaOne(U64 b)
{
    return (b >> 7) & notHFile;;
}

U64 BitBoards::soEaOne(U64 b)
{
    return (b << 9) & notHFile;
}

U64 BitBoards::westOne(U64 b)
{
    return (b >> 1) & notAFile;
}

U64 BitBoards::soWeOne(U64 b)
{

    return (b << 7) & notAFile;
}

U64 BitBoards::noWeOne(U64 b)
{
    return (b >> 9) & notAFile;
}






















