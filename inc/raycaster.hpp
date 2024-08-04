#pragma once

class Raycaster
{
    public:
        Raycaster(uint8_t* map, int mapWidth, int mapHeight, Player* player);
        ~Raycaster();

        void Raycast();
        void Draw();
        void Print(const std::string& str);
    private:
        const Player* mPlayerPtr;
        const uint8_t* mMapPtr; 
        int mMapWidth;
        int mMapHeight;
        int mScrWidth;
        int mScrHeight;
        float* mDistances;           // distance to each column
        uint8_t* mWallColours;       // colours of each column
        //WINDOW* mWindow;
        bool mHasInitWindow;
        bool mUseColour;
};