#include"SoundControl.h"

//is the approach to create multiple devices a good one?
void sounds::createNewSound(std::string path, int volume){
    int index = path.size() - 1; std::string temp;
    //locate the last '/' character and save the file name
    for(index; index >= 0; index--)
        if(path[index] == '/'){
            for(index++; index < path.size(); index++)
                temp+=path[index];
            names.push_back(temp);
            break;
        }
    specs.push_back({}); buffer.push_back({}); length.push_back({}); device.push_back({});
    index = specs.size()-1;
    SDL_LoadWAV(path.c_str(), &specs[index], &buffer[index], &length[index]);
    tempBuffer = buffer[index];
    SDL_MixAudioFormat(tempBuffer, buffer[index], specs[index].format, length[index], int(float(volume) * 1.28 - 128));
    buffer[index] = tempBuffer;
    device[index] = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(1, 0), 0, &specs[index], nullptr, 0); //<- customize these TODO
    return;
}

void sounds::playSound(std::string filename){
    int index;
    for(index = 0; index < names.size(); index++)
        if(names[index] == filename) break;
    SDL_QueueAudio(device[index], buffer[index], length[index]/2); //<-configure length TODO
    SDL_PauseAudioDevice(device[index], 0);
    return;
}   

void setVolume(){
    //set volume for an arleady created sound object(?) TODO
}
