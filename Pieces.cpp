/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Pieces.cpp
 * Author: C-60
 *
 * Created on June 10, 2017, 10:50 PM
 */

#include "Pieces.h"

//array of chess board
std::string boardArr [8][8]= {
    {"r", "n", "b", "q", "k", "b", "n", "r"},
    {"p", "p", "p", "p", "p", "p", "p", "p",},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {" ", " ", " ", " ", " ", " ", " ", " "},
    {"P", "P", "P", "P", "P", "P", "P", "P"},
    {"R", "N", "B", "Q", "K", "B", "N", "R"},
              };
//king saftey arrays
bool whiteSafe[8][8];
bool blackSafe[8][8];

Pieces::Pieces() {

}

void Pieces::coordinates(int a, int b, int c, int d){
    x1 = a;
    y1 = b;
    x2 = c;
    y2 = d;
}

bool Pieces::whichPiece(){
    //friendly check
    if(turns%2==0){
        for(int i = 0; i < 6; i++){
            if(boardArr[y2][x2] == whitePieces[i]){
                return false;
                pieceToBeTaken = whitePieces[i];
            }
        }
    } else if(turns % 2 == 1){
        for(int i = 0; i < 6; i++){
            if(boardArr[y2][x2] == blackPieces[i]){
                return false;
                pieceToBeTaken = blackPieces[i];
            }
        }
    }

    //Don't move opponent piece check
    if(turns % 2 == 0){
        for(int i = 0; i < 6; i++){
            if(boardArr[y1][x1] == blackPieces[i]){
                return false;
            }
        }
    } else if (turns % 2 == 1){
        for(int i = 0; i < 6; i++){
            if(boardArr[y1][x1] == whitePieces[i]){
                return false;
            }
        }
    }

    resetSafetyBoards();

    //Set king safety boards
    whiteKingSaftey();
    blackKingSafety();

    //find which piece to move
    //marker for piece being moved
    std::string pieceMoved;
    //white pieces
    if(turns % 2 == 0){
        if(boardArr[y1][x1] == "P"){
            pieceToBeTaken = boardArr[y2][x2];
            if(whitePawn() == false){
                return false;
            }
            pieceMoved = "P";
        }else if(boardArr[y1][x1] == "R"){
            pieceToBeTaken = boardArr[y2][x2];
            if(whiteRook() == false){
                return false;
            }
            pieceMoved = "R";
        }else if(boardArr[y1][x1] == "N"){
            pieceToBeTaken = boardArr[y2][x2];
            if(whiteKnight() == false){
                return false;
            }
            pieceMoved = "N";
        } else if(boardArr[y1][x1] == "B"){
            pieceToBeTaken = boardArr[y2][x2];
            if(whiteBishop() == false){
                return false;
            }
            pieceMoved = "B";
        } else if(boardArr[y1][x1] == "Q"){
            pieceToBeTaken = boardArr[y2][x2];
            if(whiteQueen() == false){
                return false;
            }
            pieceMoved = "Q";
        }else if(boardArr[y1][x1] == "K"){
            pieceToBeTaken = boardArr[y2][x2];
            if(whiteKing() == false){
                return false;
            }
            pieceMoved = "K";
        }

        resetSafetyBoards();
        //find out if moving piece makes king unsafe
        //and move piece back if it does
        whiteKingSaftey();
        if(isWhiteKingSafe() == false){
            kingIsUnsafe(pieceMoved);
            return false;
        }
        //king and rook move checks for castling
        if(pieceMoved == "K"){
            whiteKingMoved = true;
        } else if (pieceMoved == "R"){
            if(x1 == 0 && y1 == 7){
                wRRookMoved = true;
            } else if (x1 == 7 && y1 == 7){
                wLRookMoved = true;
            }
        }
        return true;
    //black pieces
    }else if (turns % 2 == 1) {
        if(boardArr[y1][x1] == "p"){
            pieceToBeTaken = boardArr[y2][x2];
            if(blackPawn() == false){
                return false;
            }
            pieceMoved = "p";
        }else if(boardArr[y1][x1] == "r"){
            pieceToBeTaken = boardArr[y2][x2];
            if(blackRook() == false){
                return false;
            }
            pieceMoved = "r";
        } else if(boardArr[y1][x1] == "n"){
            pieceToBeTaken = boardArr[y2][x2];
            if(blackKnight() == false){
                return false;
            }
            pieceMoved = "n";
        }else if(boardArr[y1][x1] == "b"){
            pieceToBeTaken = boardArr[y2][x2];
            if(blackBishop() == false){
                return false;
            }
            pieceMoved = "b";
        }else if(boardArr[y1][x1] == "q"){
            pieceToBeTaken = boardArr[y2][x2];
            if (blackQueen() == false){
                return false;
            }
            pieceMoved = "q";
        }else if(boardArr[y1][x1] == "k"){
            pieceToBeTaken = boardArr[y2][x2];
            if(blackKing() == false){
                return false;
            }
            pieceMoved = "k";
        }
        resetSafetyBoards();
        //find out if moving piece makes king unsafe
        //and move piece back if it does
        blackKingSafety();
        if(isBlackKingSafe() == false){
            kingIsUnsafe(pieceMoved);
            return false;
        }
        //king and castle move checks for castling
        if(pieceMoved == "k"){
            blackKingMoved = true;
        } else if (pieceMoved == "r"){
            if(x1 == 0 && y1 == 0){
                bLRookMoved = true;
            } else if (x1 == 7 && y1 == 0){
                bLRookMoved = true;
            }
        }
        return true;
    }

    return false;
}
//NEED add castling
//Need En Passant
void Pieces::resetSafetyBoards(){
    //setting king safety arrays to all true
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            whiteSafe[i][j] = true;
            blackSafe[i][j] = true;
        }
    }
}


