#include "bitboards.h"
#include <cmath>
#include <random>
#include <iostream>
#include <cstdio>
#include "externs.h"
#include "zobristh.h"
//#include "move.h"
class Move;

BitBoards::BitBoards()
{

}



void BitBoards::constructBoards()
{
    FullTiles = 0LL;

    BBWhitePawns = 0LL;
    BBWhitePieces = 0LL;
    BBWhiteRooks = 0LL;
    BBWhiteKnights = 0LL;
    BBWhiteBishops = 0LL;
    BBWhiteQueens = 0LL;
    BBWhiteKing = 0LL;

    BBBlackPieces = 0LL;
    BBBlackPawns = 0LL;
    BBBlackRooks = 0LL;
    BBBlackKnights = 0LL;
    BBBlackBishops = 0LL;
    BBBlackQueens = 0LL;
    BBBlackKing = 0LL;

    //seed bitboards
    for(int i = 0; i < 64; i++){
        if(boardArr[i/8][i%8] == "P"){
            BBWhitePawns += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        } else if(boardArr[i/8][i%8] == "R"){
            BBWhiteRooks += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "N"){
            BBWhiteKnights += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "B"){
            BBWhiteBishops += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "Q"){
            BBWhiteQueens += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "K"){
            BBWhiteKing += 1LL<<i;
            BBWhitePieces += 1LL<<i;
            FullTiles += 1LL<<i;
        } else if(boardArr[i/8][i%8] == "p"){
            BBBlackPawns += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        } else if(boardArr[i/8][i%8] == "r"){
            BBBlackRooks += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "n"){
            BBBlackKnights += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "b"){
            BBBlackBishops += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "q"){
            BBBlackQueens += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }else if(boardArr[i/8][i%8] == "k"){
            BBBlackKing += 1LL<<i;
            BBBlackPieces += 1LL<<i;
            FullTiles += 1LL<<i;
        }
    }

    //mark empty tiles with 1's
    EmptyTiles = ~FullTiles;

}

