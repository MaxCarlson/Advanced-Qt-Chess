#include "evaluatebb.h"
#include "externs.h"
#include "movegen.h"
#include "bitboards.h"
#include "hashentry.h"
#include "zobristh.h"


const U64 RankMasks8[8] =/*from rank8 to rank1 ?*/
{
    0xFFL, 0xFF00L, 0xFF0000L, 0xFF000000L, 0xFF00000000L, 0xFF0000000000L, 0xFF000000000000L, 0xFF00000000000000L
};
const U64 FileMasks8[8] =/*from fileA to FileH*/
{
    0x101010101010101L, 0x202020202020202L, 0x404040404040404L, 0x808080808080808L,
    0x1010101010101010L, 0x2020202020202020L, 0x4040404040404040L, 0x8080808080808080L
};


const U64 full  = 0xffffffffffffffffULL;

//adjustments of piece value based on our color pawn count
const int knight_adj[9] = { -20, -16, -12, -8, -4,  0,  4,  8, 10};
const int rook_adj[9] =   {  15,  12,   9,  6,  3,  0, -3, -6, -9};

//values definitely need to be adjusted and file/rank/side variable
const int rookOpenFile = 10;
const int rookHalfOpenFile = 5;

//posative value
const int BISHOP_PAIR = 30;
//used as negatives to incourage bishop pair
const int KNIGHT_PAIR = 8;
const int ROOK_PAIR = 16;

static const int SafetyTable[100] = {
    0,  0,   1,   2,   3,   5,   7,   9,  12,  15,
  18,  22,  26,  30,  35,  39,  44,  50,  56,  62,
  68,  75,  82,  85,  89,  97, 105, 113, 122, 131,
 140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
 260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
 377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
 494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500
};


/*
piece values
  P = 100
  N = 320
  B = 330
  R = 500
  Q = 900
  K = 9000
*/

evaluateBB::evaluateBB()
{

}

int evaluateBB::evalBoard(bool isWhite, const BitBoards& BBBoard, ZobristH zobristE)
{
    //transposition hash quiet
    int hash = (int)(zobristE.zobristKey % 5021983);
    HashEntry entry = transpositionEval[hash];
    evalMoveGen.grab_boards(BBBoard, isWhite);
/*
    //if we get a hash-table hit, return the evaluation
    if(entry.zobrist == zobristE.zobristKey){
        if(isWhite){

            if(!entry.flag) return entry.eval;
            else return -entry.eval;
        } else {

            if(!entry.flag) return - entry.eval;
            else return entry.eval;
        }

    }
*/
//reset values
    int totalEvaualtion = 0, midGScore = 0, endGScore = 0;
    gamePhase = 0;
    attCount[0] = 0; attCount[1] = 0;
    attWeight[0] = 0; attWeight[1] = 0;
    pawnCount[0] = 0; pawnCount[1] = 0;
    knightCount[0] = 0; knightCount[1] = 0;
    bishopCount[0] = 0; bishopCount [1] = 0;
    rookCount[0] = 0; rookCount[1] = 0;
    midGMobility[0] = 0; midGMobility[1] = 0;
    endGMobility[0] = 0; endGMobility[1] = 0;



//generate zones around kings
    generateKingZones(true);
    generateKingZones(false);


//loop through all pieces and gather numbers, mobility, king attacks, etc
    for(int i = 0; i < 64; i++){
        totalEvaualtion += getPieceValue(i);
    }
    totalEvaualtion += getPawnScore();


//adjusting meterial value of pieces bonus for bishop, small penalty for others
    if(bishopCount[0] > 1) totalEvaualtion += BISHOP_PAIR;
    if(bishopCount[1] > 1) totalEvaualtion -= BISHOP_PAIR;
    if(knightCount[0] > 1) totalEvaualtion -= KNIGHT_PAIR;
    if(knightCount[1] > 1) totalEvaualtion += KNIGHT_PAIR;
    if(rookCount[0] > 1) totalEvaualtion -= KNIGHT_PAIR;
    if(rookCount[1] > 1) totalEvaualtion += KNIGHT_PAIR;


    totalEvaualtion += knight_adj[pawnCount[0]];
    totalEvaualtion += rook_adj[pawnCount[0]];
    totalEvaualtion -= knight_adj[pawnCount[1]];
    totalEvaualtion -= rook_adj[pawnCount[1]];

    /********************************************************************
     *  Merge king attack score. We don't apply this value if there are *
     *  less than two attackers or if the attacker has no queen.        *
     *******************************************************************/

    if(attCount[0] < 2 || BBBoard.BBWhiteQueens == 0LL) attWeight[0] = 0;
    if(attCount[1] < 2 || BBBoard.BBBlackQueens == 0LL) attWeight[1] = 0;
    totalEvaualtion += SafetyTable[attWeight[0]];
    totalEvaualtion -= SafetyTable[attWeight[1]];


    /********************************************************************
    *  Merge midgame and endgame score. We interpolate between these    *
    *  two values, using a gamePhase value, based on remaining piece    *
    *  material on both sides. With less pieces, endgame score beco-    *
    *  mes more influential.                                            *
    ********************************************************************/

    midGScore +=(midGMobility[0] - midGMobility[1]);
    endGScore +=(endGMobility[0] - endGMobility[1]);
    if (gamePhase > 24) gamePhase = 24;
    int mgWeight = gamePhase;
    int egWeight = 24 - gamePhase;

    totalEvaualtion +=( (midGScore * mgWeight) + (endGScore * egWeight)) / 24;


    //switch for color
    if(!isWhite) totalEvaualtion = -totalEvaualtion;

    //save to TT eval table
    saveTT(isWhite, totalEvaualtion, hash, zobristE);

    return totalEvaualtion;
}

