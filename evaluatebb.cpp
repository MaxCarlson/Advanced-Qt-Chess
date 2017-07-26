#include "evaluatebb.h"
#include "externs.h"

const U64 full  = 0xffffffffffffffffULL;

//adjustments of piece value based on our color pawn count
const int knight_adj[9] = { -20, -16, -12, -8, -4,  0,  4,  8, 10};
const int rook_adj[9] =   {  15,  12,   9,  6,  3,  0, -3, -6, -9};

//values definitely need to be adjusted and file/rank/side variable
const int rookOpenFile = 10;
const int rookHalfOpenFile = 5;

const int BISHOP_PAIR = 30;
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

int evaluateBB::evalBoard(bool isWhite, BitBoards *BBBoard)
{
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
    generateKingZones(true, BBBoard);
    generateKingZones(false, BBBoard);


//loop through all pieces and gather numbers, mobility, king attacks, etc
    for(int i = 0; i < 64; i++){
        totalEvaualtion += getPieceValue(i, BBBoard);
    }


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

    if(attCount[0] < 2 || BBBoard->BBWhiteQueens == 0LL) attWeight[0] = 0;
    if(attCount[1] < 2 || BBBoard->BBBlackQueens == 0LL) attWeight[1] = 0;
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
    int mgWeight = gamePhase;
    int egWeight = 24 - gamePhase;

    totalEvaualtion +=( (midGScore * mgWeight) + (endGScore * egWeight)) / 24;




    if(!isWhite) totalEvaualtion = -totalEvaualtion;

    return totalEvaualtion;


}