//normal move stuff
void BitBoards::makeMove(Move move, ZobristH *zobrist, bool isWhite)
{
    std::string savedMove;
    int xyI, xyE;
    //inital spot piece mask and end spot mask
    U64 pieceMaskI = 0LL, pieceMaskE = 0LL;
    //for normal moves

    xyI = move.y * 8 + move.x;
    xyE = move.y1 * 8 + move.x1;

    pieceMaskI += 1LL<< xyI;
    pieceMaskE += 1LL << xyE;

    //store coordiantes for undoing move
    //final order is x, y, x1, y1, piece moved, piece captured (0 if none)
    savedMove += move.x;
    savedMove += move.y;
    savedMove += move.x1;
    savedMove += move.y1;

    //find BB that contains correct piece, remove piece from it's starting pos
    //on piece BB, add piece to string savedMove, if it's a capture add piece to be captured,

    //white pieces
    if(isWhite == true){
        switch(move.piece){
        case 'P':
            //remove piece from starting loc
            BBWhitePawns &= ~pieceMaskI;
            //remove piece from color BB
            BBWhitePieces &= ~pieceMaskI;
            //remove piece from full tiles
            FullTiles &= ~pieceMaskI;
            //adds piece to move to be returned in order to undo move
            savedMove += "P";
            //removes piece from capture location if capture and returns piece char
            savedMove += move.captured;

            if(move.flag != 'Q'){
                //add piece to landing spot
                BBWhitePawns |= pieceMaskE;

            //if it's a pawn promotion
            } else {
                //add queen to landing spot
                BBWhiteQueens |= pieceMaskE;
                //add promotion data to capture string
                savedMove += "O";
            }
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'R':
            //remove piece, test/save capture
            BBWhiteRooks &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "R";
            savedMove += move.captured;
            //add piece
            BBWhiteRooks |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'N':
            BBWhiteKnights &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "N";
            savedMove += move.captured;
            //add piece
            BBWhiteKnights |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'B':
            BBWhiteBishops &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "B";
            savedMove += move.captured;
            //add piece
            BBWhiteBishops |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'Q':
            BBWhiteQueens &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "Q";
            savedMove += move.captured;
            //add piece
            BBWhiteQueens |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'K':
            BBWhiteKing &= ~pieceMaskI;
            BBWhitePieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "K";
            savedMove += move.captured;
            //add piece
            BBWhiteKing |= pieceMaskE;
            //add to color pieces then full tiles
            BBWhitePieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;
        }
    //black pieces
    } else {
        switch(move.piece){
        case 'p':
            BBBlackPawns &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "p";
            savedMove += move.captured;

            if(move.flag != 'Q'){
                //add piece to landing spot
                BBBlackPawns |= pieceMaskE;

            //if it's a pawn promotion
            } else {
                //add queen to landing spot
                BBBlackQueens |= pieceMaskE;
                //add promotion data to capture string
                savedMove += "O";
            }
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'r':
            BBBlackRooks &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "r";
            savedMove += move.captured;
            //add piece
            BBBlackRooks |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'n':
            BBBlackKnights &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "n";
            savedMove += move.captured;
            //add piece
            BBBlackKnights |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'b':
            BBBlackBishops &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "b";
            savedMove += move.captured;
            //add piece
            BBBlackBishops |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'q':
            BBBlackQueens &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "q";
            savedMove += move.captured;
            //add piece
            BBBlackQueens |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;

        case 'k':
            BBBlackKing &= ~pieceMaskI;
            BBBlackPieces &= ~pieceMaskI;
            FullTiles &= ~pieceMaskI;
            savedMove += "k";
            savedMove += move.captured;
            //add piece
            BBBlackKing |= pieceMaskE;
            //add to color pieces then full tiles
            BBBlackPieces |= pieceMaskE;
            FullTiles |= pieceMaskE;
            break;
        }
    }

    //remove captured piece from BB's
    if(move.captured != '0') removeCapturedPiece(move.captured, pieceMaskE);


    //correct empty tiles to opposite of full tiles
    EmptyTiles &= ~pieceMaskE;
    EmptyTiles |= pieceMaskI;
    //EmptyTiles &= ~FullTiles
    //drawBBA();


    if((EmptyTiles & BBWhitePieces) | (EmptyTiles & BBBlackPieces)){
        drawBBA();
    }

    //Update zobrist hash
    zobrist->UpdateKey(xyI, xyE, move, isWhite);

    //change zobrist color after a move
    zobrist->UpdateColor();


}

