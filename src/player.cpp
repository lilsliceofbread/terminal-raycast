#ifdef __linux__
#include <ncurses.h>
#elif _WIN32
#include <curses.h>
#endif

#include <iostream>
#include <cmath>
#include "util.hpp"
#include "player.hpp"

Player::Player(float x, float y, float angle, float moveSpeed, float turnSpeed)
: mPosition({x, y}), mAngle(angle), mMoveSpeed(moveSpeed), mTurnSpeed(turnSpeed) {
    mDirVec.x = std::cos(mAngle);
    mDirVec.y = std::sin(mAngle);

    mPlaneVec.x = mDirVec.y;
    mPlaneVec.y = -mDirVec.x;
}

Player::~Player() {

}

const Vec2f Player::GetPos() const {
    return mPosition;
}

const Vec2f Player::GetDirVec() const {
    return mDirVec;
}

const Vec2f Player::GetPlaneVec() const {
    return mPlaneVec;
}

const float Player::GetAngle() const {
    return mAngle;
}

void Player::Update(uint8_t* map, int mapWidth, int mapHeight, float lastDeltaTime) {
    // correct speeds based on deltaTime
    float fMoveSpeed = mMoveSpeed * lastDeltaTime;
    float fTurnSpeed = mTurnSpeed * lastDeltaTime;

    int keyPressed = 0;
    while (keyPressed != ERR) {
        keyPressed = getch();

        switch (keyPressed) {
            case KEY_RIGHT:
                mAngle -= fTurnSpeed;

                // must recalculate vectors
                mDirVec.x = std::cos(mAngle); // dir vec is just unit vector
                mDirVec.y = std::sin(mAngle); // so is just cos and sin

                rotate2DVector(mPlaneVec, -fTurnSpeed);
                break;
            case KEY_LEFT:
                mAngle += fTurnSpeed;

                mDirVec.x = std::cos(mAngle);
                mDirVec.y = std::sin(mAngle);

                rotate2DVector(mPlaneVec, fTurnSpeed);
                break;
            case 'w':
                moveIfNoCollision(mPosition, fMoveSpeed * mDirVec.x, fMoveSpeed * mDirVec.y, map, mapWidth, mapHeight);
                break;
            case 's':
                moveIfNoCollision(mPosition, -(fMoveSpeed * mDirVec.x), -(fMoveSpeed * mDirVec.y), map, mapWidth, mapHeight);
                break;
            case 'a':
                moveIfNoCollision(mPosition, -(fMoveSpeed * mDirVec.y), fMoveSpeed * mDirVec.x, map, mapWidth, mapHeight);
                break;
            case 'd':
                moveIfNoCollision(mPosition, fMoveSpeed * mDirVec.y, -(fMoveSpeed * mDirVec.x), map, mapWidth, mapHeight);
                break;
        }
    }
    
    // constrain angle between 0 and 2PI
    if (mAngle > (2.0f * M_PI)) {
        mAngle -= 2.0f * M_PI;
    } else if (mAngle < 0) {
        mAngle += 2.0f * M_PI;
    }
}