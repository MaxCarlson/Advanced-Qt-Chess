#include <QApplication>
#include <QCoreApplication>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QPushButton>
#include <tile.h>
#include <externs.h>
#include <iostream>

//create an object array to to represent chess board tiles
Tile * rect[8][8];

// number of half turns
int turns = 0;

//marker for whether its "click 0" or 1
int count = 0;

int numberOfPieceMoved = 0;

int tempx, tempy, tempx2, tempy2;

//whether ai is on  //FUTURE and which side it's playing
int aiOn = 1;


//color piece arrays

std::string blackPieces[7] = {"p", "q", "k", "b", "n", "r", " "};
std::string whitePieces[7] = {"P", "Q", "K", "B", "N", "R", " "};

void accessories(QWidget *baseWidget)
{
    QLabel *player2 = new QLabel(baseWidget);
    QLabel *name2 = new QLabel("Player 2", baseWidget);
    QLabel *time2 = new QLabel("00:00:00", baseWidget);

    QLabel *player1 = new QLabel(baseWidget);
    QLabel *name1 = new QLabel("Player 1", baseWidget);
    QLabel *time1 = new QLabel("00:00:00", baseWidget);

    QLabel *moves = new QLabel(baseWidget);



    name1->setGeometry(125,610,80,20);
    time1->setGeometry(120,635,80,20);
    player1->setGeometry(100,500,100,100);
    player1->setPixmap(QPixmap(":/Images/profile.png"));

    name2->setGeometry(125,210,80,20);
    time2->setGeometry(120,235,80,20);
    player2->setGeometry(100,100,100,100);
    player2->setPixmap(QPixmap(":/Images/profile.png"));

    moves->setGeometry(1000,105,250,550);
    moves->setStyleSheet("QLabel {background-color: white;}");

}

//attempt at new game button, haven't linked it yet
void buttons(QWidget *baseWidget){
    QPushButton *newGame = new QPushButton("New Game",baseWidget);

    newGame->setGeometry(1000,665,75,35);

    QObject::connect(newGame, SIGNAL (clicked()), baseWidget, SLOT (handleNewGame(newGame)));
}

void handleNewGame(QPushButton *newGame){
    newGame->setGeometry(5,665,5,35);
}


class Border
{
public:
    Border();
    void outline(QWidget *baseWidget, int xPos, int yPos, int Pos)
    {
         QLabel *outLabel = new QLabel(baseWidget);

        if(!Pos)
            outLabel->setGeometry(xPos,yPos,552,20);        //Horizontal Borders

        else
            outLabel->setGeometry(xPos,yPos,20,512);        //Vertical Borders

        outLabel->setStyleSheet("QLabel { background-color :rgb(170, 170, 127); color : black; }");
    }
};


void chessBoard(QWidget *baseWidget, Tile *rect[8][8]){

    //set chessboard border
    Border *border[4]={ NULL };
    {
    border[0]->outline(baseWidget,330,105,0);
    border[1]->outline(baseWidget,330,637,0);
    border[2]->outline(baseWidget,330,125,1);
    border[2]->outline(baseWidget,862,125,1);
    }

    int boardX = 350, boardY = 125, k = 0;
    //seed Qt 64 tile chess board - colored empty tiles
    for(int i = 0; i < 8; i++){
        boardX = 350;
        for(int j = 0; j < 8; j++){
            //seed tile array with tile objects
            rect[i][j] = new Tile(baseWidget);
            rect[i][j]->tileColor=(i+j)%2;
            rect[i][j]->piece=0;
            rect[i][j]->row=i;
            rect[i][j]->col=j;
            rect[i][j]->tileNum=k++;
            rect[i][j]->tileDisplay();
            rect[i][j]->setGeometry(boardX,boardY,64,64);


            //rect[i][j]->setScaledContents(true);

            boardX += 64;
        }
        boardY += 64;
    }


    //Load all pieces into correct starting spots
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(boardArr[j][i] != " "){
                rect[j][i]->piece=1;
                for(int k = 0; k < 8; k++){
                    if(boardArr[j][i] == whitePieces[k]){
                        rect[j][i]->pieceColor=0;
                        rect[j][i]->display(whitePieces[k]);
                    } else if (boardArr[j][i] == blackPieces[k]){
                        rect[j][i]->pieceColor=1;
                        rect[j][i]->display(blackPieces[k]);
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    //create widget + set  intial size
    QWidget *myWidget = new QWidget();
    myWidget->setGeometry(0,0,1370,700);
    //myWidget->setSizeIncrement(500, 500);

    //buttons and other stylings
    buttons(myWidget);
    accessories(myWidget);



    //create board
    chessBoard(myWidget, rect);

    //show the board
    myWidget->show();

    return a.exec();
}
