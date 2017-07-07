#ifndef HASHENTRY_H
#define HASHENTRY_H
#include <string>

typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL

class HashEntry
{
public:
    HashEntry();
    //Z key for move
    U64 zobrist;
    //depth of move
    int depth;
    //move rating
    int eval;
    //move string
    std::string move;
    //how old move is maybe make a bool so move can be replaced
    int ancient;

private:



};

#endif // HASHENTRY_H
