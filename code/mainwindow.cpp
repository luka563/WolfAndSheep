#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "clickhandle.h"
#include <QMessageBox>
#include "gamelogic.h"
#include <QElapsedTimer>
#include <QThread>
#define SIZE 8
ClickHandle* clickHandle;
QList<QLabel*> list;
float totalEvaluations = 0;
int startX,startY,player,picSize = 64;
char Board[SIZE][SIZE];
GameLogic game;
Move bestMove;
bool firstInputDone = false;
CELL firstCELL;
QLabel *labelRed;
QElapsedTimer timer;
std::chrono::duration<double> duration;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);    
    game.initialize(Board);
    clickHandle = new ClickHandle(this);
    startX = 9;
    startY = 14;
    QLabel* labelSheep = new QLabel(this);
    QLabel* labelWolf1 = new QLabel(this);
    QLabel* labelWolf2 = new QLabel(this);
    QLabel* labelWolf3 = new QLabel(this);
    QLabel* labelWolf4 = new QLabel(this);
    list << labelSheep << labelWolf1<<labelWolf2<<labelWolf3<<labelWolf4;

    labelRed = new QLabel(this);
    labelRed->setObjectName("myLabel");
    labelRed->setStyleSheet("QLabel#myLabel { background-color: rgba(255, 0, 0, 0.2); }");


    QPixmap imageSheep("C:/Users/luka/Desktop/sheep");
    QPixmap imageWolf("C:/Users/luka/Desktop/wolf");
    for(int i=0;i< list.length();i++)
    {
        list[i]->setAttribute(Qt::WA_TransparentForMouseEvents);
        list[i]->setScaledContents(true);
        if(i == 0)
            list[i]->setPixmap(imageSheep);
        else
            list[i]->setPixmap(imageWolf);
        list[i]->hide();
    }
    printBoard();
    connectSlot();
    ui->label->installEventFilter(clickHandle);
    ui->ResetGame->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::playGame(int i,int j)
{    
    disconnectSlot();
    if(player == 1)
    {
        CELL sheep = game.coordinatesOfSheep(Board);
        if(game.isValid(sheep.i,sheep.j,i,j) && game.isFree(Board,CELL{i,j}))
        {
            game.makeMoveSheep(Board,Move{sheep.i,sheep.j,i,j});

            if (game.isGameOver(Board) == 999)
            {
                printBoard();
                QMessageBox::information(this, "Game over", "You won");
                disconnectSlot();
            }else{
                auto start = std::chrono::steady_clock::now();
                bestMove = game.BestMoveMinimizing(Board, game.DEPTH);
                auto end = std::chrono::steady_clock::now();
                std::chrono::duration<double> duration = end - start;
                qDebug()<< "Wolfs Time(s):"<< duration.count() <<" Evaluations: " << game.totalEvaluations <<"Evaluations per second: "<< game.totalEvaluations/duration.count() <<"DEPTH: "<<game.getDepth();


                game.makeMove(Board, bestMove);
                printBoard();
                connectSlot();
            }
            if(game.isGameOver(Board) == -999){
                QMessageBox::information(this, "Game over", "You lost");
                disconnectSlot();
            }
        }else
        {
            QMessageBox::information(this, "Invalid move", "Please select valid move");
            connectSlot();
        }
    }else{
        if(player == 2)
        {
            if(firstInputDone == false)
            {
                int numMoves;
                CELL* wolves = game.coordinatesOfWolves(Board);

                for(int w=0;w<4;w++)
                {
                    if(wolves[w].i == i && wolves[w].j==j)
                    {
                        firstInputDone = true;
                        firstCELL={i,j};
                        printBoard();
                        connectSlot();
                        return;
                    }
                }
                QMessageBox::information(this, "Invalid move", "Invalid move,select wolf");
                connectSlot();
            }else
            {
                if(firstCELL.i - 1 == i && abs(firstCELL.j - j)==1 && game.isFree(Board,CELL{i,j}))
                {
                    disconnectSlot();
                    //
                    game.makeMove(Board,Move{firstCELL.i,firstCELL.j,i,j});
                    printBoard();
                    if(game.isGameOver(Board)==-999)
                    {
                        QMessageBox::information(this, "Game over", "You won");
                        disconnectSlot();
                        return;
                   }
                    auto start = std::chrono::steady_clock::now();
                    bestMove = game.BestMoveMaximizing(Board,game.DEPTH+1);
                    auto end = std::chrono::steady_clock::now();
                    std::chrono::duration<double> duration = end - start;
                    qDebug()<< "Sheeps Time(s):"<< duration.count() <<" Evaluations: " << game.totalEvaluations <<"Evaluations per second: "<< game.totalEvaluations/duration.count() <<"DEPTH: "<<game.getDepth()+1;


                    game.makeMoveSheep(Board,bestMove);
                    firstInputDone = false;
                    printBoard();
                    if(game.isGameOver(Board)==999)
                        QMessageBox::information(this, "Game over", "You lost");                    
                    connectSlot();
                    return;
                }
                QMessageBox::information(this, "Invalid move", "select free square in front of wolf");
                connectSlot();
            }
        }
    }

}
void MainWindow::disconnectSlot()
{
    disconnect(clickHandle, &ClickHandle::clicked, this, nullptr);
}
void MainWindow::connectSlot()
{
    connect(clickHandle, &ClickHandle::clicked, this, [=](int x, int y) {
        int i=y/picSize;
        int j=x/picSize;
        playGame(i,j);
    });
}
void MainWindow::printBoard()
{
    if(firstInputDone)
    {
    labelRed->setGeometry(startX+firstCELL.j*picSize,startY+firstCELL.i*picSize,picSize,picSize);
    labelRed->show();
    }else
    labelRed->hide();
    int wolves = 1;
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++)
            if(Board[i][j] == '0')
            {
                list[0]->setGeometry(startX+j*picSize,startY+i*picSize,picSize,picSize);                
                list[0]->show();
            }else
                if(Board[i][j]=='1')
                {
                    list[wolves]->setGeometry(startX+j*picSize,startY+i*picSize,picSize,picSize);
                    list[wolves]->show();
                    wolves++;
                }
    QApplication::processEvents();
}
void MainWindow::computerVScomputer()
{
    while(game.isGameOver(Board) == 0)
    {
        auto start = std::chrono::steady_clock::now();
        bestMove = game.BestMoveMaximizing(Board,game.DEPTH+1);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = end - start;
        qDebug()<< "Sheeps Time(s):"<< duration.count() <<" Evaluations: " << game.totalEvaluations <<"Evaluations per second: "<< game.totalEvaluations/duration.count() <<"DEPTH: "<<game.getDepth()+1;
        int remainingTime = 1000 - std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

        if (remainingTime > 0) {
                QThread::msleep(remainingTime);
        }
        game.makeMoveSheep(Board,bestMove);
        printBoard();
        if(game.isGameOver(Board)!= 0)
                break;
        start = std::chrono::steady_clock::now();
        bestMove = game.BestMoveMinimizing(Board,game.DEPTH);
        end = std::chrono::steady_clock::now();
        duration = end - start;
        qDebug()<< "Wolfs Time(s):"<< duration.count() <<" Evaluations: " << game.totalEvaluations <<"Evaluations per second: "<< game.totalEvaluations/duration.count() <<"DEPTH: "<<game.getDepth();
        remainingTime = 1000 - std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

        if (remainingTime > 0) {
                QThread::msleep(remainingTime);
        }

        game.makeMove(Board,bestMove);
        printBoard();

    }
    ui->ResetGame->setEnabled(true);
}

