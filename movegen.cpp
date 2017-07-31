#include "movegen.h"
#include "Pieces.h"
#include "hashentry.h"
#include "bitboards.h"


static const U64 RankMasks8[8] =/*from rank8 to rank1 ?*/
{
    0xFFL, 0xFF00L, 0xFF0000L, 0xFF000000L, 0xFF00000000L, 0xFF0000000000L, 0xFF000000000000L, 0xFF00000000000000L
};
static const U64 FileMasks8[8] =/*from fileA to FileH*/
{
    0x101010101010101L, 0x202020202020202L, 0x404040404040404L, 0x808080808080808L,
    0x1010101010101010L, 0x2020202020202020L, 0x4040404040404040L, 0x8080808080808080L
};

static const U64 DiagonalMasks8[15] =/*from top left to bottom right*/
{
0x1L, 0x102L, 0x10204L, 0x1020408L, 0x102040810L, 0x10204081020L, 0x1020408102040L,
0x102040810204080L, 0x204081020408000L, 0x408102040800000L, 0x810204080000000L,
0x1020408000000000L, 0x2040800000000000L, 0x4080000000000000L, 0x8000000000000000L
};

static const U64 AntiDiagonalMasks8[15] =/*from top right to bottom left*/
{
0x80L, 0x8040L, 0x804020L, 0x80402010L, 0x8040201008L, 0x804020100804L, 0x80402010080402L,
0x8040201008040201L, 0x4020100804020100L, 0x2010080402010000L, 0x1008040201000000L,
0x804020100000000L, 0x402010000000000L, 0x201000000000000L, 0x100000000000000L
};

/*************************************************
* Values used for sorting captures are the same  *
* as normal piece values, except for a king.     *
*************************************************/

static const int SORT_VALUE[7] = {0, 100, 325, 335, 500, 975, 0};


MoveGen::MoveGen()
{

}

void MoveGen::generatePsMoves(bool capturesOnly)
{
    //counts total moves generated
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
        possibleWP(pawns, eking, capturesOnly);

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

        possibleBP(pawns, eking, capturesOnly);
    }

    //if we don't want to only generate captures
    U64 capsOnly = full;
    //if we only want to generate captures
    if(capturesOnly) capsOnly = enemys;

    //loop through board an generate ps legal moves for our side
    U64 piece = 0LL;
    for(int i = 0; i < 64; i++){
        piece = 0LL;
        piece += 1LL << i;
        if(knights & piece) possibleN(i, friends, enemys, eking, capsOnly);
        else if(bishops & piece) possibleB(i, friends, enemys, eking, capsOnly);
        else if(rooks & piece) possibleR(i, friends, enemys, eking, capsOnly);
        else if(queens & piece) possibleQ(i, friends, enemys, eking, capsOnly);
        else if(king & piece) possibleK(i, friends, enemys, capsOnly);
    }
    return;
}

//pawn moves
void MoveGen::possibleWP(U64 wpawns, U64 blackking, bool capturesOnly)
{
    char piece = 'P';
    char captured;
    int x, y, x1, y1;
    bool isWhite = true;

    //forward one
    U64 PAWN_MOVES = northOne(wpawns) & EmptyTiles;
    U64 i = PAWN_MOVES &~ (PAWN_MOVES-1);

    if(!capturesOnly){
        while(i != 0){
            int index = trailingZeros(i);
            x = index%8;
            y = index/8+1;
            x1 = index%8;
            y1 = index/8;

            movegen_push(x, y, x1, y1, piece, '0', '0');

            PAWN_MOVES &= ~i;
            i= PAWN_MOVES & ~(PAWN_MOVES-1);
        }


        //forward two
        PAWN_MOVES = (wpawns>>16) & EmptyTiles &(EmptyTiles>>8) & rank4;
        i = PAWN_MOVES &~ (PAWN_MOVES-1);

        while(i != 0){
            int index = trailingZeros(i);
            x = index%8;
            y = index/8+2;
            x1 = index%8;
            y1 = index/8;

            movegen_push(x, y, x1, y1, piece, '0', '0');

            PAWN_MOVES &= ~i;
            i= PAWN_MOVES & ~(PAWN_MOVES-1);
        }        
    }

    //capture right
    PAWN_MOVES = noEaOne(wpawns) & BBBlackPieces & ~blackking & ~rank8;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8-1;
        y = index/8+1;
        x1 = index%8;
        y1 = index/8;

        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(landing);

        movegen_push(x, y, x1, y1, piece, captured, '0');

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


    //capture left
    PAWN_MOVES = noWeOne(wpawns) & BBBlackPieces & ~blackking & ~rank8;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8+1;
        y = index/8+1;
        x1 = index%8;
        y1 = index/8;

        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(landing);

        movegen_push(x, y, x1, y1, piece, captured, '0');

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }

