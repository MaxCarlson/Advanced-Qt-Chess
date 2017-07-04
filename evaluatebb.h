#ifndef EVALUATEBB_H
#define EVALUATEBB_H

typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL


class evaluateBB
{
public:
    evaluateBB();

    //forms total evaluation for baord state
    int evalBoard();

    int getPieceValue(int location);
};

#endif // EVALUATEBB_H