void MainWindow::on_playAsSheep_clicked()
{
    disableElements();
    player = 1;
    connectSlot();


}
void MainWindow::disableElements()
{
    ui->playAsSheep->setEnabled(false);
    ui->playAsWolf->setEnabled(false);
    ui->ResetGame->setEnabled(true);
    ui->computerVscomputer->setEnabled(false);
    if(ui->AlphaBeta->isChecked())
        game.allowAlphaBeta();
    else
        game.denyAlphaBeta();
    ui->AlphaBeta->setEnabled(false);
}

void MainWindow::on_playAsWolf_clicked()
{
    disableElements();
    player = 2;
    firstInputDone = false;    
    disconnectSlot();
    bestMove = game.BestMoveMaximizing(Board,game.DEPTH);
    game.makeMoveSheep(Board,bestMove);
    printBoard();
    connectSlot();
}
void MainWindow::reset()
{
    game.initialize(Board);
    firstInputDone = false;
    printBoard();
    ui->playAsSheep->setEnabled(true);
    ui->playAsWolf->setEnabled(true);
    ui->ResetGame->setEnabled(false);    
    ui->computerVscomputer->setEnabled(true);
    ui->AlphaBeta->setEnabled(true);
    disconnectSlot();
    qDebug()<< "----------------- Reset ------------------";
}

void MainWindow::on_ResetGame_clicked()
{
    reset();
}


void MainWindow::on_computerVscomputer_clicked()
{
    disableElements();
    ui->ResetGame->setEnabled(false);
    computerVScomputer();
}

