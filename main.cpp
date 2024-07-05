#include<customsdl/customSDL.h>



int main(int argc, char* argv[]){
    //Init:
    //FreeConsole();
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window *wind = SDL_CreateWindow("Timer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 200, 0);
    SDL_Renderer *rend = SDL_CreateRenderer(wind, -1, 0);
    SDL_Event evt;
    SDL_SetRenderDrawBlendMode(rend, SDL_BlendMode::SDL_BLENDMODE_ADD);


    while(true){
        SDL_PollEvent(&evt);
        SDL_SetRenderDrawColor(rend, 0,0,0,0);
        SDL_RenderClear(rend);

        customcpp::upsAverage(true);
        if(evt.type == SDL_QUIT) break;
        SDL_RenderPresent(rend);
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}