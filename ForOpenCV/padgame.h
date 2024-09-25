#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>//or #include <opencv2/opencv.hpp>s
#include <iostream>      

class Object2D {
public:
    cv::Vec2d xyPos;
    virtual void draw(cv::Mat& image) {};
    virtual bool isConfined(int windowWidth, int windowHeight, int& mask) { return false; };
};

class Ball : public Object2D {
public:
    int radius;

    Ball(int xPos, int yPos, int radius) { this->xyPos[0] = xPos; this->xyPos[1] = yPos; this->radius = radius; };

    void draw(cv::Mat& image) {
        cv::circle(image, cv::Point(xyPos[0], xyPos[1]), radius, cv::Scalar(255, 255, 255), cv::FILLED);
    }

    bool isConfined(int windowWidth, int windowHeight, bool& isHorizontal) {
        isHorizontal = !(xyPos[0] - radius > 0 & xyPos[0] + radius < windowWidth);
        return xyPos[0] > radius & xyPos[1] > radius & xyPos[0] + radius < windowWidth & xyPos[1] + radius < windowHeight;
    }
};

class Pad : public Object2D {
public:
    int width, height;

    Pad(int xPos, int yPos, int width, int height) {
        this->xyPos[0] = xPos; this->xyPos[1] = yPos;
        this->width = width; this->height = height;
    }

    void draw(cv::Mat& image) {
        cv::rectangle(image, cv::Point(xyPos[0], xyPos[1]), cv::Point(xyPos[0] + width, xyPos[1] + height), cv::Scalar(255, 255, 255), cv::FILLED);
    }

    bool isConfined(int windowWidth, int windowHeight) {
        return xyPos[0] > 0 & xyPos[1] > 0 & xyPos[0] + width < windowWidth & xyPos[1] + height < windowHeight;
    }
};


#define WIDTH 800
#define HEIGHT 600
#define WIN_NAME "Pad Bounce"

class PadGame {
public:
    enum GamePhase {
        Start,
        Game,
        Finish
    };
    cv::Mat windowImage;
    const double frameRate = 60;
    static GamePhase phase;

    const double padSpeed = 20, ballSpeed = 300;
    double ballAngle;
    cv::Mat ballDirection;
    Pad pad = Pad(WIDTH / 2, HEIGHT - (HEIGHT / 40), WIDTH / 10, HEIGHT / 40);
    Ball ball = Ball(rand() % WIDTH, rand() % (HEIGHT / 2), HEIGHT / 40);
    
    void onMouse(int event, int x, int y);
    static void onMouse(int event, int x, int y, int flags, void* userdata);

    void start();
private:

    bool collisionFlag = false;
    void startLoop();
    void loop();
    void finishLoop();

    void moveBall() {
        ball.xyPos[0] += ballDirection.at<double>(0, 0) * ballSpeed * 1. / frameRate;
        ball.xyPos[1] += ballDirection.at<double>(1, 0) * ballSpeed * 1. / frameRate;
    }

    bool areColliding(Pad pad, Ball ball) {
        return ball.xyPos[0] + ball.radius > pad.xyPos[0]
            && ball.xyPos[0] - ball.radius < pad.xyPos[0] + pad.width
            && ball.xyPos[1] + ball.radius > pad.xyPos[1]
            && ball.xyPos[1] - ball.radius < pad.xyPos[1] + pad.height;//last check unimportant and sometimes is bad
    }

    bool checkCollisionsAndLoss() {
        if (areColliding(pad, ball)) {
            if (collisionFlag) return false;
            ballAngle = -ballAngle;
            ballDirection = cv::getRotationMatrix2D(cv::Point2f{ 0, 0 }, ballAngle, 1.).reshape(1).t(); //* cv::Mat(ballDirection).reshape(1)
            collisionFlag = true;
        }
        else collisionFlag = false;


        bool isHorizontal;
        if (!pad.isConfined(WIDTH, HEIGHT)) {
            pad.xyPos[0] = pad.xyPos[0] > WIDTH - pad.width ? WIDTH - pad.width : (pad.xyPos[0] < 0 ? 0 : pad.xyPos[0]);
            pad.xyPos[1] = pad.xyPos[1] > HEIGHT - pad.height ? HEIGHT - pad.height : (pad.xyPos[1] < 0 ? 0 : pad.xyPos[1]);
        }
        if (!ball.isConfined(WIDTH, HEIGHT, isHorizontal)) {
            if (ball.xyPos[1] > HEIGHT - ball.radius) return true;
            ball.xyPos[0] = ball.xyPos[0] > WIDTH - ball.radius ? WIDTH - ball.radius : (ball.xyPos[0] < ball.radius ? ball.radius : ball.xyPos[0]);
            ball.xyPos[1] = ball.xyPos[1] > HEIGHT - ball.radius ? HEIGHT - ball.radius : (ball.xyPos[1] < ball.radius ? ball.radius : ball.xyPos[1]);
            ballAngle = isHorizontal ? 180 - ballAngle : -ballAngle;
            ballDirection = cv::getRotationMatrix2D(cv::Point2f{ 0, 0 }, ballAngle, 1.).reshape(1).t();
        }
        return false;
    }

    bool handleInput(int key) {
        if (key == 27) return false;
        switch (key) {
        case 'a':
            pad.xyPos[0] = pad.xyPos[0] - padSpeed;
            break;
        case 'd':
            pad.xyPos[0] = pad.xyPos[0] + padSpeed;
            break;
        }
        return true;
    }

    void redraw() {
        cv::rectangle(windowImage, cv::Point(0, 0), cv::Point(WIDTH, HEIGHT), cv::Scalar(0, 0, 0), cv::FILLED);
        pad.draw(windowImage);
        ball.draw(windowImage);
        cv::imshow(WIN_NAME, windowImage);
    }
};