//Pawn promotions
    //moving forward one
    PAWN_MOVES = northOne(wpawns) & EmptyTiles & rank8;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8;
        y = 1;
        x1 = x;
        y1 = 0;

        movegen_push(x, y, x1, y1, piece, '0', 'Q');

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
        y = 1;
        x1 =index%8;
        y1 = 0;

        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(landing);

        movegen_push(x, y, x1, y1, piece, captured, 'Q');

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


    //capture left
    PAWN_MOVES = noWeOne(wpawns) & BBBlackPieces & ~blackking & rank8;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index%8+1;
        y = 1;
        x1 =index%8;
        y1 = 0;

        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(landing);

        movegen_push(x, y, x1, y1, piece, captured, 'Q');

        PAWN_MOVES &= ~i;
        i = PAWN_MOVES & ~(PAWN_MOVES-1);
    }

}

void MoveGen::possibleBP(U64 bpawns, U64 whiteking, bool capturesOnly)
{
    char piece = 'p';
    char captured;
    int x, y, x1, y1;
    bool isWhite = false;
    U64 PAWN_MOVES, i;


    if(!capturesOnly){
        //forward one
        PAWN_MOVES = southOne(bpawns) & EmptyTiles;
        i = PAWN_MOVES &~ (PAWN_MOVES-1);

        while(i != 0){
            int index = trailingZeros(i);
            x = index%8;
            y = index/8-1;
            x1 = index%8;
            y1 = index/8;

            movegen_push(x, y, x1, y1, piece, '0', '0');

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

            movegen_push(x, y, x1, y1, piece, '0', '0');

            PAWN_MOVES &= ~i;
            i= PAWN_MOVES & ~(PAWN_MOVES-1);

        }
    }

    //capture right
    PAWN_MOVES = soEaOne(bpawns) & BBWhitePieces & ~whiteking & ~rank1;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(landing);

        x = index%8-1;
        y = index/8-1;
        x1 = index%8;
        y1 = index/8;

        movegen_push(x, y, x1, y1, piece, captured, '0');

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }


    //capture left
    PAWN_MOVES = soWeOne(bpawns) & BBWhitePieces & ~whiteking & ~rank1;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        U64 landing = 0LL;
        landing += 1LL << index;
        captured = whichPieceCaptured(landing);

        x = index%8+1;
        y = index/8-1;
        x1 = index%8;
        y1 = index/8;

        movegen_push(x, y, x1, y1, piece, captured, '0');

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }

//promotions
    //forward promotions
    PAWN_MOVES = southOne(bpawns) & EmptyTiles & rank1;
    i = PAWN_MOVES &~ (PAWN_MOVES-1);

    while(i != 0){
        int index = trailingZeros(i);
        x = index % 8;
        y = 6;
        x1 = x;
        y1 = 7;

        movegen_push(x, y, x1, y1, piece, '0', 'Q');

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
        captured = whichPieceCaptured(landing);

        x = index%8-1;
        y = 6;
        x1 = index%8;
        y1 = 7;

        movegen_push(x, y, x1, y1, piece, captured, 'Q');

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
        captured = whichPieceCaptured(landing);


        x = index%8+1;
        y = 6;
        x1 =index%8;
        y1 = 7;

        movegen_push(x, y, x1, y1, piece, captured, 'Q');

        PAWN_MOVES &= ~i;
        i= PAWN_MOVES & ~(PAWN_MOVES-1);
    }

}

//other piece moves
void MoveGen::possibleN(int location, U64 friends, U64 enemys, U64 oppositeking, U64 capturesOnly)
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
        moves &= ~FILE_GH & ~friends & ~oppositeking & capturesOnly;
    } else {
        moves &= ~FILE_AB & ~friends & ~oppositeking & capturesOnly;
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
            captured = whichPieceCaptured(landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, '0');

        moves &= ~j;
        j = moves & ~(moves-1);
    }
}

