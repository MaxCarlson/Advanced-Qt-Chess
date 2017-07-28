#ifndef MOVE_H
#define MOVE_H


class Move
{
public:
    Move();

    int x;
    int y;
    int x1;
    int y1;
    char piece;
    char captured;
    int score;
    char flag;
    bool tried;
};

#endif // MOVE_H