bool Pieces::isWhiteKingSafe(){
    int x, y;
    //find king
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(boardArr[i][j] == "K"){
                x = j;
                y = i;
            }
        }
    }
    //find if king position is safe
    if(whiteSafe[y][x] == false){
        return false;
    }
    return true;
}

bool Pieces::isBlackKingSafe(){
    int x, y;
    //find king
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(boardArr[i][j] == "k"){
                x = j;
                y = i;
            }
        }
    }
    //find if king position is safe
    if(blackSafe[y][x] == false){
        return false;
    }
    return true;
}
void Pieces::kingIsUnsafe(std::string p){
    boardArr[y2][x2] = pieceToBeTaken;
    boardArr[y1][x1] = p;
    std::cout << "That move puts you into check! Try again." << std::endl;
}

//checking all squares to see where king can move or not
bool Pieces::whiteKingSaftey(){
//denoting we're marking whites un-safe areas
    char w = 'w';
    //array loop to mark board of unsafe areas
    for(int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if(boardArr[i][j] == "p"){
                //pawn sanity checks
                if(i < 7 && j < 7){
                    whiteSafe[i+1][j+1] = false;
                }
                if (i < 7 && j > 0){
                    whiteSafe[i+1][j-1] = false;
                }
            }else if(boardArr[i][j] == "r"){
                upDownLeftRightM(j, i, w);
            }else if(boardArr[i][j] == "n"){
                knightM(j, i, w);
            } else if(boardArr[i][j] == "b"){
                diagonalM(j, i, w);
            } else if (boardArr[i][j] == "q"){
                diagonalM(j, i, w);
                upDownLeftRightM(j, i, w);
            }else if (boardArr[i][j] == "k"){
                kingM(j, i, w);
            }
        }
    }

}



bool Pieces::blackKingSafety(){
//denoting we're marking blacks un-safe areas
    char w = 'b';
    //array loop to mark board of unsafe areas
    for(int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if(boardArr[i][j] == "P"){
                //pawn sanity checks
                if(i > 0 && j<7){
                    blackSafe[i-1][j+1] = false;
                }
                if(i > 0 && j > 0){
                    blackSafe[i-1][j-1] = false;
                }
            }else if(boardArr[i][j] == "R"){
                upDownLeftRightM(j, i, w);
            }else if(boardArr[i][j] == "N"){
                knightM(j, i, w);
            } else if(boardArr[i][j] == "B"){
                diagonalM(j, i, w);
            } else if (boardArr[i][j] == "Q"){
                diagonalM(j, i, w);
                upDownLeftRightM(j, i, w);
            }else if (boardArr[i][j] == "K"){
                kingM(j, i, w);
            }
        }
    }
}

