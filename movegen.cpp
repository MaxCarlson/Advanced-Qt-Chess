#include "movegen.h"
#include "Pieces.h"
const U64 RankMasks8[8] =/*from rank8 to rank1 ?*/
    {
        0xFFL, 0xFF00L, 0xFF0000L, 0xFF000000L, 0xFF00000000L, 0xFF0000000000L, 0xFF000000000000L, 0xFF00000000000000L
    };
const U64 FileMasks8[8] =/*from fileA to FileH*/
{
    0x101010101010101L, 0x202020202020202L, 0x404040404040404L, 0x808080808080808L,
    0x1010101010101010L, 0x2020202020202020L, 0x4040404040404040L, 0x8080808080808080L
};

const U64 DiagonalMasks8[15] =/*from top left to bottom right*/
{
0x1L, 0x102L, 0x10204L, 0x1020408L, 0x102040810L, 0x10204081020L, 0x1020408102040L,
0x102040810204080L, 0x204081020408000L, 0x408102040800000L, 0x810204080000000L,
0x1020408000000000L, 0x2040800000000000L, 0x4080000000000000L, 0x8000000000000000L
};

const U64 AntiDiagonalMasks8[15] =/*from top right to bottom left*/
{
0x80L, 0x8040L, 0x804020L, 0x80402010L, 0x8040201008L, 0x804020100804L, 0x80402010080402L,
0x8040201008040201L, 0x4020100804020100L, 0x2010080402010000L, 0x1008040201000000L,
0x804020100000000L, 0x402010000000000L, 0x201000000000000L, 0x100000000000000L
};

MoveGen::MoveGen()
{

}

void MoveGen::generatePsMoves(bool isWhite)
{
    moveCount = 0;
    U64 friends, enemys, pawns, knights, rooks, bishops, queens, king, eking;

    if(isWhite){
        friends = BBWhitePieces;
        enemys = BBBlackPieces;
        pawns = BBWhitePawns;
        knights = BBWhiteKnights;
        bishops = BBWhiteBishops;
        rooks = BBWhiteRooks;
        queens = BBWhiteQueens;
        king = BBWhiteKing;
        eking = BBBlackKing;
        //generate pawn moves
        possibleWP(BBWhitePawns, eking);

    } else {
        friends = BBBlackPieces;
        enemys = BBWhitePieces;
        pawns = BBBlackPawns;
        knights = BBBlackKnights;
        bishops = BBBlackBishops;
        rooks = BBBlackRooks;
        queens = BBBlackQueens;
        king = BBBlackKing;
        eking = BBWhiteKing;
        possibleBP(BBBlackPawns, eking);

    }

    U64 piece = 0LL;
    for(int i = 0; i < 64; i++){
        piece = 0LL;
        piece += 1LL << i;
        if(knights & piece) possibleN(isWhite, i, friends, enemys, eking);
        if(bishops & piece) possibleB(isWhite, i, friends, enemys, eking);
        if(rooks & piece) possibleR(isWhite, i, friends, enemys, eking);
        if(queens & piece) possibleQ(isWhite, i, friends, enemys, eking);
    }

}

//pawn moves
void MoveGen::possibleWP(U64 wpawns, U64 blackking)
{
    char piece = 'p';
    char captured;
    int x, y, x1, y1;
    bool isWhite = true;

    //forward one
    U64 PAWN_MOVES = northOne(wpawns) & EmptyTiles;
    U64 i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8;
        y = index/8+1;
        x1 = index%8;
        y1 = index/8;

        movegen_push(x, y, x1, y1, piece, '0', moveCount, '0');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


    //forward two
    PAWN_MOVES = (wpawns>>16) & EmptyTiles &(EmptyTiles>>8) &rank4;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8;
        y = index/8+2;
        x1 = index%8;
        y1 = index/8;

        movegen_push(x, y, x1, y1, piece, '0', moveCount, '0');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


    //capture right
    PAWN_MOVES = noEaOne(wpawns) & BBBlackPieces & ~blackking;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8-1;
        y = index/8+1;
        x1 = index%8;
        y1 = index/8;

        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(isWhite, landing);

        movegen_push(x, y, x1, y1, piece, captured, moveCount, '0');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


    //capture left
    PAWN_MOVES = noWeOne(wpawns) & BBBlackPieces & ~blackking;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8+1;
        y = index/8+1;
        x1 = index%8;
        y1 = index/8;

        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(isWhite, landing);

        movegen_push(x, y, x1, y1, piece, captured, moveCount, '0');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


//Pawn promotions moving forward one
    PAWN_MOVES = northOne(wpawns) & EmptyTiles & rank8;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8;
        y = index/8+1;
        x1 = x;
        y1 = 7;

        movegen_push(x, y, x1, y1, piece, '0', moveCount, 'Q');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }

