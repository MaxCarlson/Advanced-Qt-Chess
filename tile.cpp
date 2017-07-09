#include "tile.h"
#include <QDebug>
#include "Pieces.h"
#include "externs.h"
#include "bitboards.h"
#include "ai_logic.h"


Pieces *isValid = new Pieces();

extern Tile * rect[8][8];
extern QWidget *myWidget;

Tile *click1;

// NOTE!!!!!
//Castling doesn't move the rook on the GUI only on the array of the board

void Tile::mousePressEvent(QMouseEvent * event){

    //qDebug() << "My Tile knows that you clicked on it!";
    moveChecking(this, ++count);

    //std::cout << ZKey->getZobristHash(true) << std::endl;

    //debugging stuff
    for(int i = 0; i < 8; i++){
        for(int j =0; j < 8; j++){
            std::cout << boardArr[i][j] << ", " ;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;


}

bool Tile::moveChecking(Tile *temp, int countC){
    
    
    if(countC==1){
        if(temp->piece && (temp->pieceColor==turns%2)){


            if(temp->pieceName != " "){
                tempx = temp->col;
                tempy = temp->row;
                click1 = new Tile();

                temp->setStyleSheet("QLabel {background-color: green;}");

                click1=temp;
            } else {
                count = 0;
            }
        } else {
            count = 0;
        }

    } else {

        if(temp->tileNum==click1->tileNum){
            click1->tileDisplay();
            count = 0;

        } else {
            //ready coordinates to give to Pieces
            tempx = click1->col;
            tempy = click1->row;
            tempx2 = temp->col;
            tempy2 = temp->row;
            //give coordinates of piece origin and possible landing
            isValid->coordinates(tempx, tempy, tempx2, tempy2);

            //check if input coordinates are a valid move for piece and player
            if(isValid->whichPiece() == true){

                //switch Qwidget values on origin and spot piece landed
                click1->piece=0;
                temp->piece=1;

                //give moved piece same attributes
                temp->pieceColor=click1->pieceColor;
                temp->pieceName=click1->pieceName;

                //display piece in correct spot on gui
                click1->display(click1->pieceName);
                temp->display(click1->pieceName);

                click1->tileDisplay();
                temp->tileDisplay();



                turns++;
                count = 0;

                //once move is made update the zorbist hash key
                ZKey->getZobristHash(true);

                if(aiOn == 1){
                    aiTurn();
                }


            } else {

                count = 1;
            }

        }

    }
    
}

void Tile::aiTurn(){

    Ai_Logic *newMove = new Ai_Logic;
    std::string bestMove = newMove->miniMaxRoot(5, true);

    //ready coordinates to give to Pieces
    //std::string bMove = best_moves[tester];
    tempx = (int)bestMove[0]-0;
    tempy = (int)bestMove[1]-0;
    tempx2 = (int)bestMove[2]-0;
    tempy2 = (int)bestMove[3]-0;

    //swap piece to its desitnation and make prior spot blank
    boardArr[tempy2][tempx2] = boardArr[tempy][tempx];
    boardArr[tempy][tempx] = " ";

    //create new tile objects to mirror rect tiles of piece and piece landing
    Tile *aiClick;
    Tile *aiClick1;

    //give Tile objects same value as rect[y][x]'s
    aiClick = ::rect[tempy][tempx];
    aiClick1 = ::rect[tempy2][tempx2];

    //switch piece values on origin and spot piece landed
    aiClick->piece = 0;
    aiClick1->piece = 1;


    //give moved piece same color at landing
    for(int k = 0; k < 7; k++){
        if(boardArr[tempy2][tempx2] == whitePieces[k]){
            aiClick1->pieceColor=0;

        } else if (boardArr[tempy2][tempx2] == blackPieces[k]){
            aiClick1->pieceColor=1;
        }
    }

    //give piece landing spot correct piece name
    aiClick->pieceName = " ";
    aiClick1->pieceName = boardArr[tempy2][tempx2];

    //display piece having moved
    aiClick->display(aiClick->pieceName);
    aiClick1->display(aiClick1->pieceName);

    //make sure tile color is correct
    aiClick->tileDisplay();
    aiClick1->tileDisplay();

    //increment turns, ready next click for user
    turns++;
    count = 0;


    /*
    //FOR TESTING
    for(int k = 0; k < 8; k++){
        for(int j = 0; j < 8; j++){
            std::cout << boardArr[k][j];
        }
        std::cout << std::endl;
    }
    */

}


void Tile::display(std::string elem){

    this->pieceName=elem;

    //white pieces
    if(this->pieceColor == 0 && this->piece == 1){
        if(elem == "P"){
            this->setPixmap(QPixmap(":/Images/pawn_white.svg"));
        } else if(elem == "R"){
            this->setPixmap(QPixmap(":/Images/rook_white.svg"));
        } else if(elem == "N"){
            this->setPixmap(QPixmap(":/Images/knight_white.svg"));
        } else if (elem == "B"){
            this->setPixmap(QPixmap(":/Images/bishop_white.svg"));
        }else if (elem == "K"){
            this->setPixmap(QPixmap(":/Images/king_white.svg"));
        } else if(elem == "Q"){
            this->setPixmap(QPixmap(":/Images/queen_white.svg"));
        }
     //black pieces
    } else if (this->piece == 1){
        if (elem == "p"){
            this->setPixmap(QPixmap(":/Images/pawn_black.svg"));
        } else if(elem == "r"){
            this->setPixmap(QPixmap(":/Images/rook_black.svg"));
        } else if(elem == "n"){
            this->setPixmap(QPixmap(":/Images/knight_black.svg"));
        } else if (elem == "b"){
            this->setPixmap(QPixmap(":/Images/bishop_black.svg"));
        }else if (elem == "k"){
            this->setPixmap(QPixmap(":/Images/king_black.svg"));
        } else if(elem == "q"){
            this->setPixmap(QPixmap(":/Images/queen_black.svg"));
        }
    }else {
        this->clear();
    }
}

void Tile::tileDisplay()
{

    if(this->tileColor)
        this->setStyleSheet("QLabel {background-color: rgb(120, 120, 90);}:hover{background-color: rgb(170,85,127);}");
    else
        this->setStyleSheet("QLabel {background-color: rgb(211, 211, 158);}:hover{background-color: rgb(170,95,127);}");
}