int evaluateBB::returnMateScore(bool isWhite, BitBoards *BBBoard, int depth)
{
    //if we have no moves and we're in check
    if(BBBoard->isInCheck(isWhite)){
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

int evaluateBB::getPieceValue(int location, BitBoards *BBBoard)
{
    //create an empty board then shift a 1 over to the current i location
    U64 pieceLocation = 0LL;
    pieceLocation += 1LL<<location;

    //white pieces
    if(BBBoard->BBWhitePieces & pieceLocation){
        if(pieceLocation & BBBoard->BBWhitePawns){

            pawnCount[0] ++;
            return 100 +  pawnEval(true, location, BBBoard);

        } else if(pieceLocation & BBBoard->BBWhiteRooks){
            rookCount[0] ++;
            evalRook(true, location, BBBoard);
            return 500 + wRooksSqT[location];

        } else if(pieceLocation & BBBoard->BBWhiteKnights){
            knightCount[0] ++;
            evalKnight(true, location, BBBoard);
            return 320 + wKnightsSqT[location];

        } else if(pieceLocation & BBBoard->BBWhiteBishops){
            bishopCount[0] ++;
            evalBishop(true, location, BBBoard);
            return 330 + wBishopsSqT[location];

        } else if(pieceLocation & BBBoard->BBWhiteQueens){
            evalQueen(true, location, BBBoard);
            return 900 + wQueensSqt[location];

        } else if(pieceLocation & BBBoard->BBWhiteKing){

            //If both sides have no queens use king end game board
            if((BBBoard->BBWhiteQueens | BBBoard->BBBlackQueens) & full){
                return 20000 + wKingEndSqT[location];
            }
            //if end game conditions fail use mid game king board
            return 20000 + wKingMidSqT[location];

        }
    } else if (BBBoard->BBBlackPieces & pieceLocation) {
        if(pieceLocation & BBBoard->BBBlackPawns ){
            pawnCount[1] ++;
            return -100  - pawnEval(false, location, BBBoard);

        } else if(pieceLocation & BBBoard->BBBlackRooks){
            rookCount[1] ++;
            evalRook(false, location, BBBoard);
            return -500 -bRookSqT[location];

        } else if(pieceLocation & BBBoard->BBBlackKnights){
            knightCount[1] ++;
            evalKnight(false, location, BBBoard);
            return -320 -bKnightSqT[location];

        } else if(pieceLocation & BBBoard->BBBlackBishops){
            bishopCount[1] ++;
            evalBishop(false, location, BBBoard);
            return -330 -bBishopsSqT[location];

        } else if(pieceLocation & BBBoard->BBBlackQueens){

            evalQueen(false, location, BBBoard);
            return -900 -bQueenSqT[location];

        } else if(pieceLocation & BBBoard->BBBlackKing){
            if((BBBoard->BBBlackQueens | BBBoard->BBWhiteQueens) & full){
                return -20000 -bKingEndSqT[location];
            }
         return -20000 -bKingMidSqT[location];
        }
    }
    return 0;
}

void evaluateBB::generateKingZones(bool isWhite, BitBoards *BBBoard)
{
    U64 king;
    if(isWhite){
        king = BBBoard->BBWhiteKing;
    } else {
        king = BBBoard->BBBlackKing;
    }

    //draw 8 tile zone around king to psuedo king BB
    king |= BBBoard->northOne(king);
    king |= BBBoard->noEaOne(king);
    king |= BBBoard->eastOne(king);
    king |= BBBoard->soEaOne(king);
    king |= BBBoard->southOne(king);
    king |= BBBoard->soWeOne(king);
    king |= BBBoard->westOne(king);
    king |= BBBoard->noWeOne(king);

    if(isWhite){
        wKingZ = king;
    } else {
        bKingZ = king;
    }

}

int evaluateBB::pawnEval(bool isWhite, int location, BitBoards *BBBoard)
{
    int side;
    int result = 0;
    int flagIsPassed = 1; // we will be trying to disprove that
    int flagIsWeak = 1;   // we will be trying to disprove that
    int flagIsOpposed = 0;

    U64 pawn = 0LL, opawns, epawns;
    pawn += 1LL << location;
    if(isWhite){
        opawns = BBBoard->BBWhitePawns;
        epawns = BBBoard->BBBlackPawns;
        side = 0;
    } else {
        opawns = BBBoard->BBBlackPawns;
        epawns = BBBoard->BBWhitePawns;
        side = 1;
    }
/*
    BBBoard->drawBB(pawn);
    pawn = pawn << 8;
    BBBoard->drawBB(pawn);
    pawn = pawn >> 8;
    BBBoard->drawBB(pawn);
*/
    //loop through and find out if pawn is doubled, passed, or opposed
    U64 tpawn = pawn;
    while(tpawn != 0LL){
        if(isWhite){
            tpawn = tpawn >> 8; //move "pawn" one step north
            flagIsPassed = 0;
            //if pawn is blocked by our pawn
            if(tpawn & opawns){
                result -= 20; //doubled penalty

            } else if (tpawn & epawns) {
                flagIsOpposed = 1; //if pawn is opposed
            }

            //if there is an enemy pawn to the north east or north west, pawn is no longer passed
            //Not file A/H used to ensure wrapping doesn't occur
            if((BBBoard->noWeOne(tpawn) & epawns & ~ BBBoard->FileHBB) || (BBBoard->noEaOne(tpawn) & epawns & ~BBBoard->FileABB)){
                flagIsPassed = 0;
            }

        } else {
            tpawn = tpawn << 8; //one step south
            flagIsPassed = 0;
            //if pawn is blocked by our pawn
            if(tpawn & opawns){
                result -= 20; //doubled penalty

            } else if (tpawn & epawns) {
                flagIsOpposed = 1; //if pawn is opposed
            }

            //if there is an enemy pawn to the south east or south west, pawn is no longer passed
            if((BBBoard->soWeOne(tpawn) & epawns & ~ BBBoard->FileHBB) || (BBBoard->soEaOne(tpawn) & epawns) & ~BBBoard->FileABB){
                flagIsPassed = 0;
            }

        }
    }

//another loop going backwards looking if the pawn is supported by friendly pawns
    tpawn = pawn;

    while(tpawn != 0LL){
        if(isWhite){
            tpawn = tpawn << 8; //south one

            if(BBBoard->soWeOne(tpawn) & opawns &~ BBBoard->FileHBB){
                flagIsWeak = 0;
                break;
            }

            if(BBBoard->soEaOne(tpawn) & opawns & ~BBBoard->FileABB){
                flagIsWeak = 0;
                break;
            }
        } else {
            tpawn = tpawn >> 8; //north one

            if(BBBoard->noWeOne(tpawn) & opawns &~ BBBoard->FileHBB){
                flagIsWeak = 0;
                break;
            }

            if(BBBoard->noEaOne(tpawn) & opawns & ~BBBoard->FileABB){
                flagIsWeak = 0;
                break;
            }
        }
    }

    //evaluate passed pawns, scoring them higher if they are protected or
    //if their advance is supported by friendly pawns
    if(flagIsPassed){
        if(isPawnSupported(isWhite, BBBoard, pawn, opawns)){
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

int evaluateBB::isPawnSupported(bool isWhite, BitBoards *BBBoard, U64 pawn, U64 pawns)
{
    if(BBBoard->westOne(pawn) & pawns) return 1;
    if(BBBoard->eastOne(pawn) & pawns) return 1;

    if(isWhite){
        if(BBBoard->soWeOne(pawn) & pawns) return 1;
        if(BBBoard->soEaOne(pawn) & pawns) return 1;
    } else {
        if(BBBoard->noWeOne(pawn) & pawns) return 1;
        if(BBBoard->noEaOne(pawn) & pawns) return 1;
    }
    return 0;
}

void evaluateBB::evalKnight(bool isWhite, int location, BitBoards *BBBoard)
{
    int kAttks = 0, mob = 0, side;
    gamePhase += 1;

    U64 knight = 0LL, friends, eking, kingZone;
    knight += 1LL << location;
    if(isWhite){
        friends = BBBoard->BBWhitePieces;
        eking = BBBoard->BBBlackKing;
        kingZone = bKingZ;
        side = 0;
    } else {
        friends = BBBoard->BBBlackPieces;
        eking = BBBoard->BBWhiteKing;
        kingZone = wKingZ;
        side = 1;
    }

//similar to move generation code except we increment mobility counter and king area attack counters
    U64 moves;

    if(location > 18){
        moves = BBBoard->KNIGHT_SPAN<<(location-18);
    } else {
        moves = BBBoard->KNIGHT_SPAN>>(18-location);
    }

    if(location % 8 < 4){
        moves &= ~BBBoard->FILE_GH & ~friends & ~eking;
    } else {
        moves &= ~BBBoard->FILE_AB & ~friends & ~eking;
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

void evaluateBB::evalBishop(bool isWhite, int location, BitBoards *BBBoard)
{
    int kAttks = 0, mob = 0, side;
    gamePhase += 1;

    U64 bishop = 0LL, friends, eking, kingZone;
    bishop += 1LL << location;
    if(isWhite){
        friends = BBBoard->BBWhitePieces;
        eking = BBBoard->BBBlackKing;
        kingZone = bKingZ;
        side = 0;
    } else {
        friends = BBBoard->BBBlackPieces;
        eking = BBBoard->BBWhiteKing;
        kingZone = wKingZ;
        side = 1;
    }

    U64 moves = BBBoard->DAndAntiDMoves(location) & ~friends & ~eking;

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

void evaluateBB::evalRook(bool isWhite, int location, BitBoards *BBBoard)
{
    bool  ownBlockingPawns = false, oppBlockingPawns = false;
    int kAttks = 0, mob = 0, side;
    gamePhase += 2;

    U64 rook = 0LL, friends, eking, kingZone, currentFile, opawns, epawns;
    rook += 1LL << location;

    int x = location % 8;
    currentFile = BBBoard->FileABB << x;

    if(isWhite){
        friends = BBBoard->BBWhitePieces;
        eking = BBBoard->BBBlackKing;
        kingZone = bKingZ;
        side = 0;

        opawns = BBBoard->BBWhitePawns;
        epawns = BBBoard->BBBlackPawns;
    } else {
        friends = BBBoard->BBBlackPieces;
        eking = BBBoard->BBWhiteKing;
        kingZone = wKingZ;
        side = 1;

        opawns = BBBoard->BBBlackPawns;
        epawns = BBBoard->BBWhitePawns;
    }

//open and half open file detection add bonus to mobility score of side
    if(currentFile & opawns){
        ownBlockingPawns = true;
    } else if (currentFile & epawns){
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
    U64 moves = BBBoard->horizVert(location) & ~friends & ~eking;

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

void evaluateBB::evalQueen(bool isWhite, int location, BitBoards *BBBoard)
{
    gamePhase += 4;
    int kAttks = 0, mob = 0, side;

    U64 queen = 0LL, friends, eking, kingZone;
    queen += 1LL << location;
    if(isWhite){
        friends = BBBoard->BBWhitePieces;
        eking = BBBoard->BBBlackKing;
        kingZone = bKingZ;
        side = 0;
    } else {
        friends = BBBoard->BBBlackPieces;
        eking = BBBoard->BBWhiteKing;
        kingZone = wKingZ;
        side = 1;
    }

//similar to move gen, increment mobility and king attacks
    U64 moves = (BBBoard->DAndAntiDMoves(location) | BBBoard->horizVert(location)) & ~friends & ~eking;

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