//mark where is unsafe and figure out who
void Pieces::whiteOrBlack(int x, int y, char wb){
    //sanity check
    if(x > 7 || x < 0){
        return;
    } else if (y > 7 || y < 0){
        return;
    }
    if(wb == 'w'){
        whiteSafe[y][x] = false;
    } else if (wb == 'b'){
        blackSafe[y][x] = false;
    }
}

bool Pieces::sanityCheck(int x, int y){
    if(x > 7 || x < 0){
        return false;
    }else if(y > 7 || y < 0){
        return false;
    }
    return true;
}

void Pieces::upDownLeftRightM(int x, int y, char wb){
    //mark up unsafe
    for(int i = y-1; i > -1; i--){
        //sanity checks
        if(sanityCheck(x, i) == true){
            if(boardArr[i][x] != " "){
                whiteOrBlack(x,i,wb);
                break;
            }
            whiteOrBlack(x,i,wb);
        }

    }
    //mark down
    for(int i = y+1; i < 8; i++){
        if(sanityCheck(x, i) == true){
            if(boardArr[i][x] != " "){
                whiteOrBlack(x,i,wb);
                break;
            }
            whiteOrBlack(x,i,wb);
        }

    }

    //mark right unsafe
    for(int i = x+1; i < 8; i++){
        if(sanityCheck(i, y) == true){
            if(boardArr[y][i] != " "){
                whiteOrBlack(i,y,wb);
                break;
            }
            whiteOrBlack(i,y,wb);
        }

    }

    //mark left
    for(int i = x-1; i > -1; i--){
        if(sanityCheck(i, y) == true){
            if(boardArr[y][i] != " "){
                whiteOrBlack(i,y,wb);
                break;
            }
            whiteOrBlack(i,y,wb);
        }

    }
}
void Pieces::diagonalM(int x, int y, char wb){
    int j = y-1;
    //up right safety check
    for(int i = x+1; i < 8; i++){
        //sanity checks
        if(sanityCheck(i, j) == true){
            if(boardArr[j][i] != " "){
                whiteOrBlack(i,j,wb);
                break;
            }
            whiteOrBlack(i,j,wb);
        }
        j --;
    }

    //down right
    j = y+1;
    for(int i = x+1; i < 8; i++){
        if(sanityCheck(i, j) == true){
            if(boardArr[j][i] != " "){
                whiteOrBlack(i,j,wb);
                break;
            }
            whiteOrBlack(i,j,wb);
        }
        j ++;
    }

    //down left
    j = y+1;
    for(int i = x-1; i > -1; i--){
        if(sanityCheck(i, j) == true){
            if(boardArr[j][i] != " "){
                whiteOrBlack(i,j,wb);
                break;
            }
            whiteOrBlack(i,j,wb);
        }
        j ++;
    }

    //up left
    j = y-1;
    for(int i = x-1; i > -1; i--){
        if(sanityCheck(i, j) == true){
            if(boardArr[j][i] != " "){
                whiteOrBlack(i,j,wb);
                break;
            }
            whiteOrBlack(i,j,wb);
        }
        j --;
    }
}

void Pieces::knightM(int x, int y, char wb){
    //counter clock-wise all knight jumps
    //Up right
    whiteOrBlack(x-2,y-1,wb);
    whiteOrBlack(x-1,y-2,wb);
    //Down right
    whiteOrBlack(x-2,y+1,wb);
    whiteOrBlack(x-1,y+2,wb);
    //Down left
    whiteOrBlack(x+1,y+2,wb);
    whiteOrBlack(x+2,y+1,wb);
    //Up left
    whiteOrBlack(x+2,y-1,wb);
    whiteOrBlack(x+1,y-2,wb);
}

void Pieces::kingM(int x, int y, char wb){
    //clock-wise starting at 12
    //king attack safety marking
    whiteOrBlack(x,y-1,wb);
    whiteOrBlack(x+1,y-1,wb);
    whiteOrBlack(x+1,y,wb);
    whiteOrBlack(x+1,y+1,wb);
    whiteOrBlack(x,y+1,wb);
    whiteOrBlack(x-1,y+1,wb);
    whiteOrBlack(x-1,y,wb);
    whiteOrBlack(x-1,y-1,wb);
}

