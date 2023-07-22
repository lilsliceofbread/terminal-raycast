#pragma once

class Raycaster {
    public:
        Raycaster(uint8_t* map, int mapWidth, int mapHeight, Player* player);
        ~Raycaster();

        void InitWindow();
        void Raycast();
        void Draw();
        void Print(const std::string& str);
        //void SetUnicode();
    private:
        const Player* mPlayerPtr;   // pointer to const data (no changing)
        const uint8_t* mMapPtr; 
        int mMapWidth;
        int mMapHeight;
        int mScrWidth;
        int mScrHeight;
        float* mDistances;           // distance to each column
        uint8_t* mWallColours;       // colours of each column
        char wallChar, ceilChar, floorChar;
        WINDOW* mWindow;
        bool mHasInitWindow;
};