void BitBoards::unmakeMove(Move moveKey, ZobristH *zobrist, bool isWhite)
{

    //parse string move and change to ints
    int x = moveKey.x;
    int y = moveKey.y;
    int x1 = moveKey.x1;
    int y1 = moveKey.y1;
    int xyI = y*8+x, xyE = y1*8+x1;
    //inital spot piece mask and end spot mask
    U64 pieceMaskI = 0LL, pieceMaskE = 0LL;
    pieceMaskI += 1LL<< xyI;
    pieceMaskE += 1LL << xyE;

    if(isWhite){
        switch(moveKey.piece){
            case 'P':
            //if move not a promotion
            if(moveKey.flag == '0'){
                //remove piece from where it landed
                BBWhitePawns &= ~pieceMaskE;
            //promotion unmake
            } else {
                BBWhiteQueens &= ~pieceMaskE;
            }
            //put it back where it started
            BBWhitePawns |= pieceMaskI;
            //change color boards same way
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'R':
            BBWhiteRooks &= ~pieceMaskE;
            BBWhiteRooks |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'N':
            BBWhiteKnights &= ~pieceMaskE;
            BBWhiteKnights |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'B':
            BBWhiteBishops &= ~pieceMaskE;
            BBWhiteBishops |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'Q':
            BBWhiteQueens &= ~pieceMaskE;
            BBWhiteQueens |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;

            case 'K':
            BBWhiteKing &= ~pieceMaskE;
            BBWhiteKing |= pieceMaskI;
            BBWhitePieces &= ~pieceMaskE;
            BBWhitePieces |= pieceMaskI;
            break;
        }
    } else {
        switch(moveKey.piece){
            case 'p':
            if(moveKey.flag == '0'){
                //remove piece from where it landed
                BBBlackPawns &= ~pieceMaskE;
            //promotion unmake
            } else {
                BBBlackQueens &= ~pieceMaskE;
            }
            //put it back where it started
            BBBlackPawns |= pieceMaskI;
            //change color boards same way
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'r':
            BBBlackRooks &= ~pieceMaskE;
            BBBlackRooks |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'n':
            BBBlackKnights &= ~pieceMaskE;
            BBBlackKnights |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'b':
            BBBlackBishops &= ~pieceMaskE;
            BBBlackBishops |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'q':
            BBBlackQueens &= ~pieceMaskE;
            BBBlackQueens |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

            case 'k':
            BBBlackKing &= ~pieceMaskE;
            BBBlackKing |= pieceMaskI;
            BBBlackPieces &= ~pieceMaskE;
            BBBlackPieces |= pieceMaskI;
            break;

        }
    }

    //correct full tiles and run unmake capture function
    //if a piece has been captured ///Might have errors in calling unmake
    //even when a piece hasn't been captured
    if(isWhite){
        if(moveKey.captured == '0'){
            FullTiles &= ~pieceMaskE;
            FullTiles |= pieceMaskI;
        } else{
            undoCapture(pieceMaskE, moveKey.captured, !isWhite);
            FullTiles |= pieceMaskI;
        }
    } else {
        if(moveKey.captured == '0'){
            FullTiles &= ~pieceMaskE;
            FullTiles |= pieceMaskI;
        } else{
            undoCapture(pieceMaskE, moveKey.captured, !isWhite);
            FullTiles |= pieceMaskI;
        }
    }

    //correct empty tiles to opposite of full tiles
    EmptyTiles = ~FullTiles;

    //update zobrist hash
    zobrist->UpdateKey(xyI, xyE, moveKey, isWhite);

    //change cobrist color after a move
    zobrist->UpdateColor();

}

void BitBoards::undoCapture(U64 location, char piece, bool isNotWhite)
{

    if(isNotWhite){
        switch(piece){
            case 'P':
                //restore piece to both piece board and color board
                //no need to change FullTiles as captured piece was already there
                BBWhitePawns |= location;
                BBWhitePieces |= location;
                break;
            case 'R':
                BBWhiteRooks |= location;
                BBWhitePieces |= location;
                break;
            case 'N':
                BBWhiteKnights |= location;
                BBWhitePieces |= location;
                break;
            case 'B':
                BBWhiteBishops |= location;
                BBWhitePieces |= location;
                break;
            case 'Q':
                BBWhiteQueens |= location;
                BBWhitePieces |= location;
                break;
            case 'K':
                BBWhiteKing |= location;
                BBWhitePieces |= location;
                std::cout << "WKING CAPTURES" << std::endl;
            default:

                std::cout << "UNDO CAPTURE ERROR" << std::endl;

        }
    } else {
        switch(piece){
            case 'p':
                //restore piece to both piece board and color board
                //no need to change FullTiles as captured piece was already there
                BBBlackPawns |= location;
                BBBlackPieces |= location;
                break;
            case 'r':
                BBBlackRooks |= location;
                BBBlackPieces |= location;
                break;
            case 'n':
                BBBlackKnights |= location;
                BBBlackPieces |= location;
                break;
            case 'b':
                BBBlackBishops |= location;
                BBBlackPieces |= location;
                break;
            case 'q':
                BBBlackQueens |= location;
                BBBlackPieces |= location;
                break;
            case 'k':
                BBBlackKing |= location;
                BBBlackPieces |= location;
                std::cout << "BKING CAPTURES" << std::endl;
            default:
                drawBBA();
                std::cout << "UNDO CAPTURE ERROR" << std::endl;
        }
    }

}

