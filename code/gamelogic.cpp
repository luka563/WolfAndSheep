#include "gamelogic.h"

GameLogic::GameLogic()
{
    DEPTH = 18;
    allowAlphaBeta();
}
int GameLogic::getDepth()
{
    return DEPTH;
}
void GameLogic::setDepth(int depth)
{
    DEPTH = depth;
    return;
}
int GameLogic::Min(int a, int b) {
    return (a < b) ? a : b;
}
int GameLogic::Max(int a, int b) {
    return (a > b) ? a : b;
}
void GameLogic::initialize(char board[][SIZE]) {   
    int i, j;
    for (i = 0;i < SIZE;i++)
        for (j = 0;j < SIZE;j++)
            if ((j + i) % 2 == 0)
                board[i][j] = 'X';
            else
                board[i][j] = ' ';
    board[0][3] = '0';
    sheep.i = 0;
    sheep.j = 3;
    for (i = 0; i < SIZE; i += 2)
        board[7][i] = '1';
}
int GameLogic::MinimaxValue(char state[][SIZE], int depth, int alpha, int beta, int isMaximizingPlayer) {    
    if (depth == 0 || isGameOver(state) != 0)
    {
        totalEvaluations++;        
        if (isMaximizingPlayer)
            return evaluate(state) + DEPTH - depth;
        return evaluate(state) - DEPTH + depth;
    }
    if (isMaximizingPlayer)
    {
        int bestScore = INT_MIN,numMoves;
        struct Move* possibleMoves = LegalSheepMoves(state,&numMoves);

        for (int i = 0;i < numMoves;i++)
        {
            makeMoveSheep(state, possibleMoves[i]);
            bestScore = Max(bestScore, MinimaxValue(state, depth - 1, alpha, beta, 0));
            revertMoveSheep(state, possibleMoves[i]);
            //bestScore = Max(bestScore, score);

            if(AlphaBeta)
            {
            alpha = Max(alpha, bestScore);
            if (beta <= alpha)
            {
                break;
            }
            }
        }
        delete[] possibleMoves;
        return bestScore;
    }
    else
    {
        int bestScore = INT_MAX,numMoves;
        struct Move* possibleMoves = LegalWolvesMoves(state,&numMoves);

        for (int i = 0;i < numMoves;i++)
        {
            makeMove(state, possibleMoves[i]);
            bestScore = Min(bestScore, MinimaxValue(state, depth - 1, alpha, beta, 1));
            revertMove(state, possibleMoves[i]);
            //bestScore = Min(bestScore, score);
            if(AlphaBeta)
            {
            beta = Min(beta, bestScore);
            if (beta <= alpha)
            {
                break;
            }
            }
        }
        delete[] possibleMoves;
        return bestScore;
    }
}
struct Move GameLogic::BestMoveMinimizing(char state[][SIZE], int depth) {
    totalEvaluations = 0;
    int bestScore = INT_MAX;
    int alpha = INT_MIN;
    int beta = INT_MAX;
    struct Move bestMove;
    bool bestMoveFound = false;
    int numMoves;
    struct Move* possibleMoves = LegalWolvesMoves(state,&numMoves);
    if(numMoves == 1)
    {
        bestMove = possibleMoves[0];
        delete[] possibleMoves;
        //free(possibleMoves);
        return bestMove;
    }
    for (int i = 0;i < numMoves;i++)
    {
        makeMove(state, possibleMoves[i]);
        int score = MinimaxValue(state, depth - 1, alpha, beta, 1);
        revertMove(state, possibleMoves[i]);
        if (score < bestScore)
        {
            bestScore = score;
            bestMove = possibleMoves[i];
            bestMoveFound = true;
        }
        if(AlphaBeta)
        {
        beta = Min(beta, bestScore);
        if (beta <= alpha)
        {
            break;
        }
        }
    }
    if(bestMoveFound == false)
        bestMove = possibleMoves[0];
    delete[] possibleMoves;
    //free(possibleMoves);
    return bestMove;
}


struct Move* GameLogic::LegalWolvesMoves(char board[][SIZE], int* numMoves) {
    int z;
    *numMoves = 0;
    struct Move* moves = new Move[8];
    struct CELL* wolves = coordinatesOfWolves(board);