// Test if moves are valid + move piece + take piece
bool Pieces::whiteKing(){
    //king is safe after move check
    //up
    if(y1-y2 == 1 && x1 == x2){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == blackPieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "K";
                return true;
            }
        }

    //down
    } else if(y2-y1 == 1 && x1 == x2){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == blackPieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "K";
                return true;
            }
        }

    //right
    } else if(y1 == y2 && x2-x1 == 1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == blackPieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "K";
                return true;
            }
        }

    //left
    } else if(y1 == y2 && x1-x2 == 1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == blackPieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "K";
                return true;
            }
        }
    //up left
    }else if(y1-y2 == 1 && x1-x2 == 1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == blackPieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "K";
                return true;
            }
        }
    //up right
    } else if(y1-y2 == 1 && x1-x2 == -1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == blackPieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "K";
                return true;
            }
        }
    //down left
    } else if (y1-y2 == -1 && x1-x2 == 1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == blackPieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "K";
                return true;
            }
        }
    //down right
    } else if (y1-y2 == -1 && x1-x2 == -1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == blackPieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "K";
                return true;
            }
        }
    }

    //castle and king start coordinates match
    if(whiteKingMoved == false && x1 == 4 && y1 == 7){
        //if left rook and land coordinates match and sqaures between are empty
        if(wLRookMoved == false && x2 == 2 && y2 == 7 && boardArr[y1][x1-1] == " " && boardArr[y1][x1-2] == " " && boardArr[y1][x1-3] == " "){

            //not moving through enemy lines or into check
            if(whiteSafe[y1][x1] == true && whiteSafe[y1][x1-1] == true && whiteSafe[y1][x1-2] == true){
                boardArr[y1][x1-2] = "K";
                boardArr[y1][x1-1] = "R";
                boardArr[y1][x1-4] = " ";
                boardArr[y1][x1] = " ";
                wLRookMoved = true;
                numberOfPieceMoved = 2;
                return true;
            }
        //if right rook and king start coordinates match and sqaures between are empty
        } else if (wRRookMoved == false && x2 == 6 && y2 == 7 && boardArr[y1][x1+1] == " " && boardArr[y1][x1+2] == " "){

            //not moving through enemy lines or into check
            if(whiteSafe[y1][x1] == true && whiteSafe[y1][x1+1] == true && whiteSafe[y1][x1+1] == true){
                boardArr[y1][x1+2] = "K";
                boardArr[y1][x1+1] = "R";
                boardArr[y1][x1+4] = " ";
                boardArr[y1][x1] = " ";
                wLRookMoved = true;
                numberOfPieceMoved = 2;
                return true;
            }
        }
    }

    return false;
}

bool Pieces::blackKing(){
    //king is safe after move check
    //up
    if(y1-y2 == 1 && x1 == x2){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == whitePieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "k";
                return true;
            }
        }

    //down
    } else if(y2-y1 == 1 && x1 == x2){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == whitePieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "k";
                return true;
            }
        }

    //right
    } else if(y1 == y2 && x2-x1 == 1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == whitePieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "k";
                return true;
            }
        }

    //left
    } else if(y1 == y2 && x1-x2 == 1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == whitePieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "k";
                return true;
            }
        }
    //up left
    }else if(y1-y2 == 1 && x1-x2 == 1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == whitePieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "k";
                return true;
            }
        }
    //up right
    } else if(y1-y2 == 1 && x1-x2 == -1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == whitePieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "k";
                return true;
            }
        }
    //down left
    } else if (y1-y2 == -1 && x1-x2 == 1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == whitePieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "k";
                return true;
            }
        }
    //down right
    } else if (y1-y2 == -1 && x1-x2 == -1){
        for(int i = 0; i < 7; i++){
            if(boardArr[y2][x2] == whitePieces[i]){
                boardArr[y1][x1] = " ";
                boardArr[y2][x2] = "k";
                return true;
            }
        }
    }

    //castle and king start coordinates match
    if(blackKingMoved == false && x1 == 4 && y1 == 0){
        //if left rook and land coordinates match and sqaures between are empty
        if(bLRookMoved == false && x2 == 2 && y2 == 0 && boardArr[y1][x1-1] == " " && boardArr[y1][x1-2] == " " && boardArr[y1][x1-3] == " "){

            //not moving through enemy lines or into check
            if(blackSafe[y1][x1] == true && blackSafe[y1][x1-1] == true && blackSafe[y1][x1-2] == true){
                boardArr[y1][x1-2] = "k";
                boardArr[y1][x1-1] = "r";
                boardArr[y1][x1-4] = " ";
                boardArr[y1][x1] = " ";
                bLRookMoved = true;
                numberOfPieceMoved = 2;
                return true;
            }
        //if right rook and king start coordinates match and sqaures between are empty
        } else if (bRRookMoved == false && x2 == 6 && y2 == 0 && boardArr[y1][x1+1] == " " && boardArr[y1][x1+2] == " "){

            //not moving through enemy lines or into check
            if(blackSafe[y1][x1] == true && blackSafe[y1][x1+1] == true && blackSafe[y1][x1+1] == true){
                boardArr[y1][x1+2] = "k";
                boardArr[y1][x1+1] = "r";
                boardArr[y1][x1+4] = " ";
                boardArr[y1][x1] = " ";
                bLRookMoved = true;
                numberOfPieceMoved = 2;
                return true;
            }
        }
    }

    return false;
}

