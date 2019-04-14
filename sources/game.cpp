#include "headers/game.h"

Game::Game(int width, int height)
{    
    setGameSize(width, height);

    m_player.setSource(QUrl::fromLocalFile(QString("/home/denis/Desktop/projects/snake/snake/sounds/pop1")));
    m_player.setLoopCount(QSoundEffect::Infinite);

    m_player.play();

    m_bodyImg   = QImage(QString(":/sprites/imgs/snakeBody.jpg"));
    m_headImg   = QImage(QString(":/sprites/imgs/head.jpg"));
    m_tokenImg  = QImage(QString(":/sprites/imgs/token.jpg"));

    m_painter = new QPainter();
}

void Game::setGameSize(int width, int height)
{
//    m_map = cv::Mat(height, width, CV_8UC3, cv::Scalar(127,127,127));
    m_mapWidth = width;
    m_mapHeight = height;

    resetGame();

    m_display = m_map;
}

void Game::resetGame()
{
    m_map = QImage(m_mapWidth * m_sizeMulti, m_mapHeight * m_sizeMulti, QImage::Format_RGB32);
    m_map.fill(QColor(Qt::gray).rgb());

    m_snakePoints.clear();

    m_score = 0;

    cv::Point headPos;

    headPos.x = m_mapWidth/2;
    headPos.y = m_mapHeight/2;

    std::pair< cv::Point, char > startPoint;

    startPoint.first = headPos;
    startPoint.second =  'e';

    m_snakePoints.push_back(startPoint);

    buildTokenList();

    addToken();

    emit newScore(m_score);
}

void Game::nextGameTick(char command)
{
    cv::Point previousBodyPos = m_snakePoints.front().first;

    char previousBodyHeading = m_snakePoints.front().second;
    char heading = m_snakePoints.front().second;

    heading = determineNewHeading(heading, command);

    m_snakePoints.front().second = heading;

    moveHead(heading, previousBodyPos);

    checkCollision();

    if(m_snakePoints.size() == 1){return;}

    //Move remaining body parts while tracking heading for each
    for(size_t i = 1; i < m_snakePoints.size(); i++){
        auto currentPoint = m_snakePoints.at(i).first;
        char currentHeading = m_snakePoints.at(i).second;

        m_snakePoints.at(i).first = previousBodyPos;
        m_snakePoints.at(i).second = previousBodyHeading;

        previousBodyPos = currentPoint;
        previousBodyHeading = currentHeading;
    }

    emit newScore(m_score);
}

QPixmap Game::getGameAsPixmap()
{
    return QPixmap::fromImage(m_display);
}

void Game::drawSnake()
{
    try {
        m_display = m_map;
        m_painter->begin(&m_display);

        for(auto &bodyPoint : m_snakePoints){

            QImage bodyImg;

            if(bodyPoint.second == 'n'){bodyImg = rotateImg(m_bodyImg, 0);}
            if(bodyPoint.second == 'e'){bodyImg = rotateImg(m_bodyImg, 90);}
            if(bodyPoint.second == 'w'){bodyImg = rotateImg(m_bodyImg, -90);}
            if(bodyPoint.second == 's'){bodyImg = rotateImg(m_bodyImg, 180);}

            m_painter->drawImage(QPoint(bodyPoint.first.x * m_sizeMulti, bodyPoint.first.y * m_sizeMulti), bodyImg);
        }

        QImage headImg;

        if(m_snakePoints.front().second == 'n'){headImg = rotateImg(m_headImg, 0);}
        if(m_snakePoints.front().second == 'e'){headImg = rotateImg(m_headImg, 90);}
        if(m_snakePoints.front().second == 'w'){headImg = rotateImg(m_headImg, -90);}
        if(m_snakePoints.front().second == 's'){headImg = rotateImg(m_headImg, 180);}

        m_painter->drawImage(QPoint(m_snakePoints.front().first.x * m_sizeMulti, m_snakePoints.front().first.y * m_sizeMulti), headImg);
        m_painter->drawImage(QPoint(m_tokenPoint.x * m_sizeMulti, m_tokenPoint.y * m_sizeMulti), m_tokenImg);

        m_painter->end();

    } catch (std::exception &e) {qDebug("%s", e.what());}
}

void Game::addToken()
{
    size_t index = static_cast<size_t>(rand()) % m_totalTokens.size();

    m_tokenPoint = m_totalTokens.at(index);

    m_totalTokens.erase(m_totalTokens.begin() + static_cast<long>(index));

}