void evaluateBB::saveTT(bool isWhite, int totalEvaualtion, int hash, const ZobristH &zobristE)
{
    //store eval into eval hash table
    transpositionEval[hash].eval = totalEvaualtion;
    transpositionEval[hash].zobrist = zobristE.zobristKey;

    if(isWhite) transpositionEval[hash].flag = 0;
    else transpositionEval[hash].flag = 1;
}

int evaluateBB::returnMateScore(bool isWhite, int depth)
{
    U64 king;
    if(isWhite) king = evalMoveGen.BBWhiteKing;
    else king = evalMoveGen.BBBlackKing;

    //if we have no moves and we're in checkmate
    if(evalMoveGen.isAttacked(king, isWhite)){
        if(isWhite){
            return 32000 + depth; //increase mate score the faster we find it
        } else {
            return -32000 - depth;
        }
    }
    //if it's a stalemate
    if(isWhite){
        return 25000 + depth;
    } else {
        return -25000 - depth;
    }
}

//white piece square lookup tables
int wPawnsSqT[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

int wKnightsSqT[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

int wBishopsSqT[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

int wRooksSqT[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

int wQueensSqt[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

int wKingMidSqT[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

int wKingEndSqT[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

//black piece square lookup tables
int bPawnSqT[64] = {
     0,  0,   0,   0,   0,   0,  0,   0,
     5, 10,  10, -20, -20,  10, 10,   5,
     5, -5, -10,   0,   0, -10, -5,   5,
     0,  0,   0,  20,  20,   0,  0,   0,
     5,  5,  10,  25,  25,  10,  5,   5,
    10, 10,  20,  30,  30,  20,  10, 10,
    50, 50,  50,  50,  50,  50,  50, 50,
     0,  0,   0,   0,   0,   0,   0,  0,

};

int bKnightSqT[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50,

};

int bBishopsSqT[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -20, -10, -10, -10, -10, -10, -10, -20,

};

int bRookSqT[64] = {
     0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5, 10, 10, 10, 10, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0,

};

int bQueenSqT[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10,   0,   0,  0,  0,   5,   0, -10,
    -10,   0,   5,  5,  5,   5,   5, -10,
    -5,    0,   5,  5,  5,   5,   0,   0,
    -5,    0,   5,  5,  5,   5,   0,  -5,
    -10,   0,   5,  5,  5,   5,   0, -10,
    -10,   0,   0,  0,  0,   0,   0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20,
};

int bKingMidSqT[64]= {
     20,  30,  10,   0,   0,  10,  30,  20,
     20,  20,   0,   0,   0,   0,  20,  20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
};

int bKingEndSqT[64] = {
    -50, -30, -30, -30, -30, -30, -30, -50,
    -30, -30,   0,   0,   0,   0, -30, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -20, -10,   0,   0, -10, -20, -30,
    -50, -40, -30, -20, -20, -30, -40, -50,
};

int weak_pawn_pcsq[2][64] = { {
     0,   0,   0,   0,   0,   0,   0,   0,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
     0,   0,   0,   0,   0,   0,   0,   0
}, {
   0, 0, 0, 0, 0, 0, 0, 0,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   -10, -12, -14, -16, -16, -14, -12, -10,
   0, 0, 0, 0, 0, 0, 0, 0,
}
};

int passed_pawn_pcsq[2][64] = { {
     0,   0,   0,   0,   0,   0,   0,   0,
   140, 140, 140, 140, 140, 140, 140, 140,
    92,  92,  92,  92,  92,  92,  92,  92,
    56,  56,  56,  56,  56,  56,  56,  56,
    32,  32,  32,  32,  32,  32,  32,  32,
    20,  20,  20,  20,  20,  20,  20,  20,
    20,  20,  20,  20,  20,  20,  20,  20,
     0,   0,   0,   0,   0,   0,   0,   0
}, {
    0, 0, 0, 0, 0, 0, 0, 0,
    20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20,
    32, 32, 32, 32, 32, 32, 32, 32,
    56, 56, 56, 56, 56, 56, 56, 56,
    92, 92, 92, 92, 92, 92, 92, 92,
    140, 140, 140, 140, 140, 140, 140, 140,
    0, 0, 0, 0, 0, 0, 0, 0,
}
};

int evaluateBB::getPieceValue(int location)
{
    //create an empty board then shift a 1 over to the current i location
    U64 pieceLocation = 1LL << location;

    //white pieces
    if(evalMoveGen.BBWhitePieces & pieceLocation){
        if(pieceLocation & evalMoveGen.BBWhitePawns){
            pawnCount[0] ++;
            return 100; //eval pawns sepperatly

        } else if(pieceLocation & evalMoveGen.BBWhiteRooks){
            rookCount[0] ++;
            evalRook(true, location);
            return 500 + wRooksSqT[location];

        } else if(pieceLocation & evalMoveGen.BBWhiteKnights){
            knightCount[0] ++;
            evalKnight(true, location);
            return 320 + wKnightsSqT[location];

        } else if(pieceLocation & evalMoveGen.BBWhiteBishops){
            bishopCount[0] ++;
            evalBishop(true, location);
            return 330 + wBishopsSqT[location];

        } else if(pieceLocation & evalMoveGen.BBWhiteQueens){
            evalQueen(true, location);
            return 900 + wQueensSqt[location];

        } else if(pieceLocation & evalMoveGen.BBWhiteKing){

            //If both sides have no queens use king end game board
            if((evalMoveGen.BBWhiteQueens | evalMoveGen.BBBlackQueens) & full){
                return 20000 + wKingEndSqT[location];
            }
            //if end game conditions fail use mid game king board
            return 20000 + wKingMidSqT[location];

        }
    } else if (evalMoveGen.BBBlackPieces & pieceLocation) {
        if(pieceLocation & evalMoveGen.BBBlackPawns ){
            pawnCount[1] ++;
            return -100;

        } else if(pieceLocation & evalMoveGen.BBBlackRooks){
            rookCount[1] ++;
            evalRook(false, location);
            return -500 -bRookSqT[location];

        } else if(pieceLocation & evalMoveGen.BBBlackKnights){
            knightCount[1] ++;
            evalKnight(false, location);
            return -320 -bKnightSqT[location];

        } else if(pieceLocation & evalMoveGen.BBBlackBishops){
            bishopCount[1] ++;
            evalBishop(false, location);
            return -330 -bBishopsSqT[location];

        } else if(pieceLocation & evalMoveGen.BBBlackQueens){

            evalQueen(false, location);
            return -900 -bQueenSqT[location];

        } else if(pieceLocation & evalMoveGen.BBBlackKing){
            if((evalMoveGen.BBBlackQueens | evalMoveGen.BBWhiteQueens) & full){
                return -20000 -bKingEndSqT[location];
            }
         return -20000 -bKingMidSqT[location];
        }
    }
    return 0;
}

void evaluateBB::generateKingZones(bool isWhite)
{
    U64 king;
    if(isWhite){
        king = evalMoveGen.BBWhiteKing;
    } else {
        king = evalMoveGen.BBBlackKing;
    }
    MoveGen *m;
    //draw 8 tile zone around king to psuedo king BB ~~ needs to be eleven tile eventually
    king |= evalMoveGen.northOne(king);
    king |= evalMoveGen.noEaOne(king);
    king |= evalMoveGen.eastOne(king);
    king |= evalMoveGen.soEaOne(king);
    king |= evalMoveGen.southOne(king);
    king |= evalMoveGen.soWeOne(king);
    king |= evalMoveGen.westOne(king);
    king |= evalMoveGen.noWeOne(king);

    if(isWhite){
        wKingZ = king;
    } else {
        bKingZ = king;
    }

}

int evaluateBB::getPawnScore()
{
    //get zobristE/bitboard of current pawn positions
    U64 pt = evalMoveGen.BBWhitePawns | evalMoveGen.BBBlackPawns;
    int hash = (int)(pt % 400000);
    //probe pawn hash table
    if(transpositionPawn[hash].zobrist == pt){
        return transpositionPawn[hash].eval;
    }

    int score = 0;
    U64 pieceLocation;
    for(int i = 0; i < 64; i++){
        pieceLocation = 1LL << i;
        if(pieceLocation & evalMoveGen.BBWhitePawns){
            score += pawnEval(true, i);
        } else if (pieceLocation & evalMoveGen.BBBlackPawns){
            score -= pawnEval(false, i);
        }
    }

    //store entry to pawn hash table
    transpositionPawn[hash].eval = score;
    transpositionPawn[hash].zobrist = pt;

    return score;
}

int evaluateBB::pawnEval(bool isWhite, int location)
{
    int side;
    int result = 0;
    int flagIsPassed = 1; // we will be trying to disprove that
    int flagIsWeak = 1;   // we will be trying to disprove that
    int flagIsOpposed = 0;

    U64 pawn = 0LL, opawns, epawns;
    pawn += 1LL << location;
    if(isWhite){
        opawns = evalMoveGen.BBWhitePawns;
        epawns = evalMoveGen.BBBlackPawns;
        side = 0;
    } else {
        opawns = evalMoveGen.BBBlackPawns;
        epawns = evalMoveGen.BBWhitePawns;
        side = 1;
    }

    int file = location % 8;
    int rank = location / 8;

    U64 doubledPassMask = FileMasks8[file]; //mask for finding doubled or passed pawns

    U64 left = 0LL;
    if(file > 0) left = FileMasks8[file-1]; //masks are accoring to whites perspective

    U64 right = 0LL;
    if(file < 7) right = FileMasks8[file+1]; //files to the left and right of pawn

    U64 supports = right | left, tmpSup = 0LL; //mask for area behind pawn and to the left an right, used to see if weak + mask for holding and values

    opawns &= ~ pawn; //remove this pawn from his friendly pawn BB so as not to count himself in doubling

    if(doubledPassMask & opawns) result -= 10; //real value for doubled pawns is -twenty, because this method counts them twice it's set at half real

    if(isWhite){
        for(int i = 7; i > rank-1; i--) {
            doubledPassMask &= ~RankMasks8[i];
            left &= ~RankMasks8[i];
            right &= ~RankMasks8[i];
            tmpSup |= RankMasks8[i];
        }
    } else {
        for(int i = 0; i < rank+1; i++) {
            doubledPassMask &= ~RankMasks8[i];
            left &= ~RankMasks8[i];
            right &= ~RankMasks8[i];
            tmpSup |= RankMasks8[i];
        }
    }


    //if there is an enemy pawn ahead of this pawn
    if(doubledPassMask & epawns) flagIsOpposed = 1;

    //if there is an enemy pawn on the right or left ahead of this pawn
    if(right & epawns || left & epawns) flagIsPassed = 0;

    opawns |= pawn; // restore real our pawn board

    tmpSup &= ~ RankMasks8[rank]; //remove our rank from supports
    supports &= tmpSup; // get BB of area whether support pawns could be

    //if there are pawns behing this pawn and to the left or the right pawn is not weak
    if(supports & opawns) flagIsWeak = 0;


    //evaluate passed pawns, scoring them higher if they are protected or
    //if their advance is supported by friendly pawns
    if(flagIsPassed){
        if(isPawnSupported(isWhite, pawn, opawns)){
            result += (passed_pawn_pcsq[side][location] * 10) / 8;
        } else {
            result += passed_pawn_pcsq[side][location];
        }
    }

    //eval weak pawns, increasing the penalty if they are in a half open file
    if(flagIsWeak){
        result += weak_pawn_pcsq[side][location];

        if(flagIsOpposed){
            result -= 4;
        }
    }

    return result;

}

int evaluateBB::isPawnSupported(bool isWhite, U64 pawn, U64 pawns)
{
    if(evalMoveGen.westOne(pawn) & pawns) return 1;
    if(evalMoveGen.eastOne(pawn) & pawns) return 1;

    if(isWhite){
        if(evalMoveGen.soWeOne(pawn) & pawns) return 1;
        if(evalMoveGen.soEaOne(pawn) & pawns) return 1;
    } else {
        if(evalMoveGen.noWeOne(pawn) & pawns) return 1;
        if(evalMoveGen.noEaOne(pawn) & pawns) return 1;
    }
    return 0;
}

void evaluateBB::evalKnight(bool isWhite, int location)
{
    int kAttks = 0, mob = 0, side;
    gamePhase += 1;

    U64 knight = 0LL, friends, eking, kingZone;
    knight += 1LL << location;
    if(isWhite){
        friends = evalMoveGen.BBWhitePieces;
        eking = evalMoveGen.BBBlackKing;
        kingZone = bKingZ;
        side = 0;
    } else {
        friends = evalMoveGen.BBBlackPieces;
        eking = evalMoveGen.BBWhiteKing;
        kingZone = wKingZ;
        side = 1;
    }

//similar to move generation code except we increment mobility counter and king area attack counters
    U64 moves;

    if(location > 18){
        moves = evalMoveGen.KNIGHT_SPAN<<(location-18);
    } else {
        moves = evalMoveGen.KNIGHT_SPAN>>(18-location);
    }

    if(location % 8 < 4){
        moves &= ~evalMoveGen.FILE_GH & ~friends & ~eking;
    } else {
        moves &= ~evalMoveGen.FILE_AB & ~friends & ~eking;
    }

    U64 j = moves & ~(moves-1);

    while(j != 0){
        //for each move not on friends increment mobility
        ++mob;

        if(j & kingZone){
            ++kAttks; //this knight is attacking zone around enemy king
        }
        moves &= ~j;
        j = moves & ~(moves-1);
    }

    //Evaluate mobility. We try to do it in such a way zero represent average mob
    midGMobility[side] += 4 *(mob-4);
    endGMobility[side] += 4 *(mob-4);

    //save data on king attacks
    if(kAttks){
        attCount[side] ++;
        attWeight[side] += 2 * kAttks;
    }

}

void evaluateBB::evalBishop(bool isWhite, int location)
{
    int kAttks = 0, mob = 0, side;
    gamePhase += 1;

    U64 bishop = 0LL, friends, eking, kingZone;
    bishop += 1LL << location;
    if(isWhite){
        friends = evalMoveGen.BBWhitePieces;
        eking = evalMoveGen.BBBlackKing;
        kingZone = bKingZ;
        side = 0;
    } else {
        friends = evalMoveGen.BBBlackPieces;
        eking = evalMoveGen.BBWhiteKing;
        kingZone = wKingZ;
        side = 1;
    }

    //U64 moves = evalMoveGen.DAndAntiDMoves(location) & ~friends & ~eking;
    U64 moves = slider_attacks.BishopAttacks(evalMoveGen.FullTiles, location);
    moves &= ~friends & ~ eking;

    U64 j = moves & ~ (moves-1);
    while(j != 0){
        ++mob; //increment bishop mobility

        if(j & kingZone){
            ++kAttks; //this bishop is attacking zone around enemy king
        }
        moves &= ~j;
        j = moves & ~(moves-1);
    }

    //Evaluate mobility. We try to do it in such a way zero represent average mob
    midGMobility[side] += 3 *(mob-7);
    endGMobility[side] += 3 *(mob-7);

    //save data on king attacks
    if(kAttks){
        attCount[side] ++;
        attWeight[side] += 2 * kAttks;
    }


}

void evaluateBB::evalRook(bool isWhite, int location)
{
    bool  ownBlockingPawns = false, oppBlockingPawns = false;
    int kAttks = 0, mob = 0, side;
    gamePhase += 2;

    U64 rook = 0LL, friends, eking, kingZone, currentFile, opawns, epawns;
    rook += 1LL << location;

    int x = location % 8;
    currentFile = evalMoveGen.FileABB << x;

    if(isWhite){
        friends = evalMoveGen.BBWhitePieces;
        eking = evalMoveGen.BBBlackKing;
        kingZone = bKingZ;
        side = 0;

        opawns = evalMoveGen.BBWhitePawns;
        epawns = evalMoveGen.BBBlackPawns;
    } else {
        friends = evalMoveGen.BBBlackPieces;
        eking = evalMoveGen.BBWhiteKing;
        kingZone = wKingZ;
        side = 1;

        opawns = evalMoveGen.BBBlackPawns;
        epawns = evalMoveGen.BBWhitePawns;
    }

//open and half open file detection add bonus to mobility score of side
    if(currentFile & opawns){
        ownBlockingPawns = true;
    }
    if (currentFile & epawns){
        oppBlockingPawns = true;
    }

    if(!ownBlockingPawns){
        if(!oppBlockingPawns){
            midGMobility[side] += rookOpenFile;
            endGMobility[side] += rookOpenFile;
        } else {
            midGMobility[side] += rookHalfOpenFile;
            endGMobility[side] += rookHalfOpenFile;
        }
    }

//mobility and king attack gen/detection
    U64 moves = slider_attacks.RookAttacks(evalMoveGen.FullTiles, location);
    moves &= ~friends & ~ eking;

    U64 j = moves & ~ (moves-1);
    while(j != 0){
        ++mob; //increment bishop mobility

        if(j & kingZone){
            ++kAttks; //this bishop is attacking zone around enemy king
        }
        moves &= ~j;
        j = moves & ~(moves-1);
    }

    //Evaluate mobility. We try to do it in such a way zero represent average mob
    midGMobility[side] += 2 *(mob-7);
    endGMobility[side] += 4 *(mob-7);

    //save data on king attacks
    if(kAttks){
        attCount[side] ++;
        attWeight[side] += 3 * kAttks;
    }
}

void evaluateBB::evalQueen(bool isWhite, int location)
{
    gamePhase += 4;
    int kAttks = 0, mob = 0, side;

    U64 queen = 0LL, friends, eking, kingZone;
    queen += 1LL << location;
    if(isWhite){
        friends = evalMoveGen.BBWhitePieces;
        eking = evalMoveGen.BBBlackKing;
        kingZone = bKingZ;
        side = 0;
    } else {
        friends = evalMoveGen.BBBlackPieces;
        eking = evalMoveGen.BBWhiteKing;
        kingZone = wKingZ;
        side = 1;
    }

//similar to move gen, increment mobility and king attacks
    U64 moves = slider_attacks.QueenAttacks(evalMoveGen.FullTiles, location);
    moves &= ~friends & ~ eking;

    U64 j = moves & ~ (moves-1);
    while(j != 0){
        ++mob; //increment bishop mobility

        if(j & kingZone){
            ++kAttks; //this bishop is attacking zone around enemy king
        }
        moves &= ~j;
        j = moves & ~(moves-1);
    }

    //Evaluate mobility. We try to do it in such a way zero represent average mob
    midGMobility[side] += 1 *(mob-14);
    endGMobility[side] += 2 *(mob-14);

    //save data on king attacks
    if(kAttks){
        attCount[side] ++;
        attWeight[side] += 4 * kAttks;
    }


}


