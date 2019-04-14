#ifndef GAME_H
#define GAME_H

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QPixmap>
#include <QImage>
#include <QRgb>
#include <QSoundEffect>
#include <QSound>
#include <QPainter>

#include <vector>
#include <queue>
#include <chrono>
#include <thread>

class Game: public QObject
{
    Q_OBJECT
public:
    Game();
    Game(int width, int height);

    void setGameSize(int width, int height);
    void resetGame();
    void nextGameTick(char command);

    void drawSnake();

    QPixmap getGameAsPixmap();

signals:
    void gameOver();
    void newScore(int score);

private:
//    cv::Mat m_object;
//    cv::Mat m_map;
//    cv::Mat m_display;

    QImage m_map;
    QImage m_display;
    QImage m_bodyImg;
    QImage m_headImg;
    QImage m_tokenImg;

    QPainter *m_painter;

    QSoundEffect m_player;

    std::vector< std::pair<cv::Point, char> > m_snakePoints;
    std::vector< cv::Point > m_totalTokens;

    cv::Point m_tokenPoint;

    int m_mapWidth;
    int m_mapHeight;
    int m_score;

    const int m_sizeMulti = 16;
private://methoids

    void addToken();
    void checkCollision();
    void buildTokenList();
    void moveHead(const char heading, cv::Point &previousBodyPos);

    bool containsSnake(int x, int y);

    char determineNewHeading(char heading, const char command);

    QImage rotateImg(const QImage &srcImg, int angle);

    std::pair<cv::Point, cv::Point> getBoundingRect();
};

#endif // GAME_H