    for (z = 0; z < 4; z++) {
        int i = wolves[z].i;
        int j = wolves[z].j;

        //gore levo
        if (board[i - 1][j - 1] == ' ')
            moves[(*numMoves)++] = Move{ i, j, i - 1, j - 1 };

        //gore desno
        if (board[i - 1][j + 1] == ' ')
            moves[(*numMoves)++] = Move{ i, j, i - 1, j + 1 };
    }

    if (*numMoves < 8) {
        Move* temp = new Move[*numMoves];
        std::copy(moves, moves + *numMoves, temp);
        delete[] moves;
        moves = temp;
    }

    delete[] wolves;
    return moves;
}



int GameLogic::evaluate(char board[][SIZE]) {    
    if (!canSheepMove(sheep.i, sheep.j, board))
        return INT_MIN;
    else
        if (sheep.i == SIZE - 1)
            return INT_MAX;

    //return sheep.i;


    struct CELL* wolves = coordinatesOfWolves(board);

    if(wolves[0].i == wolves[1].i && wolves[1].i == wolves[2].i &&
        wolves[2].i == wolves[3].i && sheep.i < wolves[0].i)
            return INT_MIN/(wolves[0].i +1);

    if(sheep.i <= wolves[0].i)
        return INT_MAX/2;

    int left = 0,i;

    for( i = 0;i<4;i++)
        if(wolves[i].j<4)
            left++;
    if(left == 2)
        return INT_MIN/4;

    int volvesSum = 0;
    for ( i = 0; i < 4; i++)
        volvesSum += abs(wolves[i].i - sheep.i);
    return sheep.i * 2 - volvesSum;
}

struct CELL*  GameLogic::coordinatesOfWolves(char board[][SIZE]) {
    int br = 0,i,j;
    char c;
    struct CELL* cells = new CELL[4];
    //CELL sheep = coordinatesOfSheep(board);
    for (i = SIZE-1; i >=0; i--)
        if(sheep.j<4)
        for (j = (i+1)%2; j < SIZE; j+=2)
        //for(j=0;j<SIZE;j++)
        {
            c = board[i][j];
            if (c == '1')
                cells[br++] = CELL{ i, j };
        }

    else
        for (j = (SIZE-1) - (i%2) ; j >=0; j-=2)
        //for (j = SIZE-1; j >=0; j--)
        {
            c = board[i][j];
            if (c == '1')
                cells[br++] = CELL{ i, j };
        }

    return cells;
}
void GameLogic::makeMoveSheep(char board[][SIZE], struct Move m) {
    board[m.endI][m.endJ] = board[m.startI][m.startJ];
    board[m.startI][m.startJ]=' ';
    sheep.i = m.endI;
    sheep.j = m.endJ;
}

void GameLogic::revertMoveSheep(char board[][SIZE], struct Move m) {
    //char pom = board[m.startI][m.startJ];
    //board[m.startI][m.startJ] = board[m.endI][m.endJ];
    //board[m.endI][m.endJ]=pom;

    board[m.startI][m.startJ]=board[m.endI][m.endJ];
    board[m.endI][m.endJ] = ' ';
    sheep.i = m.startI;
    sheep.j = m.startJ;
}



