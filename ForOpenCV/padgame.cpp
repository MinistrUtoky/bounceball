#include "padgame.h"

PadGame::GamePhase PadGame::phase = PadGame::GamePhase::Start;

void PadGame::onMouse(int event, int x, int y) {
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        phase = Game;
    }
}

void PadGame::onMouse(int event, int x, int y, int flags, void* userdata) {
    PadGame* pg = reinterpret_cast<PadGame*>(userdata);
    pg->onMouse(event, x, y);
}

void PadGame::start() {
    ballAngle = rand() % 180;
    ballDirection = cv::getRotationMatrix2D(cv::Point2f{ 0, 0 }, ballAngle, 1.).reshape(1).t();
    pad = Pad(WIDTH / 2, HEIGHT - (HEIGHT / 40), WIDTH / 10, HEIGHT / 40);
    ball = Ball(rand() % WIDTH, rand() % (HEIGHT / 2), HEIGHT / 40);
    phase = Start;

    startLoop();
}

void PadGame::startLoop() {
    cv::Mat startImage = cv::imread("T:/Start.png", cv::IMREAD_COLOR);
    cv::namedWindow(WIN_NAME, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(WIN_NAME, onMouse, this);
    while (phase == Start) {
        int key = cv::pollKey();
        if (key == 27) return;
        cv::imshow(WIN_NAME, startImage);
    }
    loop();
}


void PadGame::loop() {
    cv::setMouseCallback(WIN_NAME, NULL, NULL);
    windowImage = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);//or CV_64F or CV_32F or CV_16U etc
    cv::namedWindow(WIN_NAME, cv::WINDOW_AUTOSIZE);
    redraw();
    double pastFrame = cv::getTickCount();
    while (phase == Game) {
        if (!handleInput(cv::pollKey())) return;
        if ((cv::getTickCount() - pastFrame) / cv::getTickFrequency() > 1. / frameRate) {
            moveBall();
            pastFrame = cv::getTickCount();
        }
        if (checkCollisionsAndLoss()) {
            phase = Finish;
            break;
        }
        redraw();
    }
    finishLoop();
}

void PadGame::finishLoop() {
    cv::Mat endImage = cv::imread("T:/End.png", cv::IMREAD_COLOR);
    cv::namedWindow(WIN_NAME, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(WIN_NAME, onMouse, this);
    while (phase == Finish) {
        int key = cv::pollKey();
        if (key == 27) return;
        cv::imshow(WIN_NAME, endImage);
    }
    start();
}