void Game::checkCollision()
{
    if(m_snakePoints.front().first.y == m_tokenPoint.y && m_snakePoints.front().first.x == m_tokenPoint.x){
        std::pair<cv::Point, char> newBodyPart;

        newBodyPart.second = m_snakePoints.back().second;

        if(m_snakePoints.back().second == 'n'){
            newBodyPart.first.x = m_snakePoints.back().first.x;
            newBodyPart.first.y = m_snakePoints.back().first.y + 1;
        }
        if(m_snakePoints.back().second == 's'){
            newBodyPart.first.x = m_snakePoints.back().first.x;
            newBodyPart.first.y = m_snakePoints.back().first.y - 1;
        }
        if(m_snakePoints.back().second == 'e'){
            newBodyPart.first.x = m_snakePoints.back().first.x - 1;
            newBodyPart.first.y = m_snakePoints.back().first.y;
        }
        if(m_snakePoints.back().second == 'w'){
            newBodyPart.first.x = m_snakePoints.back().first.x + 1;
            newBodyPart.first.y = m_snakePoints.back().first.y;
        }

        m_score += 10;
        m_snakePoints.push_back(newBodyPart);

        addToken();
    }

    bool first = true;

    for(auto &currentBodyPart : m_snakePoints){
        if(currentBodyPart.first.x == m_snakePoints.front().first.x && currentBodyPart.first.y == m_snakePoints.front().first.y && !first){
            emit gameOver();
        }

        if(first){
            first = false;
        }
    }
}

void Game::buildTokenList()
{
    for(int x = 0; x < m_mapWidth; x++){
        for(int y = 0; y < m_mapHeight; y++){
            cv::Point pt;

            pt.x = x;
            pt.y = y;

            m_totalTokens.push_back(pt);
        }
    }
}

void Game::moveHead(const char heading,  cv::Point &previousBodyPos)
{
    if(heading == 'n'){
        if(previousBodyPos.y - 1 < 0){
             m_snakePoints.front().first.y = m_mapHeight - 1;
        }
        else {
            m_snakePoints.front().first.y = previousBodyPos.y - 1;
        }
    }

    else if(heading == 'e'){
        if(previousBodyPos.x + 1 >= m_mapWidth){
             m_snakePoints.front().first.x = 0;
        }
        else {
            m_snakePoints.front().first.x = previousBodyPos.x + 1;
        }
    }

    else if(heading == 'w'){
        if(previousBodyPos.x - 1 < 0){
             m_snakePoints.front().first.x = m_mapWidth - 1;
        }
        else {
            m_snakePoints.front().first.x = previousBodyPos.x - 1;
        }
    }

    else if(heading == 's'){
        if(previousBodyPos.y + 1 >= m_mapHeight){
             m_snakePoints.front().first.y = 0;
        }
        else {
            m_snakePoints.front().first.y = previousBodyPos.y + 1;
        }
    }
}

bool Game::containsSnake(int x, int y)
{
    for(auto &currentBodyPart : m_snakePoints){
        if(currentBodyPart.first.x == x && currentBodyPart.first.y == y){
            return true;
        }
    }

    return false;
}

char Game::determineNewHeading(char heading, const char command)
{
    if(command == 'u' && (heading == 'e' || heading == 'w')){
        heading = 'n';
    }
    else if (command == 'l') {
        if(heading == 'n'){
            heading = 'w';
        }
        else if (heading == 's') {
            heading  =  'w';
        }
    }
    else if (command == 'r') {
        if(heading == 'n'){
            heading = 'e';
        }
        else if (heading == 's') {
            heading  =  'e';
        }
    }
    if(command == 'd' && (heading == 'e' || heading == 'w')){
        heading = 's';
    }

    return heading;
}

QImage Game::rotateImg(const QImage &srcImg, int angle)
{
    QPoint center = srcImg.rect().center();
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(angle);
    QImage dstImg = srcImg.transformed(matrix);

    return dstImg;
}

std::pair<cv::Point, cv::Point> Game::getBoundingRect()
{
    cv::Point topLeft   = m_snakePoints.front().first;
    cv::Point topRight  = m_snakePoints.front().first;

    for(auto &currentBodyPart : m_snakePoints){
        if(currentBodyPart.first.x < topLeft.x && currentBodyPart.first.y < topLeft.y){
            topLeft = currentBodyPart.first;
        }
        if(currentBodyPart.first.x > topLeft.x && currentBodyPart.first.y > topLeft.y){
            topRight = currentBodyPart.first;
        }
    }

    std::pair<cv::Point, cv::Point> bounding;

    bounding.first = topLeft;
    bounding.second = topRight;

    return bounding;
}

Game::Game()
{

}