void MoveGen::possibleB(int location, U64 friends, U64 enemys, U64 oppositeking, U64 capturesOnly)
{
    char piece;
    if(isWhite) piece = 'B';
    else piece = 'b';

    U64 moves = slider_attacks.BishopAttacks(FullTiles, location);
    moves &= ~friends & capturesOnly & ~oppositeking;

    //U64 moves =  DAndAntiDMoves(location) & ~friends & ~oppositeking & capturesOnly;
    U64 j = moves & ~ (moves-1);

    char captured;
    int x, y, x1, y1;
    while(j != 0){
        int index = trailingZeros(j);
        captured = '0';
        U64 landing = 0LL;
        landing += 1LL << index;
        if(landing & enemys){
            captured = whichPieceCaptured(landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, '0');

        moves &= ~j;
        j = moves & ~(moves-1);
    }
}

void MoveGen::possibleR(int location, U64 friends, U64 enemys, U64 oppositeking, U64 capturesOnly)
{
    char piece;
    if(isWhite) piece = 'R';
    else piece = 'r';

    drawBBA();
    drawBB(BBBlackRooks);
    drawBB(FullTiles);
    U64 moves = slider_attacks.RookAttacks(FullTiles, location);
    moves &= ~friends & capturesOnly & ~oppositeking;
    //U64 moves = horizVert(location);
    //moves &= ~friends & capturesOnly &~oppositeking;
    U64 j = moves & ~ (moves-1);

    char captured;
    int x, y, x1, y1;
    while(j != 0){
        int index = trailingZeros(j);
        captured = '0';
        U64 landing = 0LL;
        landing += 1LL << index;
        if(landing & enemys){
            captured = whichPieceCaptured(landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, '0');

        moves &= ~j;
        j = moves & ~(moves-1);
    }

}

void MoveGen::possibleQ(int location, U64 friends, U64 enemys, U64 oppositeking, U64 capturesOnly)
{
    char piece;
    if(isWhite) piece = 'Q';
    else piece = 'q';

    const U64 f = FullTiles;

    //grab moves from magic bitboards
    U64 moves = slider_attacks.QueenAttacks(FullTiles, location);
    //and against friends and a full board if normal move gen, or enemy board if captures only
    moves &= ~friends & capturesOnly & ~oppositeking;
    //U64 moves = horizVert(location) | DAndAntiDMoves(location);
    //moves &= ~friends & capturesOnly & ~oppositeking;

    U64 j = moves & ~ (moves-1);

    char captured;
    int x, y, x1, y1;
    while(j != 0){
        int index = trailingZeros(j);

        captured = '0';
        U64 landing = 0LL;
        landing += 1LL << index;
        if(landing & enemys){
            captured = whichPieceCaptured(landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, '0');

        moves &= ~j;
        j = moves & ~(moves-1);
    }

}

void MoveGen::possibleK(int location, U64 friends, U64 enemys, U64 capturesOnly)
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
        moves &= ~FILE_GH & ~friends & capturesOnly;

    } else {
        moves &= ~FILE_AB & ~friends & capturesOnly;
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
            captured = whichPieceCaptured(landing);
        }

        x = location % 8;
        y = location / 8;
        x1 = index % 8;
        y1 = index / 8;

        movegen_push(x, y, x1, y1, piece, captured, '0');

        moves &= ~j;
        j = moves &~ (moves-1);
    }

}

void MoveGen::movegen_push(int x, int y, int x1, int y1, char piece, char captured, char flag)
{
    //store move data to move object array
    moveAr[moveCount].x = x;
    moveAr[moveCount].y = y;
    moveAr[moveCount].x1 = x1;
    moveAr[moveCount].y1 = y1;
    moveAr[moveCount].piece = piece;
    moveAr[moveCount].captured = captured;
    moveAr[moveCount].flag = flag;
    moveAr[moveCount].score = 0;

    int sPos = y * 8 + x, ePos = y1 * 8 + x;

    //history heristics for quiet moves, color, from pos, to pos
    moveAr[moveCount].score = sd.history[isWhite][sPos][ePos];

    //scoring capture moves
    if(captured != '0'){
        //int pieceValues[5] = {100, 320, 330, 500, 900};
        //int capturedValues[5] = {100, 320, 330, 500, 900};
        int pNum, cNum, from;
        if(piece == 'P' || piece == 'p') {pNum = 1; from = 5;}
        else if(piece == 'N' || piece == 'n') {pNum = 2; from = 4;}
        else if(piece == 'B' || piece == 'b') {pNum = 3; from = 3;}
        else if(piece == 'R' || piece == 'r') {pNum = 4; from = 2;}
        else if(piece == 'Q' || piece == 'q') {pNum = 5; from = 1;}
        else if (piece == 'K' || piece == 'k') {pNum = 6; from = 0;} //different value from actual piece value

        if(captured == 'P' || captured == 'p') cNum = 1;
        else if(captured == 'N' || captured == 'n') cNum = 2;
        else if(captured == 'B' || captured == 'b') cNum = 3;
        else if(captured == 'R' || captured == 'r') cNum = 4;
        else if(captured == 'Q' || captured == 'q') cNum = 5;

        Move t = moveAr[moveCount];

        if(blind(t, SORT_VALUE[pNum], SORT_VALUE[cNum])) moveAr[moveCount].score = SORT_CAPT + SORT_VALUE[cNum] + from;
        else moveAr[moveCount].score = SORT_CAPT + SORT_VALUE[cNum] + from;

    }

    //pawn promotions
    if(moveAr[moveCount].flag == 'Q') moveAr[moveCount].score += 775;

    moveCount ++;
}

