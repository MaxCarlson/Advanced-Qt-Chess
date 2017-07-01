/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Pieces.h
 * Author: C-60
 *
 * Created on June 10, 2017, 10:50 PM
 */

#ifndef PIECES_H
#define PIECES_H
#include <cstdlib>
#include <iostream>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>

#include <externs.h>




class Pieces {
public:

    Pieces();


    void coordinates(int a, int b, int c, int d);
    bool whichPiece();

    static bool isWhiteKingSafe();
    static bool isBlackKingSafe();



private:

    char alphabet[8] = {'a','b','c','d','e','f','g','h'};
    //int flip[8] = {7, 6, 5, 4, 3, 2, 1, 0};

    //arrays for checking friendly fire
    std::string blackPieces[7] = {"p", "q", "k", "b", "n", "r", " "};
    std::string whitePieces[7] = {"P", "Q", "K", "B", "N", "R", " "};

    //marker incase move is invalid and piece need to be reset
    std::string pieceToBeTaken;

    //rook movements for castling
    bool wLRookMoved = false;
    bool wRRookMoved = false;
    bool bLRookMoved = false;
    bool bRRookMoved = false;

    // kings safe checking + arrays for white and black lines of attack for king

    bool whiteKingSaftey();
    bool blackKingSafety();


    void kingIsUnsafe(std::string p);

    void resetSafetyBoards();

    //Movement safe functions for marking board unsafe
    void upDownLeftRightM(int x, int y, char wb);
    void diagonalM(int x, int y, char wb);
    void knightM(int x, int y, char wb);
    void kingM(int x, int y, char wb);

    bool sanityCheck(int x, int y);

    //small check for king safety to re-use indiscriminate of color
    void whiteOrBlack(int x, int y, char wb);


    //Piece and move coordinates
    bool whiteKingMoved = false;
    bool blackKingMoved = false;
    int x1, y1, x2, y2;

    //white pieces
    bool whitePawn();
    bool whiteRook();
    bool whiteKnight();
    bool whiteBishop();
    bool whiteQueen();
    bool whiteKing();

    //black pieces
    bool blackPawn();
    bool blackRook();
    bool blackKnight();
    bool blackBishop();
    bool blackQueen();
    bool blackKing();
};


#endif /* PIECES_H */
