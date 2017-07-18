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

    //iterative deepening
    std::string iterativeDeep(int depth);

private:
    //minmax with alpha beta, the main component of our search
    int alphaBeta(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, bool allowNull);

        //counts number of piece postitions tried
        int positionCount = 0;

        //sort moves and apply heuristics like killer and transpostion data
        std::string sortMoves(std::string moves, HashEntry entry, int currentDepth, bool isWhite);

        //killer heuristics function
        std::string killerHe(int depth, std::string moves);
             std::stack<std::string> killerHArr[25];
             std::string killers[3][29];
        std::string killerTest(int depth, std::string moves);
        void addToKillers(int depth, std::string move);

        //Null moves function
        int nullMoves(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth);

        //Quiescent search ~~ search positions farther if there are captures on horizon
        int quiescent(int alpha, int beta, bool isWhite, int currentDepth, int quietDepth, long currentTime, long timeLimmit);
        //if a capture cannot increase alpha, don't bother searching it
        bool deltaPruning(std::string move, int eval, bool isWhite, int alpha, bool isEndGame);

        //orders moves by placing in order of most valuable victim least valuable attacker
        std::string mostVVLVA(std::string captures, bool isWhite);

//transposition table functions
    //add best move to TT
    void addMoveTT(std::string bestmove, int depth, long eval, int flag);
    //add to quiescience TT
    void addTTQuiet(std::string bestmove, int quietDepth, long eval, int flag);


//misc functions that are not implemented or are old

    //root function for recursive move finiding via minimax
    std::string miniMaxRoot(int depth, bool isMaximisingPlayer, long currentTime, long timeLimmit);

    //bulk of minimax
    long miniMax(int depth, long alpha, long beta, bool isMaximisingPlayer, long currentTime, long timeLimmit);

    std::string debug(std::string ttMove, std::string moves);


    //TEST
    int principleV(int depth, int alpha, int beta, bool isWhite, int currentDepth);

    int zWSearch(int depth, int beta, bool isWhite);



};

#endif // AI_LOGIC_H