bool MoveGen::blind(Move move, int pieceVal, int captureVal)
{
    //approx static eval, Better Lower if not Defended
    char piece = move.piece;
    int slocation = move.y * 8 + move.x;
    int elocation = move.y1 * 8 + move.x1;
    U64 sLoc = 1LL << slocation;
    U64 eLoc = 1LL << elocation;

    //captures from pawns don't lose material
    if(piece == 'P' || piece == 'p') return true;

    //capture lower takes higher
    if(captureVal >= pieceVal - 50) return true;

    //move the piece only on the color BB's and full/emptys
    if(isWhite){
        BBWhitePieces &= ~sLoc;
        BBWhitePieces |= eLoc;
    } else {
        BBBlackPieces &= ~sLoc;
        BBBlackPieces |= eLoc;
    }
    FullTiles &= ~ sLoc;
    FullTiles |= eLoc;
    EmptyTiles = ~FullTiles;

    bool defended = false;

    //in order to test if capture location is attacked
    if(isAttacked(eLoc, isWhite)){
        defended = true;
    }
    //correct BB's
    if(isWhite){
        BBWhitePieces |= sLoc;
        BBWhitePieces &= ~ eLoc;
    } else {
        BBBlackPieces |= sLoc;
        BBBlackPieces &= ~eLoc;
    }
    FullTiles |= sLoc;
    FullTiles &= ~eLoc;
    EmptyTiles = ~FullTiles;

    if(!defended) return true;

    return 0; //of other captures we know not
}

Move MoveGen::movegen_sort(int ply)
{
    int best = -9999999;
    int high;
    //find best scoring move
    for(int i = 0; i < moveCount; i++){
        if(moveAr[i].score > best && !moveAr[i].tried){
            high = i;
        }
    }
    //mark best scoring move tried since we're about to try it
    //~~~ change later if we don't always try move on return
    moveAr[high].tried = true;

    return moveAr[high];
}

void MoveGen::reorderMoves(int ply, HashEntry entry)
{

    for(int i = 0; i < moveCount; i++){
        if(moveAr[i].x == sd.killers[ply][1].x
        && moveAr[i].y == sd.killers[ply][1].y
        && moveAr[i].x1 == sd.killers[ply][1].x1
        && moveAr[i].y1 == sd.killers[ply][1].y1
        && moveAr[i].piece == sd.killers[ply][1].piece
        && moveAr[i].score < SORT_KILL - 1){
            moveAr[i].score = SORT_KILL - 1;
        }
        if(moveAr[i].x == sd.killers[ply][0].x
        && moveAr[i].y == sd.killers[ply][0].y
        && moveAr[i].x1 == sd.killers[ply][0].x1
        && moveAr[i].y1 == sd.killers[ply][0].y1
        && moveAr[i].piece == sd.killers[ply][0].piece
        && moveAr[i].score < SORT_KILL){
            moveAr[i].score = SORT_KILL;
        }       

    }
/*
    if(entry.move.flag != 0x58 && entry.move.flag != 88 && entry.move.flag != 'X'){
        moveAr[moveCount] = entry.move;
        moveAr[moveCount].score = SORT_HASH;
        moveCount ++;
    }
*/
}

char MoveGen::whichPieceCaptured(U64 landing)
{
    if(isWhite){
        if(landing & BBBlackPawns) return 'p';
        if(landing & BBBlackKnights) return 'n';
        if(landing & BBBlackBishops) return 'b';
        if(landing & BBBlackRooks) return 'r';
        if(landing & BBBlackQueens) return 'q';
        if(landing & BBBlackKing) return 'k';
    } else {
        if(landing & BBWhitePawns) return 'P';
        if(landing & BBWhiteKnights) return 'N';
        if(landing & BBWhiteBishops) return 'B';
        if(landing & BBWhiteRooks) return 'R';
        if(landing & BBWhiteQueens) return 'Q';
        if(landing & BBWhiteKing) return 'K';
    }
    drawBBA();
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
    U64 binaryS = 1LL << s;

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
        }
    }

    //mark empty tiles with 1's
    EmptyTiles = ~FullTiles;

}

