#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QPixmap>
#include <QKeyEvent>
#include <QLabel>

#include "myqgraphicsview.h"
#include "game.h"


#define GAMEWIDTH 20
#define GAMEHEIGHT 20

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void leftSelected() {m_nextCommand = 'l';}
    void rightSelected(){m_nextCommand = 'r';}
    void upSelected()   {m_nextCommand = 'u';}
    void downSelected() {m_nextCommand = 'd';}
    void gameTick();
    void frameTick();

    void gameOver();
    void newGame();
    void gameScore(int score);

    bool eventFilter(QObject *target, QEvent *event);

private:
    Ui::MainWindow *ui;

    myQGraphicsView *m_viewer;

    QGraphicsScene *m_mainGameScene;

    QGraphicsPixmapItem *m_pixmap;
    QGraphicsTextItem *m_textItem;

    Game *m_myGame;

    QTimer *m_gameTicker;
    QTimer *m_frameTicker;

    QLabel *m_score;

    char m_nextCommand;
};

#endif // MAINWINDOW_H
