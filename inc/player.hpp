#pragma once

class Player {
    public:
        Player(float x, float y, float angle, float moveSpeed, float turnSpeed);
        ~Player();

        void Update(uint8_t* map, int mapWidth, int mapHeight, float lastDeltaTime);

        const Vec2f GetPos() const;
        const Vec2f GetDirVec() const;
        const Vec2f GetPlaneVec() const;
        const float GetAngle() const;

    private:
        Vec2f mPosition;
        Vec2f mDirVec;
        Vec2f mPlaneVec;
        float mAngle;
        float mMoveSpeed;
        float mTurnSpeed;
};