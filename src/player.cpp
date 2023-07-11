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

void Player::Input(uint8_t* map, int mapWidth, int mapHeight, float lastDeltaTime) {
        // correct speeds based on deltaTime
        float fMoveSpeed = mMoveSpeed * lastDeltaTime;
        float fTurnSpeed = mTurnSpeed * lastDeltaTime;

        if(getKeyPressed(XK_Right)) {
            mAngle -= fTurnSpeed;

            // must recalculate vectors
            mDirVec.x = std::cos(mAngle); // dir vec is just unit vector
            mDirVec.y = std::sin(mAngle); // so is just cos and sin

            rotate2DVector(mPlaneVec, -fTurnSpeed);
        } else if (getKeyPressed(XK_Left)) {
            mAngle += fTurnSpeed;
            
            mDirVec.x = std::cos(mAngle); 
            mDirVec.y = std::sin(mAngle);

            rotate2DVector(mPlaneVec, fTurnSpeed);
        }

        // prevent mAngle > 360 or < 0
        if(mAngle > (2.0f * M_PI)) {
            mAngle -= 2.0f * M_PI;
        } else if (mAngle < 0) {
            mAngle += 2.0f * M_PI;
        }

        if(getKeyPressed(XK_w) && !getKeyPressed(XK_s)) {
            moveIfNoCollision(mPosition, fMoveSpeed * mDirVec.x, fMoveSpeed * mDirVec.y, map, mapWidth, mapHeight);
        } else if(getKeyPressed(XK_s) && !getKeyPressed(XK_w)) {
            moveIfNoCollision(mPosition, -(fMoveSpeed * mDirVec.x), -(fMoveSpeed * mDirVec.y), map, mapWidth, mapHeight);
        }

        // perpendicular vector is sin and -cos or -sin and cos
        if(getKeyPressed(XK_a) && !getKeyPressed(XK_d)) {
            moveIfNoCollision(mPosition, -(fMoveSpeed * mDirVec.y), fMoveSpeed * mDirVec.x, map, mapWidth, mapHeight);
        } else if(getKeyPressed(XK_d) && !getKeyPressed(XK_a)) {
            moveIfNoCollision(mPosition, fMoveSpeed * mDirVec.y, -(fMoveSpeed * mDirVec.x), map, mapWidth, mapHeight);
        } 
}