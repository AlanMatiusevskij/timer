#pragma once
#include<vector>
#include<string>
#include<SDL2/SDL.h>
class sounds{
private:
    //sound variables
    std::vector<SDL_AudioSpec> specs;
    std::vector<Uint32> length;
    std::vector<Uint8*> buffer;
    std::vector<SDL_AudioDeviceID> device;
    std::vector<std::string> names; //used to quickly identify
    Uint8* tempBuffer;

public:
    //sound functions
    void createNewSound(std::string path, int volume);
    void playSound(std::string filename);
    void setVolume();
};