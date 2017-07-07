#ifndef HASHENTRY_H
#define HASHENTRY_H

typedef unsigned long long  U64; // supported by MSC 13.00+ and C99
#define C64(constantU64) constantU64##ULL

class HashEntry
{
public:
    HashEntry();

private:
    //Z key for move
    U64 zobrist;
    //depth of move
    int depth;
    //move rating
    int eval;
    //how old move is
    int ancient;

};

#endif // HASHENTRY_H