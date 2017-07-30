#ifndef MOVE_H
#define MOVE_H

#include "externs.h"

class Move
{
public:
    Move();

    int x;
    int y;
    int x1;
    int y1;
    U8 p; //later to replace piece
    char piece;
    char captured;
    int score;
    char flag;
    bool tried;

    /*
    char id;
    char from;
    char to;
    U8 piece_from;
    U8 piece_to;
    U8 piece_cap;
    char flags;
    char castle;
    char ply;
    char ep;
    int score;
    */

};

#endif // MOVE_H