void MoveGen::grab_boards(BitBoards *BBBoard, bool wOrB)
{
    isWhite = wOrB;
    FullTiles = BBBoard->FullTiles;
    EmptyTiles = BBBoard->EmptyTiles;

    BBWhitePieces = BBBoard->BBWhitePieces;
    BBWhitePawns = BBBoard->BBWhitePawns;
    BBWhiteKnights = BBBoard->BBWhiteKnights;
    BBWhiteBishops = BBBoard->BBWhiteBishops;
    BBWhiteRooks = BBBoard->BBWhiteRooks;
    BBWhiteQueens = BBBoard->BBWhiteQueens;
    BBWhiteKing = BBBoard->BBWhiteKing;

    BBBlackPieces = BBBoard->BBBlackPieces;
    BBBlackPawns = BBBoard->BBBlackPawns;
    BBBlackKnights = BBBoard->BBBlackKnights;
    BBBlackBishops = BBBoard->BBBlackBishops;
    BBBlackRooks = BBBoard->BBBlackRooks;
    BBBlackQueens = BBBoard->BBBlackQueens;
    BBBlackKing = BBBoard->BBBlackKing;
}

bool MoveGen::isAttacked(U64 pieceLoc, bool wOrB)
{
    U64 attacks = 0LL, friends, enemys, pawns, knights, rooks, bishops, queens, king;
    //int x, y, x1, y1;

    if(wOrB){
        friends = BBWhitePieces;
        enemys = BBBlackPieces;
        pawns = BBBlackPawns;
        knights = BBBlackKnights;
        bishops = BBBlackBishops;
        rooks = BBBlackRooks;
        queens = BBBlackQueens;
        king = BBBlackKing;

        //pawns
        //capture right
        attacks = soEaOne(pawns);
        //capture left
        attacks |= soWeOne(pawns);

    } else {
        friends = BBBlackPieces;
        enemys = BBWhitePieces;
        pawns = BBWhitePawns;
        knights = BBWhiteKnights;
        bishops = BBWhiteBishops;
        rooks = BBWhiteRooks;
        queens = BBWhiteQueens;
        king = BBWhiteKing;

        //capture right
        attacks = noEaOne(pawns);
        //capture left
        attacks |= noWeOne(pawns);
    }

    if(attacks & pieceLoc) return true;

    //int is piece/square attacked location on board
    int location = trailingZeros(pieceLoc);

//very similar to move generation code just ending with generated bitboards of attacks
    //knight attacks
    if(location > 18){
        attacks = KNIGHT_SPAN<<(location-18);
    } else {
        attacks = KNIGHT_SPAN>>(18-location);
    }

    if(location % 8 < 4){
        attacks &= ~FILE_GH & ~friends;
    } else {
        attacks &= ~FILE_AB & ~friends;
    }

    if(attacks & knights) return true;

    //diagonal of bishops and queens attack check
    U64 BQ = bishops | queens;

    attacks = DAndAntiDMoves(location) & BQ;

    if(attacks & BQ) return true;

    //horizontal of rooks and queens attack check
    U64 BR = bishops | queens;

    attacks = horizVert(location) & BR;

    if(attacks & BR) return true;

    //king attack checks
    attacks = northOne(pieceLoc);
    attacks |= noEaOne(pieceLoc);
    attacks |= eastOne(pieceLoc);
    attacks |= soEaOne(pieceLoc);
    attacks |= southOne(pieceLoc);
    attacks |= soWeOne(pieceLoc);
    attacks |= westOne(pieceLoc);
    attacks |= noWeOne(pieceLoc);

    if(attacks & king) return true;

return false;
}

void MoveGen::drawBBA()
{
    char flips[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};
    char flipsL[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    int c = 0;

    for(int i = 0; i < 64; i++){
        if((i)%8 == 0){
            std::cout<< std::endl;
            std::cout << flips[c] << " | ";
            c++;
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

        //if(i % 8 == 7){
        //    std::cout << "| " << flips[c] ;
       //     c++;
       // }
    }

    std::cout << std::endl << "    ";
    for(int i = 0; i < 8; i++){
        std::cout << flipsL[i] << "  ";
    }

    std::cout << std::endl << std::endl;;
}

void MoveGen::drawBB(U64 board)
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
