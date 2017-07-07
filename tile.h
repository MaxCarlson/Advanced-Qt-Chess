#ifndef TILE_H
#define TILE_H


#include <QLabel>



class Tile : public QLabel
{
public:
    //Fields
    int tileColor, piece, pieceColor, row, col, tileNum;
    std::string pieceName;
    int tempx, tempy, tempx2, tempy2;

    //constructor
    Tile(QWidget* pParent=0, Qt::WindowFlags f=0) : QLabel(pParent, f) {};
    Tile(const QString& text, QWidget* pParent = 0, Qt::WindowFlags f = 0) : QLabel(text, pParent, f){};

    //mouse clicks
    void mousePressEvent(QMouseEvent * event);

    //Move checking
    bool moveChecking(Tile *temp, int count);
    
    //display pieces on correct tiles
    void display(std::string elem);

    //color tiles
    void tileDisplay();

    void aiTurn();

};

#endif // TILE_H
