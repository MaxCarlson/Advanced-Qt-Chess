#ifndef AI_LOGIC_H
#define AI_LOGIC_H

#include <string>
#include <algorithm>

#include "externs.h"
#include "evaluate.h"



class Ai_Logic
{
public:
    Ai_Logic();

    std::vector<std::string> possible_moves;

    //find board values
    static float evaluateBoard(float depth, int NumberOfMoves);

    //peice values and abs val for black and white
    static float getPieceValue(std::string piece, int x, int y);
    static float getAbsoluteValue(std::string piece, int x, int y);

    //root function for recursive move finiding via minimax
    std::string miniMaxRoot(int depth, bool isMaximisingPlayer);

    float miniMax(float depth, float alpha, float beta, bool isMaximisingPlayer, int numberOfMoves);

private:
    //change board back only one move
    void undoMove(int x, int y, int x1, int y1, std::string piece1, std::string piece2);
    //change board back to before move testing/evaluating state
    void undo_move1();

    //sort first few moves to increase alpha beta efficiancy
    std::vector<std::string> sortMoves(std::vector<std::string> moves, bool isMaximisingPlayer);

    //counts number of piece postitions tried
    long positionCount = 0;

    void queenHasMoved();

    int modifyDepth(int depth, int numberOfMoves);

};

#endif // AI_LOGIC_H
