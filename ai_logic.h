#ifndef AI_LOGIC_H
#define AI_LOGIC_H

#include <string>
#include <algorithm>
#include <stack>

#include "externs.h"
#include "evaluatebb.h"



class Ai_Logic
{
public:
    Ai_Logic();

    std::vector<std::string> possible_moves;

    //root function for recursive move finiding via minimax
    std::string miniMaxRoot(int depth, bool isMaximisingPlayer);


private:
    //bulk of minimax
    long miniMax(int depth, long alpha, long beta, bool isMaximisingPlayer, int numberOfMoves);
    //killer heuristics function
    std::string killerHe(int depth, std::string moves, bool isWhite);
    std::stack<std::string> killerHArr[7];    
    //Null move pruning
    long nullMovePruning(int depth, long alpha, long beta, bool isMaximisingPlayer);


    //counts number of piece postitions tried
    long positionCount = 0;

    //sort first few moves to increase alpha beta efficiancy
    std::vector<std::string> sortMoves(std::vector<std::string> moves, bool isMaximisingPlayer);

    int modifyDepth(int depth, int numberOfMoves);

};

#endif // AI_LOGIC_H
