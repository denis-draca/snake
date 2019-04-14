#include "headers/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug("hi");
    ui->setupUi(this);

    installEventFilter(this);

    m_viewer            = new myQGraphicsView(this);
    m_mainGameScene     = new QGraphicsScene(this);
    m_myGame            = new Game(GAMEWIDTH,GAMEHEIGHT);
    m_gameTicker        = new QTimer(this);
    m_frameTicker       = new QTimer(this);
    m_pixmap            = new QGraphicsPixmapItem();
    m_textItem          = new QGraphicsTextItem();
    m_score             = new QLabel(this);

    m_nextCommand = '0';

//    //Control Buttons

    QPushButton *newGame     = new QPushButton(tr("New Game"), this);

    ui->verticalLayout->addWidget(m_score);
    ui->verticalLayout->addWidget(m_viewer);
    ui->verticalLayout->addWidget(newGame);

//    ui->verticalLayout->addLayout(controlButtonLayout);

    m_pixmap = m_mainGameScene->addPixmap(m_myGame->getGameAsPixmap());
    m_textItem = m_mainGameScene->addText(QString(""));

    m_mainGameScene->setSceneRect(m_pixmap->boundingRect());

    m_viewer->setScene(m_mainGameScene);

    m_viewer->installEventFilter(this);

    connect(newGame, &QPushButton::clicked, this, &MainWindow::newGame);

    connect(m_gameTicker, &QTimer::timeout, this, &MainWindow::gameTick);
    connect(m_frameTicker, &QTimer::timeout, this, &MainWindow::frameTick);

    connect(m_myGame, &Game::gameOver, this, &MainWindow::gameOver);
    connect(m_myGame, &Game::newScore, this, &MainWindow::gameScore);

    m_gameTicker->start(100);
    m_frameTicker->start(100);
}

MainWindow::~MainWindow()
{
    delete m_viewer;
    delete ui;
}

void MainWindow::gameTick()
{
    m_myGame->nextGameTick(m_nextCommand);

    m_nextCommand = '0';
}

void MainWindow::frameTick()
{
    m_myGame->drawSnake();

    QPixmap newPixmap = m_myGame->getGameAsPixmap();

    m_pixmap->setPixmap(newPixmap);
}

void MainWindow::gameOver()
{
    m_gameTicker->stop();
    m_frameTicker->stop();

    m_pixmap->setPixmap(QPixmap());

    m_textItem->setPlainText(QString("YOU FUCKED UP"));

    m_mainGameScene->setSceneRect(m_textItem->boundingRect());

    m_viewer->fitInView(m_viewer->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::newGame()
{
    m_textItem->setPlainText(QString(""));

    m_myGame->resetGame();


    m_pixmap->setPixmap(m_myGame->getGameAsPixmap());

    m_mainGameScene->setSceneRect(m_pixmap->boundingRect());

    m_viewer->fitInView(m_viewer->sceneRect(), Qt::KeepAspectRatio);

    m_gameTicker->start(100);
    m_frameTicker->start(100);
}

void MainWindow::gameScore(int score)
{
    m_score->setText(QString("Current Score: %1").arg(score));
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {

        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        switch (keyEvent->key()) {
            case Qt::Key::Key_Right:
                rightSelected();
                break;
            case Qt::Key_Left:
                leftSelected();
                break;
            case Qt::Key_Up:
                upSelected();
                break;
            case Qt::Key_Down:
                downSelected();
                break;
        }

    }

    return QObject::eventFilter(target, event);
}
