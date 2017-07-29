#ifndef EVALUATEBB_H
#define EVALUATEBB_H


typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL

class ZobristH;
class BitBoards;
class MoveGen;


class evaluateBB
{
public:
    evaluateBB();

    //forms total evaluation for baord state
    int evalBoard(bool isWhite, BitBoards *BBBoard, ZobristH *zobrist);

    //returns mate or stalemate score
    int returnMateScore(bool isWhite, MoveGen move_gen, int depth);

private:
    //material value representing wheter we call end game/mid game
    int gamePhase = 0;

    int pawnCount[2];
    int knightCount[2];
    int bishopCount[2];
    int rookCount[2];

    //attack count and weight against king by side, 0 is white attacking black king
    int attCount[2];
    int attWeight[2];

    //mid and end game mobility by color, 0 for white
    int midGMobility[2];
    int endGMobility[2];

    //gets rudimentry value of piece + square table value
    int getPieceValue(int location, MoveGen gen_moves);

    //generate zone around king ///Up for debate as to how large zone should be, currently encompasses 8 tiles directly around king
    //currently includes blocking pieces in zone
    void generateKingZones(bool isWhite, MoveGen move_gen);
        U64 wKingZ;
        U64 bKingZ;



//piece evaluation for mobility, attacking king squares, etc
    int pawnEval(bool isWhite, int location, MoveGen move_gen);
        int isPawnSupported(bool isWhite, MoveGen move_gen, U64 pawn, U64 pawns);

    void evalKnight(bool isWhite, int location, MoveGen move_gen);

    void evalBishop(bool isWhite, int location, MoveGen move_gen);

    void evalRook(bool isWhite, int location, MoveGen move_gen);

    void evalQueen(bool isWhite, int location, MoveGen move_gen);

    void saveTT(bool isWhite, ZobristH *zobrist, int totalEvaualtion, int hash);


};

#endif // EVALUATEBB_H
