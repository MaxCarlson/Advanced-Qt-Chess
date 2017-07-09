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
    //Check transposition table for a move and decide whether or not to use value
    bool checkTTable(int depth, int eval);

    //add best move to TT
    void addMoveTT(std::string bestmove, int depth, long eval, int flag);
    //look up if board state has been evaluated and stored to table previously, if so add best move first ~~ possibly only use best move for search
    std::string lookUpTTable(int depth);

    long lookUpTTEval(int depth);

    std::string debug(std::string ttMove, std::string moves);



    //counts number of piece postitions tried
    long positionCount = 0;

    //sort first few moves to increase alpha beta efficiancy
    std::vector<std::string> sortMoves(std::vector<std::string> moves, bool isMaximisingPlayer);

    int modifyDepth(int depth, int numberOfMoves);

};

#endif // AI_LOGIC_H