//pawn capture promotions
    //capture right
    PAWN_MOVES = noEaOne(wpawns) & BBBlackPieces & ~blackking & rank8;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8-1;
        y = 7;
        x1 =index%8;
        y1 = 7;

        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(isWhite, landing);

        movegen_push(x, y, x1, y1, piece, captured, moveCount, 'Q');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


    //capture left
    PAWN_MOVES = noWeOne(wpawns) & BBBlackPieces & ~blackking & rank8;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8+1;
        y = 7;
        x1 =index%8;
        y1 = 7;

        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(isWhite, landing);

        movegen_push(x, y, x1, y1, piece, captured, moveCount, 'Q');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }

}

void MoveGen::possibleBP(U64 bpawns, U64 whiteking)
{
    char piece = 'p';
    char captured;
    int x, y, x1, y1;
    bool isWhite = false;

    //forward one
    U64 PAWN_MOVES = southOne(bpawns) & EmptyTiles;
    U64 i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8;
        y = index/8-1;
        x1 = index%8;
        y1 = index/8;

        movegen_push(x, y, x1, y1, piece, '0', moveCount, '0');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);

    }

    //forward two
    PAWN_MOVES = (bpawns<<16) & EmptyTiles &(EmptyTiles<<8) & rank5;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8;
        y = index/8-2;
        x1 = index%8;
        y1 = index/8;

        movegen_push(x, y, x1, y1, piece, '0', moveCount, '0');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);

    }

    //capture right
    PAWN_MOVES = soEaOne(bpawns) & BBWhitePieces & ~whiteking;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(isWhite, landing);

        x = index%8-1;
        y = index/8-1;
        x1 = index%8;
        y1 = index/8;

        movegen_push(x, y, x1, y1, piece, captured, moveCount, '0');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


    //capture left
    PAWN_MOVES = soWeOne(bpawns) & BBWhitePieces & ~whiteking;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(isWhite, landing);

        x = index%8+1;
        y = index/8-1;
        x1 = index%8;
        y1 = index/8;

        movegen_push(x, y, x1, y1, piece, captured, moveCount, '0');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


//promotions
    PAWN_MOVES = southOne(bpawns) & EmptyTiles & rank1;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8;
        y = 0;
        x1 = x;
        y1 = 0;

        movegen_push(x, y, x1, y1, piece, '0', moveCount, 'Q');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }

    //capture promotions
    //capture right
    PAWN_MOVES = soEaOne(bpawns) & BBWhitePieces & ~whiteking & rank1;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(isWhite, landing);

        x = index%8-1;
        y = 0;
        x1 = index%8;
        y1 = 0;

        movegen_push(x, y, x1, y1, piece, captured, moveCount, 'Q');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


    //capture left
    PAWN_MOVES = soWeOne(bpawns) & BBWhitePieces & ~whiteking & rank1;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(isWhite, landing);


        x = index%8+1;
        y = 0;
        x1 =index%8;
        y1 = 0;

        movegen_push(x, y, x1, y1, piece, captured, moveCount, 'Q');
        moveCount ++;

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }

}