void BitBoards::drawBB(U64 board)
{
    for(int i = 0; i < 64; i++){
        if(board & (1ULL << i)){
            std::cout<< 1 <<", ";
        } else {
            std::cout << 0 << ", ";
        }
        if((i+1)%8 == 0){
            std::cout<< std::endl;
        }
    }
    std::cout<< std::endl;
}

void BitBoards::drawBBA()
{
    char flips[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};
    char flipsL[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    int c = 0;

    for(int i = 0; i < 64; i++){
        if((i)%8 == 0){
            std::cout<< std::endl;
            std::cout << flips[c] << " | ";
            c++;
        }

        if(BBWhitePawns & (1ULL<<i)){
            std::cout << "P" << ", ";
        }
        if(BBWhiteRooks & (1ULL<<i)){
            std::cout << "R" << ", ";
        }
        if(BBWhiteKnights & (1ULL<<i)){
            std::cout << "N" << ", ";
        }
        if(BBWhiteBishops & (1ULL<<i)){
            std::cout << "B" << ", ";
        }
        if(BBWhiteQueens & (1ULL<<i)){
            std::cout << "Q" << ", ";
        }
        if(BBWhiteKing & (1ULL<<i)){
            std::cout << "K" << ", ";
        }
        if(BBBlackPawns & (1ULL<<i)){
            std::cout << "p" << ", ";
        }
        if(BBBlackRooks & (1ULL<<i)){
            std::cout << "r" << ", ";
        }
        if(BBBlackKnights & (1ULL<<i)){
            std::cout << "n" << ", ";
        }
        if(BBBlackBishops & (1ULL<<i)){
            std::cout << "b" << ", ";
        }
        if(BBBlackQueens & (1ULL<<i)){
            std::cout << "q" << ", ";
        }
        if(BBBlackKing & (1ULL<<i)){
            std::cout << "k" << ", ";
        }
        if(EmptyTiles & (1ULL<<i)){
            std::cout << " " << ", ";
        }

        //if(i % 8 == 7){
        //    std::cout << "| " << flips[c] ;
       //     c++;
       // }
    }

    std::cout << std::endl << "    ";
    for(int i = 0; i < 8; i++){
        std::cout << flipsL[i] << "  ";
    }

    std::cout << std::endl << std::endl;;
}

void BitBoards::removeCapturedPiece(char captured, U64 location)
{
    switch(captured){
    case 'P':
        BBWhitePawns &= ~location;
        BBWhitePieces &= ~location;
        break;
    case 'N':
        BBWhiteKnights &= ~location;
        BBWhitePieces &= ~location;
        break;
    case 'B':
        BBWhiteBishops &= ~location;
        BBWhitePieces &= ~location;
        break;
    case 'R':
        BBWhiteRooks &= ~location;
        BBWhitePieces &= ~location;
        break;
    case 'Q':
        BBWhiteQueens &= ~location;
        BBWhitePieces &= ~location;
        break;
    case 'p':
        BBBlackPawns &= ~location;
        BBBlackPieces &= ~location;
        break;
    case 'n':
        BBBlackKnights &= ~location;
        BBBlackPieces &= ~location;
        break;
    case 'b':
        BBBlackBishops &= ~location;
        BBBlackPieces &= ~location;
        break;
    case 'r':
        BBBlackRooks &= ~location;
        BBBlackPieces &= ~location;
        break;
    case 'q':
        BBBlackQueens &= ~location;
        BBBlackPieces &= ~location;
        break;
    default:
        std::cout << "Remove Captured Piece on make move ERROR" << std::endl;
    }
}
