void GameLogic::makeMove(char board[][SIZE], struct Move m) {
    //char pom = board[m.startI][m.startJ];
    //board[m.startI][m.startJ] = board[m.endI][m.endJ];
    //board[m.endI][m.endJ]=pom;

    board[m.endI][m.endJ] = board[m.startI][m.startJ];
    board[m.startI][m.startJ]=' ';
    /*
    if(m.startI == sheep.i && m.startJ == sheep.j)
    {
    sheep.i = m.endI;
    sheep.j = m.endJ;
    }
    */
}

    void GameLogic::revertMove(char board[][SIZE], struct Move m) {
    //char pom = board[m.startI][m.startJ];
    //board[m.startI][m.startJ] = board[m.endI][m.endJ];
    //board[m.endI][m.endJ]=pom;

    board[m.startI][m.startJ]=board[m.endI][m.endJ];
    board[m.endI][m.endJ] = ' ';
    /*
    if(m.endI == sheep.i && m.endJ == sheep.j)
    {
    sheep.i = m.startI;
    sheep.j = m.startJ;
    }
    */
}

    struct Move* GameLogic::LegalSheepMoves(char board[][SIZE], int* numMoves) {

    int i = sheep.i;
    int j = sheep.j;
    *numMoves = 0;
    struct Move* moves = new Move[4];

    // dole desno
    if (board[i + 1][j + 1] == ' ')
    moves[(*numMoves)++] = Move{ i, j, i + 1, j + 1 };


    // dole levo
    if (board[i + 1][j - 1] == ' ')
    moves[(*numMoves)++] = Move{ i, j, i + 1, j - 1 };


    // gore desno
    if (board[i - 1][j + 1] == ' ')
    moves[(*numMoves)++] = Move{ i, j, i - 1, j + 1 };


    // gore levo

    if (board[i - 1][j - 1] == ' ')
    moves[(*numMoves)++] = Move{ i, j, i - 1, j - 1 };


    if (*numMoves < 4) {
    Move* temp = new Move[*numMoves];
    std::copy(moves, moves + *numMoves, temp);
    delete[] moves;
    moves = temp;
    }

    return moves;
    }

int GameLogic::isGameOver( char board[][SIZE]) {
    //struct CELL sheep = coordinatesOfSheep(board);
    if (!canSheepMove(sheep.i, sheep.j, board))
        return -999;
    else
        if (sheep.i == SIZE - 1)
            return 999;
    return 0;
}
int GameLogic::isValid(int startI, int startJ, int destI, int destJ) {
    //if (destI >= SIZE || destI < 0 || destJ >= SIZE || destJ < 0 || startI >= SIZE || startI < 0 || startJ >= SIZE || startJ < 0)
    //    return 0;
    if (abs(destJ - startJ) != 1 || abs(startI - destI) != 1)
        return 0;
    //if ( abs(startI - destI) != 1)
    //    return 0;
    return 1;
}

bool GameLogic::canSheepMove(int i, int j, char board[][SIZE]) {

    if (board[i + 1][j + 1] == ' ')
        return true;

    if (board[i + 1][j - 1] == ' ')
        return true;

    if (board[i - 1][j - 1] == ' ')
        return true;

    if (board[i - 1][j + 1] == ' ')
        return true;

    return false;
}
struct CELL GameLogic::coordinatesOfSheep(char board[][SIZE]) {
    struct CELL c ={0,0};
    int i, j;
    for (i = 0; i < SIZE; i++)
        for (j = (i+1)%2; j < SIZE; j+=2)
        //for (j = 0; j < SIZE; j++)
            if (board[i][j] == '0')
            {
                c.i = i;
                c.j = j;
                return c;
            }
    return c;
}
struct Move GameLogic::BestMoveMaximizing(char state[][SIZE], int depth) {
    totalEvaluations = 0;
    int bestScore = INT_MIN;
    int alpha = INT_MIN;
    int beta = INT_MAX;
    struct Move bestMove;
    bool bestMoveFound = false;
    int numMoves;
    struct Move* possibleMoves = LegalSheepMoves(state,&numMoves);
    if(numMoves == 1)
    {
        bestMove = possibleMoves[0];
        delete[] possibleMoves;
        //free(possibleMoves);
        return bestMove;
    }


    for (int i = 0;i < numMoves;i++)
    {
        makeMoveSheep(state, possibleMoves[i]);
        int score = MinimaxValue(state, depth - 1, alpha, beta, 0);
        revertMoveSheep(state, possibleMoves[i]);

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = possibleMoves[i];
            bestMoveFound = true;
        }
        if(AlphaBeta)
        {
        alpha = Max(alpha, bestScore);
        if (beta <= alpha)
        {
            break;
        }
        }
    }
    if(bestMoveFound == false)
        bestMove = possibleMoves[0];
    delete[] possibleMoves;
    return bestMove;
}
bool GameLogic::isFree(char board[][SIZE],CELL cell)
{
    return board[cell.i][cell.j]==' ';
}
void GameLogic::allowAlphaBeta()
{
    AlphaBeta = true;
}
void GameLogic::denyAlphaBeta()
{
    AlphaBeta = false;
}