bool Pieces::whiteQueen(){
    //only perfect diagonal check
    int diag = abs(x1-x2);
    int diag1 = abs(y1-y2);

    if(diag == diag1){
        //up right
        if(y1-y2 > 0 && x1-x2 < 0){
            int j = x1+1;
            for(int i = y1-1; i >= y2+1; i--){
                if(boardArr[i][j] != " "){
                    return false;
                }
                j ++;
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "Q";
            return true;

        //up left
        } else if(y1-y2 > 0 && x1-x2 > 0){
            int j = x1-1;
            for(int i = y1-1; i >= y2+1; i--){
                if(boardArr[i][j] != " "){
                    return false;
                }
                j --;
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "Q";
            return true;

        //down left
        }else if(y1-y2 < 0 && x1-x2 > 0){
            int j = x1-1;
            for(int i = y1+1; i <= y2-1; i++){
                if(boardArr[i][j] != " "){
                    return false;
                }
                j --;
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "Q";
            return true;

        //down right
        } else if(y1-y2 < 0 && x1-x2 < 0){
            int j = x1+1;
            for(int i = y1+1; i <= y2-1; i++){
                if(boardArr[i][j] != " "){
                    return false;
                }
                j ++;
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "Q"
                    "";
            return true;
        }
    } else {
        int distance;

        //going up
        if(x1 == x2 && y1 != y2){
            distance = y1-y2;

            if(distance > 0){
                for(int i = y1-1; i >= y2+1; --i){
                    if(boardArr[i][x1] != " "){
                        return false;
                    }
                }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "Q";
            return true;
        //going down
        }else if(distance < 0){
            for(int i = y1+1; i <= y2-1; i++){
                if(boardArr[i][x1] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "Q";
            return true;
        }

        //Horizontal test
        } else if (x1 != x2 && y1 == y2){
            distance = x1-x2;
            //going left
            if(distance > 0){
                for(int i = x1-1; i >= x2+1; --i){
                    if(boardArr[y1][i] != " "){
                        return false;
                    }
                }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "Q";
            return true;

            //going right
        }else if (distance < 0){
            for(int i = x1+1; i <= x2-1; ++i){
                if(boardArr[y1][i] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "Q";
            return true;
            }
        }

    }

return false;

}

bool Pieces::blackQueen(){
    //only perfect diagonal check
    int diag = abs(x1-x2);
    int diag1 = abs(y1-y2);

    if(diag == diag1){
        //up right
        if(y1-y2 > 0 && x1-x2 < 0){
            int j = x1+1;
            for(int i = y1-1; i >= y2+1; i--){
                if(boardArr[i][j] != " "){
                    return false;
                }
                j ++;
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "q";
            return true;

        //up left
        } else if(y1-y2 > 0 && x1-x2 > 0){
            int j = x1-1;
            for(int i = y1-1; i >= y2+1; i--){
                if(boardArr[i][j] != " "){
                    return false;
                }
                j --;
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "q";
            return true;

        //down left
        }else if(y1-y2 < 0 && x1-x2 > 0){
            int j = x1-1;
            for(int i = y1+1; i <= y2-1; i++){
                if(boardArr[i][j] != " "){
                    return false;
                }
                j --;
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "q";
            return true;

        //down right
        } else if(y1-y2 < 0 && x1-x2 < 0){
            int j = x1+1;
            for(int i = y1+1; i <= y2-1; i++){
                if(boardArr[i][j] != " "){
                    return false;
                }
                j ++;
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "q"
                    "";
            return true;
        }
    } else {
        int distance;

        //going up
        if(x1 == x2 && y1 != y2){
            distance = y1-y2;

            if(distance > 0){
                for(int i = y1-1; i >= y2+1; --i){
                    if(boardArr[i][x1] != " "){
                        return false;
                    }
                }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "q";
            return true;
        //going down
        }else if(distance < 0){
            for(int i = y1+1; i <= y2-1; i++){
                if(boardArr[i][x1] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "q";
            return true;
        }

        //Horizontal test
        } else if (x1 != x2 && y1 == y2){
            distance = x1-x2;
            //going left
            if(distance > 0){
                for(int i = x1-1; i >= x2+1; --i){
                    if(boardArr[y1][i] != " "){
                        return false;
                    }
                }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "q";
            return true;

            //going right
        }else if (distance < 0){
            for(int i = x1+1; i <= x2-1; ++i){
                if(boardArr[y1][i] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "q";
            return true;
            }
        }

    }

return false;

}



bool Pieces::whiteBishop(){
    //only perfect diagonal check
    int diag = abs(x1-x2);
    int diag1 = abs(y1-y2);

    if(diag != diag1){
        return false;
    }

    //up right
    if(y1-y2 > 0 && x1-x2 < 0){
        int j = x1+1;
        for(int i = y1-1; i >= y2+1; i--){
            if(boardArr[i][j] != " "){
                return false;
            }
            j ++;
        }
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "B";
        return true;

    //up left
    } else if(y1-y2 > 0 && x1-x2 > 0){
        int j = x1-1;
        for(int i = y1-1; i >= y2+1; i--){
            if(boardArr[i][j] != " "){
                return false;
            }
            j --;
        }
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "B";
        return true;

    //down left
    }else if(y1-y2 < 0 && x1-x2 > 0){
        int j = x1-1;
        for(int i = y1+1; i <= y2-1; i++){
            if(boardArr[i][j] != " "){
                return false;
            }
            j --;
        }
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "B";
        return true;

    //down right
    } else if(y1-y2 < 0 && x1-x2 < 0){
        int j = x1+1;
        for(int i = y1+1; i <= y2-1; i++){
            if(boardArr[i][j] != " "){
                return false;
            }
            j ++;
        }
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "B";
        return true;
    }

    return false;
}

bool Pieces::blackBishop(){
    //only perfect diagonal check
    int diag = abs(x1-x2);
    int diag1 = abs(y1-y2);

    if(diag != diag1){
        return false;
    }

    //up right
    if(y1-y2 > 0 && x1-x2 < 0){
        int j = x1+1;
        for(int i = y1-1; i >= y2+1; i--){
            if(boardArr[i][j] != " "){
                return false;
            }
            j ++;
        }
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "b";
        return true;

    //up left
    } else if(y1-y2 > 0 && x1-x2 > 0){
        int j = x1-1;
        for(int i = y1-1; i >= y2+1; i--){
            if(boardArr[i][j] != " "){
                return false;
            }
            j --;
        }
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "b";
        return true;

    //down left
    }else if(y1-y2 < 0 && x1-x2 > 0){
        int j = x1-1;
        for(int i = y1+1; i <= y2-1; i++){
            if(boardArr[i][j] != " "){
                return false;
            }
            j --;
        }
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "b";
        return true;

    //down right
    } else if(y1-y2 < 0 && x1-x2 < 0){
        int j = x1+1;
        for(int i = y1+1; i <= y2-1; i++){
            if(boardArr[i][j] != " "){
                return false;
            }
            j ++;
        }
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "b";
        return true;
    }

    return false;
}

bool Pieces::whiteKnight(){
    //up left
    if(x1-2 == x2 && y1-1 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "N";
        return true;
    } else if(x1-1 == x2 && y1-2 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "N";
        return true;
    }
    //up right
    if(x1+2 == x2 && y1-1 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "N";
        return true;
    } else if(x1+1 == x2 && y1-2 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "N";
        return true;
    }
    //down left
    if(x1-2 == x2 && y1+1 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "N";
        return true;
    } else if(x1-1 == x2 && y1+2 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "N";
        return true;
    }
    //down right
    if(x1+2 == x2 && y1+1 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "N";
        return true;
    } else if(x1+1 == x2 && y1+2 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "N";
        return true;
    }
    return false;
}

bool Pieces::blackKnight(){
    //up left
    if(x1-2 == x2 && y1-1 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "n";
        return true;
    } else if(x1-1 == x2 && y1-2 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "n";
        return true;
    }
    //up right
    if(x1+2 == x2 && y1-1 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "n";
        return true;
    } else if(x1+1 == x2 && y1-2 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "n";
        return true;
    }
    //down left
    if(x1-2 == x2 && y1+1 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "n";
        return true;
    } else if(x1-1 == x2 && y1+2 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "n";
        return true;
    }
    //down right
    if(x1+2 == x2 && y1+1 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "n";
        return true;
    } else if(x1+1 == x2 && y1+2 == y2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "n";
        return true;
    }
    return false;
}


bool Pieces::whiteRook(){
    int distance;
    bool pieceOn = false;
    //vertical only test
    if(x1 == x2 && y1 != y2){
        distance = y1-y2;
        //going up
        if(distance > 0){
            for(int i = y1-1; i >= y2+1; --i){
                if(boardArr[i][x1] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "R";
            return true;
        //going down
        }else if(distance < 0){
            for(int i = y1+1; i <= y2-1; i++){
                if(boardArr[i][x1] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "R";
            return true;
        }

    //Horizontal test
    } else if (x1 != x2 && y1 == y2){
        distance = x1-x2;
        //going left
        if(distance > 0){
            for(int i = x1-1; i >= x2+1; --i){
                if(boardArr[y1][i] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "R";
            return true;

        //going right
        }else if (distance < 0){
            for(int i = x1+1; i <= x2-1; ++i){
                if(boardArr[y1][i] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "R";
            return true;
        }
    }

    return false;
}

bool Pieces::blackRook(){
    int distance;
    //bool pieceOn = false;
    //vertical only test
    if(x1 == x2 && y1 != y2){
        distance = y1-y2;
        //going up
        if(distance > 0){
            for(int i = y1-1; i >= y2+1; --i){
                if(boardArr[i][x1] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "r";
            return true;
        //going down
        }else if(distance < 0){
            for(int i = y1+1; i <= y2-1; i++){
                if(boardArr[i][x1] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "r";
            return true;
        }

    //Horizontal test
    } else if (x1 != x2 && y1 == y2){
        distance = x1-x2;
        //going left
        if(distance > 0){
            for(int i = x1-1; i >= x2+1; --i){
                if(boardArr[y1][i] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "r";
            return true;

        //going right
        }else if (distance < 0){
            for(int i = x1+1; i <= x2-1; ++i){
                if(boardArr[y1][i] != " "){
                    return false;
                }
            }
            boardArr[y1][x1] = " ";
            boardArr[y2][x2] = "r";
            return true;
        }
    }

    return false;
}

bool Pieces::whitePawn(){
    //pawn first turn
    if(boardArr[y2][x2] == " " && y1-y2 <= 2 && y1-y2 > 0 && x1 == x2 && y1 == 6){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "P";
        return true;
    //pawn move forward
    } else if(boardArr[y2][x2] == " " && y1-y2 == 1 && x1 == x2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "P";
        return true;
    //pawn takes piece diagonal
    } else if(boardArr[y2][x2] != " " && y1-y2 == 1 && x1-x2 != 0){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "P";
        return true;
    //Need turn to queen + others
    }

    return false;
}

bool Pieces::blackPawn(){
    //pawn first turn
    if(boardArr[y2][x2] == " " && y2-y1 <= 2 && y2-y1 > 0 && x1 == x2 && y1 == 1){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "p";
        return true;
    //pawn move forward
    } else if(boardArr[y2][x2] == " " && y2-y1 == 1 && x1 == x2){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "p";
        return true;
    //pawn takes piece diagonal
    } else if(boardArr[y2][x2] != " " && y2-y1 == 1 && x1-x2 != 0){
        boardArr[y1][x1] = " ";
        boardArr[y2][x2] = "p";
        return true;
    //Need turn to queen  + others
    }
    return false;
}
