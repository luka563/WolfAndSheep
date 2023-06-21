#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#define SIZE 8
#define SHEEP 0
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>



struct CELL {
    int i;
    int j;
};
struct Move {
    int startI;
    int startJ;
    int endI;
    int endJ;
};

class GameLogic
{
private:
    bool AlphaBeta;    
public:
    struct CELL sheep;
    float totalEvaluations = 0;
    int DEPTH;
    GameLogic();
    int getDepth();
    void setDepth(int depth);
    void initialize(char board[][SIZE]);
    int Min(int a, int b);
    int Max(int a, int b);
    int MinimaxValue(char state[][SIZE], int depth, int alpha, int beta, int isMaximizingPlayer);
    int isGameOver( char board[][SIZE]);
    struct CELL coordinatesOfSheep(char board[][SIZE]);
    bool canSheepMove(int i, int j, char board[][SIZE]);
    int isValid(int startI, int startJ, int destI, int destJ);
    struct Move* LegalSheepMoves(char[][SIZE], int* numMoves);

    void makeMoveSheep(char board[][SIZE], struct Move m);
    void revertMoveSheep(char board[][SIZE], struct Move m);

    void makeMove(char board[][SIZE], struct Move m);
    void revertMove(char board[][SIZE], struct Move m);
    struct Move* LegalWolvesMoves(char board[][SIZE], int* numMoves);   
    struct CELL* coordinatesOfWolves(char board[][SIZE]);
    struct Move BestMoveMaximizing(char state[][SIZE], int depth);
    int evaluate(char board[][SIZE]);
    struct Move BestMoveMinimizing(char state[][SIZE], int depth);
    bool isFree(char board[][SIZE],CELL cell);
    void allowAlphaBeta();
    void denyAlphaBeta();
};

#endif // GAMELOGIC_H
