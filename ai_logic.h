#ifndef AI_LOGIC_H
#define AI_LOGIC_H
#include <string>
#include <stack>
#include <thread>
#include "move.h"
//#include "hashentry.h"

class ZobristH;
class BitBoards;
class evaluateBB;
class HashEntry;


class Ai_Logic
{
public:
    Ai_Logic();

    //iterative deepening
    Move iterativeDeep(int depth);

private:
    //minmax with alpha beta, the main component of our search
    int alphaBeta(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, bool allowNull, BitBoards *BBBoard, ZobristH *zobrist, evaluateBB *eval);

        //sort moves and apply heuristics like killer and transpostion data
        std::string sortMoves(std::string moves, HashEntry entry, int currentDepth, bool isWhite, BitBoards *BBBoards, ZobristH *zobrist);

        //killer heuristics function
        std::string killerHe(int depth, std::string moves);
             std::stack<std::string> killerHArr[25];
             std::string killers[3][29];
        std::string killerTest(int depth, std::string moves);
        void addToKillers(int depth, std::string move);

        //Null moves function
        int nullMoves(int depth, int alpha, int beta, bool isWhite, long currentTime, long timeLimmit, int currentDepth, BitBoards *BBBoard, ZobristH *zobrist, evaluateBB *eval);

        //Quiescent search ~~ search positions farther if there are captures on horizon
        int quiescent(int alpha, int beta, bool isWhite, int currentDepth, int quietDepth, long currentTime, long timeLimmit, BitBoards *BBBoard, ZobristH *zobrist, evaluateBB *eval);
        //if a capture cannot increase alpha, don't bother searching it
        /*
        bool deltaPruning(std::string move, int eval, bool isWhite, int alpha, bool isEndGame, BitBoards *BBBoard);
        bool badCapture(std::string move, bool isWhite, BitBoards *BBBoard);
        */
        //orders moves by placing in order of most valuable victim least valuable attacker
        std::string mostVVLVA(std::string captures, bool isWhite, BitBoards *BBBoard);

        bool isCapture(std::string move, bool isWhite, BitBoards *BBBoard);


//transposition table functions
    //add best move to TT
    void addMoveTT(std::string bestmove, int depth, long eval, int flag, ZobristH *zobrist);
    //add to quiescience TT
    void addTTQuiet(std::string bestmove, int quietDepth, long eval, int flag, ZobristH *zobrist);

//object variables

    //princiapl variation array
    Move pVArr[29];

    //counts number of piece postitions tried
    int positionCount = 0;

    //count of quiescnce positions checked
    int qCount = 0;

    //number representing amount to reduce search with Null-Moves
    int depthR = 2;

};

#endif // AI_LOGIC_H