//other piece moves
void MoveGen::possibleN(bool isWhite, int location, U64 friends, U64 enemys, U64 oppositeking)
{
    char piece;
    if(isWhite) piece = 'N';
    else piece = 'n';
    U64 moves;

    //use the knight span board which holds possible knight moves
    //and apply a shift to the knights current pos
    if(location > 18){
        moves = KNIGHT_SPAN<<(location-18);
    } else {
        moves = KNIGHT_SPAN>>(18-location);
    }

    //making sure the moves don't wrap around to other side once shifter
    //as well as friendly and illegal king capture check
    if(location % 8 < 4){
        moves &= ~FILE_GH & ~friends & ~oppositeking;
    } else {
        moves &= ~FILE_AB & ~friends & ~oppositeking;
    }

    U64 j = moves & ~(moves-1);
    char captured;
    int x, y, x1, y1;
    while(j != 0){
        //store moves
        int index = trailingZeros(j);
        captured = '0';
        U64 landing = 0LL;
        landing += 1LL << index;
        if(landing & enemys){
            captured = whichPieceCaptured(isWhite, landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, moveCount, '0');
        moveCount ++;

        moves &= ~j;
        j = moves & ~(moves-1);
    }
}

void MoveGen::possibleB(bool isWhite, int location, U64 friends, U64 enemys, U64 oppositeking)
{
    char piece;
    if(isWhite) piece = 'B';
    else piece = 'b';

    U64 moves =  DAndAntiDMoves(location) & ~friends & ~oppositeking;
    U64 j = moves & ~ (moves-1);

    char captured;
    int x, y, x1, y1;
    while(j != 0){
        int index = trailingZeros(j);
        captured = '0';
        U64 landing = 0LL;
        landing += 1LL << index;
        if(landing & enemys){
            captured = whichPieceCaptured(isWhite, landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, moveCount, '0');
        moveCount ++;

        moves &= ~j;
        j = moves & ~(moves-1);
    }
}

void MoveGen::possibleR(bool isWhite, int location, U64 friends, U64 enemys, U64 oppositeking)
{
    char piece;
    if(isWhite) piece = 'R';
    else piece = 'r';

    U64 moves = horizVert(location) & ~friends & ~oppositeking;
    U64 j = moves & ~ (moves-1);

    char captured;
    int x, y, x1, y1;
    while(j != 0){
        int index = trailingZeros(j);
        captured = '0';
        U64 landing = 0LL;
        landing += 1LL << index;
        if(landing & enemys){
            captured = whichPieceCaptured(isWhite, landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, moveCount, '0');
        moveCount ++;

        moves &= ~j;
        j = moves & ~(moves-1);
    }

}

void MoveGen::possibleQ(bool isWhite, int location, U64 friends, U64 enemys, U64 oppositeking)
{
    char piece;
    if(isWhite) piece = 'Q';
    else piece = 'q';

    U64 moves = (DAndAntiDMoves(location) | horizVert(location)) & ~friends & ~oppositeking;
    U64 j = moves & ~ (moves-1);

    char captured;
    int x, y, x1, y1;
    while(j != 0){
        int index = trailingZeros(j);

        captured = '0';
        U64 landing = 0LL;
        landing += 1LL << index;
        if(landing & enemys){
            captured = whichPieceCaptured(isWhite, landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, moveCount, '0');
        moveCount ++;

        moves &= ~j;
        j = moves & ~(moves-1);
    }

}

void MoveGen::possibleK(bool isWhite, int location, U64 friends, U64 enemys)
{
    U64 moves;
    char piece;
    if(isWhite) piece = 'K';
    else piece = 'k';


    if(location > 9){
        moves = KING_SPAN << (location-9);

    } else {
        moves = KING_SPAN >> (9-location);
    }

    if(location % 8 < 4){
        moves &= ~FILE_GH & ~friends;

    } else {
        moves &= ~FILE_AB & ~friends;
    }

    U64 j = moves &~(moves-1);

    char captured;
    int x, y, x1, y1;
    while(j != 0){
        int index = trailingZeros(j);
        captured = '0';
        U64 landing = 0LL;
        landing += 1LL << index;
        if(landing & enemys){
            captured = whichPieceCaptured(isWhite, landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, moveCount, '0');
        moveCount ++;

        moves &= ~j;
        j = moves &~ (moves-1);
    }



}

void MoveGen::movegen_push(int x, int y, int x1, int y1, char piece, char captured, int moveCount, char flag)
{
    moveAr[moveCount]->x = x;
    moveAr[moveCount]->y = y;
    moveAr[moveCount]->x1 = x1;
    moveAr[moveCount]->y1 = y1;
    moveAr[moveCount]->piece = piece;
    moveAr[moveCount]->captured = captured;


    //scoring capture moves
    if(captured != '0'){
        //int pieceValues[5] = {100, 320, 330, 500, 900};
        //int capturedValues[5] = {100, 320, 330, 500, 900};
        int pVal, cVal;
        if(piece == 'P' || piece == 'p') pVal = 100;
        else if(piece == 'N' || piece == 'n') pVal = 320;
        else if(piece == 'B' || piece == 'b') pVal = 330;
        else if(piece == 'R' || piece == 'r') pVal = 500;
        else if(piece == 'Q' || piece == 'q') pVal = 900;

        if(piece == 'P' || piece == 'p') cVal = 100;
        else if(piece == 'N' || piece == 'n') cVal = 320;
        else if(piece == 'B' || piece == 'b') cVal = 330;
        else if(piece == 'R' || piece == 'r') cVal = 500;
        else if(piece == 'Q' || piece == 'q') cVal = 900;

        moveAr[moveCount]->score = cVal - pVal + 50;

    }


}

char MoveGen::whichPieceCaptured(bool isWhite, U64 landing)
{
    if(isWhite){
        if(landing & BBBlackPawns) return 'p';
        if(landing & BBBlackKnights) return 'n';
        if(landing & BBBlackBishops) return 'b';
        if(landing & BBBlackQueens) return 'q';
        if(landing & BBBlackKing) return 'k';
    } else {
        if(landing & BBWhitePawns) return 'P';
        if(landing & BBWhiteKnights) return 'N';
        if(landing & BBWhiteBishops) return 'B';
        if(landing & BBWhiteQueens) return 'Q';
        if(landing & BBWhiteKing) return 'K';
    }
    std::cout << "which piece captured error" << std::endl;
    return '0';
}

//implement into other MOVE GEN ASIDE FROM KINGS, MUCH FASTER THAN for 64 loop
int MoveGen::trailingZeros(U64 i)
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

U64 MoveGen::horizVert(int s)
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

U64 MoveGen::DAndAntiDMoves(int s)
{
    U64 binaryS = 1LL <<s;

    U64 possibilitiesDiagonal = ((FullTiles & DiagonalMasks8[(s / 8) + (s % 8)]) - (2 * binaryS)) ^ ReverseBits(ReverseBits(FullTiles & DiagonalMasks8[(s / 8) + (s % 8)]) - (2 * ReverseBits(binaryS)));

    U64 possibilitiesAntiDiagonal = ((FullTiles & AntiDiagonalMasks8[(s / 8) + 7 - (s % 8)]) - (2 * binaryS)) ^ ReverseBits(ReverseBits(FullTiles & AntiDiagonalMasks8[(s / 8) + 7 - (s % 8)]) - (2 * ReverseBits(binaryS)));

    return (possibilitiesDiagonal & DiagonalMasks8[(s / 8) + (s % 8)]) | (possibilitiesAntiDiagonal & AntiDiagonalMasks8[(s / 8) + 7 - (s % 8)]);
}

U64 MoveGen::ReverseBits(U64 input)
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

void MoveGen::constructBoards()
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

}

U64 MoveGen::northOne(U64 b)
{
    return b >> 8;
}

U64 MoveGen::southOne(U64 b)
{
    return b << 8;
}

U64 MoveGen::eastOne (U64 b)
{
    return (b << 1) & notHFile;
}

U64 MoveGen::noEaOne(U64 b)
{
    return (b >> 7) & notHFile;;
}

U64 MoveGen::soEaOne(U64 b)
{
    return (b << 9) & notHFile;
}

U64 MoveGen::westOne(U64 b)
{
    return (b >> 1) & notAFile;
}

U64 MoveGen::soWeOne(U64 b)
{

    return (b << 7) & notAFile;
}

U64 MoveGen::noWeOne(U64 b)
{
    return (b >> 9) & notAFile;
}


//normal move stuff
std::string MoveGen::makeMove(Move *move, ZobristH *zobrist)
{
    std::string savedMove;
    bool wOrB;
    int x, y, x1, y1, xyI, xyE;
    //inital spot piece mask and end spot mask
    U64 pieceMaskI = 0LL, pieceMaskE = 0LL;
    //for normal moves

    xyI = move->y * 8 + move->x;
    xyE = move->y1* 8 + move->x1;

    pieceMaskI += 1LL<< xyI;
    pieceMaskE += 1LL << xyE;
    if(BBWhitePieces & pieceMaskI) wOrB = true;
    else wOrB = false;

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
            savedMove += move->captured;

            if(move->flag != 'Q'){
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
            savedMove += move->captured;
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
            savedMove += move->captured;
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
            savedMove += move->captured;
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
            savedMove += move->captured;
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
            savedMove += move->captured;
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
            savedMove += move->captured;

            if(move->flag != 'Q'){
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
            savedMove += move->captured;
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
            savedMove += move->captured;
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
            savedMove += move->captured;
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
            savedMove += move->captured;
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
            savedMove += move->captured;
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
    zobrist->UpdateKey(xyI, xyE, savedMove);

    //change zobrist color after a move
    zobrist->UpdateColor();

    return savedMove;


}

void MoveGen::unmakeMove(std::string moveKey, ZobristH *zobrist)
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
    zobrist->UpdateKey(xyI, xyE, moveKey);

    //change cobrist color after a move
    zobrist->UpdateColor();

    bool t;
    if(wOrB == 'w'){
        t = true;
    } else {
        t = false;
    }

}

void MoveGen::undoCapture(U64 location, char piece, char whiteOrBlack)
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

