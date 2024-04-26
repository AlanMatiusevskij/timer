#include<cmath>
#include<iostream>
#include<Windows.h>
#include"SoundControl.h"

#include<SDL2/SDL_ttf.h>

//Global variables
    sounds soundController;
    //Window parameters
    const int wWIDTH = 200, wHEIGHT = 200;
    //Updates Per Second
    const int UPS = 60;
    //Main timer
    SDL_TimerID mainTimer;
    //Font paths
    std::string mainFont_PATH = "./fonts/OpenSans-Regular.ttf";
    //Buttons/Rects
    SDL_Rect rect[30];
    //types
    bool ONREPEAT = false;

//Functional variables
    int TIME, HOUR = 0, MIN = 0, SEC = 0, MILISEC = 0;
    int start_HOUR, start_MIN, start_SEC;
    //Timer to increase/decrease time variables faster if buttons are held
    SDL_TimerID accTimeIncr, backgroundColorChange;
    //Button areas
    SDL_Point minimizeAppArea[2], closeAppArea[2]; 
    //Others
    int cMouseX, cMouseY, r, g, b;
    //Surface and texture for conversion, temporary integers
    SDL_Surface* tSurface;
    SDL_Texture* tTexture;
    int t1, t2;

bool breakModeOn = false;
bool CURRENTBREAK = false;
//0 needs to do a break; 1 needs to start counting
int breakModeStatus = 0;
int break_HOUR = 0;
int break_MIN = 10;
int break_SEC = 0;
//Visual variables 
    struct{
        //Sets the blending intensity of border lines.
        int blendFactor = 9;
        //Sets the intensity of darkening once the user hovers over buttons.
        float hover_Darkness = 0.65;
        //Base value
        int basebg_r = 64, basebg_g = 67, basebg_b = 70;
        //Background color values.
        int bg_r = basebg_r, bg_g = basebg_g, bg_b = basebg_b;
        //values once timer hits 0
        int t_bg_r = 58, t_bg_g = 58, t_bg_b = 58;
        //How many pixels wide is the border.
        int borderPixelsWide = 5;
        //How many pixels wide is the top bar
        int topBarPixels = 10;
        //Top bar color values
        int tb_r = 32, tb_g = 32, tb_b = 32;
        //Close button dimensions
        int closeButtonWidth = 15, closeButtonHeight = topBarPixels;
        //Close button color values
        int BASE_topBarButton_r = 42, BASE_topBarButton_g = 42, BASE_topBarButton_b = 42,
            HOVERED_topBarButton_r = BASE_topBarButton_r*hover_Darkness, HOVERED_topBarButton_g = BASE_topBarButton_g*hover_Darkness, HOVERED_topBarButton_b = BASE_topBarButton_b*hover_Darkness,
            topBarSymbol_r = 20, topBarSymbol_g = 20, topBarSymbol_b = 20,
            CURRENT_topBarButton_r = BASE_topBarButton_r, CURRENT_topBarButton_g = BASE_topBarButton_g, CURRENT_topBarButton_b = BASE_topBarButton_b;
    }border; // Main window border struct;

    //Font color
    unsigned char mCR=232, mCG=232, mCB=232;

    //Main countdown variables (Countdown Rectangle.. - Individual Border Pixel Size)
        int cRWidth = 45, cRHeight = 55, cRIndBorderPixelS = 3;
        std::string s_numbs;

    //Rectangles (positions)
    SDL_Rect *rectTimes[3], rWBorder;

    struct{
        //@param [0]-running.bmp; [1]-paused.bmp; [2]-HOVERED_running.bmp; [3]-HOVERED_paused.bmp
        std::string PATH[7]{"./assets/running.bmp", "./assets/paused.bmp", "./assets/HOVERED_running.bmp", "./assets/HOVERED_paused.bmp", "./assets/arrowUP.bmp", "./assets/arrowDOWN.bmp", "./assets/repeat.bmp"}; 
        std::pair<int, int> IMGSIZE[4]{{128,128}, {128,128}, {128,128},{128,128}};
        std::pair<int,int> BUTTONSIZE[1]{{35,35}};
        bool button_pressed[1] = {false}; //@param [0] - (T)running/(F)paused;
    }BMP; //<- image format

//Functions
    Uint32 seconds_timer(Uint32 interval, void* nothing);
    void loadTexture(SDL_Renderer* renderer, std::string PATH, SDL_Rect* rectangle, Uint8 a);
    bool isOnButton(int *x, int *y, SDL_Rect* button);
    void listOfWhatButtonsDo(int* index, int* sub_index);

enum BUTTON_TYPE{
    //buttons are a,b,c,d pixels away from each border and strech to fill the whole area
    BUTTON_FIXED = 1,
    //buttons are a/b and c/d pixels away from each border and have a fixed size 
    BUTTON_STATIC = 2,
    //buttons require a pre_defined ratio value, so that their position is 'in the same place' relative to the screen size, fixed button size
    BUTTON_DYNAMIC_POSITION = 3,
    //Applied only to a dynamic button, scales the button size with window size
    DYNAMIC_BUTTON_SIZE = 4,
    // Buttons are a,b pixels away from another button
    BUTTON_FIXED_RELATIVE = 5
};
    //Draws or prepares a rectangle for a button
    //a = x1, b = y1, c=x2, b=y2
    //ratio : what part(%) of a window does the parameter take up; ratioX = wWIDTH/x or wWIDTH/w, same with Y
void drawButton(SDL_Renderer* renderer, int type, int ID, int a, int b, int c, int d, int startingWidth, int startingHeigth, int w, int h, SDL_Rect* relative){
    if(type < 1 || type > 5) std::cout << "INVALID TYPE PROVIDED TO drawButton()\n";

    if(type == BUTTON_FIXED){
        if(a == -1 || b == -1 || c == -1 || d == -1) std::cout << "provided a,b,c or d values can't be -1!\n";
            rect[ID].x = a;
            rect[ID].y = b;
            rect[ID].w = wWIDTH-c;
            rect[ID].h = wHEIGHT-d;
    }
    if(type == BUTTON_STATIC){
        if(w == -1 || h == -1) std::cout << "provided w or h can't be -1!\n";
        if(a == -1 && c == -1 || b == -1 && d == -1) std::cout << "Wrong dymensions a,b,c,d provided to drawButton(): " << a << "\n"<<b <<"\n"<< c << "\n"<<d << "\n";
        else{    
            (a!=-1) ? t1 = a : t1 = wWIDTH-c-border.borderPixelsWide;
            (b!=-1) ? t2 = b : t2 = wHEIGHT-d-border.borderPixelsWide;
            rect[ID].x = t1;
            rect[ID].y = t2;
            rect[ID].w = w;
            rect[ID].h = h;
        }
    }
    if(type == BUTTON_DYNAMIC_POSITION || type == BUTTON_DYNAMIC_POSITION + DYNAMIC_BUTTON_SIZE){
        //Argument warnings
        if(startingWidth == -1) std::cout << "provided ratioX can't be -1!\n";
        if(startingHeigth == -1) std::cout << "provided ratioY can't be -1!\n";
        if(w == -1) std::cout << "provided w can't be -1!\n";
        if(h == -1) std::cout << "provided h can't be -1!\n";
        if(a == -1) std::cout << "provided a can't be -1!\n";
        if(b == -1) std::cout << "provided b can't be -1!\n";

        //if DYNAMIC_BUTTON_SIZE
        if(type == BUTTON_DYNAMIC_POSITION + DYNAMIC_BUTTON_SIZE){
            rect[ID].x = a * (wWIDTH / startingWidth);
            rect[ID].y = b * (wHEIGHT / startingHeigth);
            //BMP.rect[ID].w = w/(ratioX);
            //BMP.rect[ID].h = h*(ratioX);
        }
        else{
            rect[ID].x = (a + w/2) * (float(wWIDTH) / startingWidth) - w/2;
            rect[ID].y = (b + h/2) * (float(wHEIGHT) / startingHeigth) - h/2;
            rect[ID].w = w;
            rect[ID].h = h;
        } 
    }
    if(type == BUTTON_FIXED_RELATIVE){
        if(relative == nullptr) std::cout << "provided 'relative' can't be a nullptr!\n";
        rect[ID].x =  relative->x + a;
        rect[ID].y = relative->y + b;
        rect[ID].w = w;
        rect[ID].h = h;
    }
    return;
}
int indx, sub_indx;
Uint32 accel_Timer(Uint32 interval, void* nothing){
    listOfWhatButtonsDo(&indx, &sub_indx);
    return 50;
}
    //Make buttons work
void buttonFunctionallity(SDL_Renderer* mainRend, std::string* path, SDL_Rect* button, int* x, int *y, int a, SDL_Event* event, int index, int sub_index){
    if(isOnButton(x, y, button)){
        loadTexture(mainRend, *path, button, a);
        if(event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT){
            listOfWhatButtonsDo(&index, &sub_index);
            indx = index; sub_indx = sub_index;
            accTimeIncr = SDL_AddTimer(650, accel_Timer, nullptr);
        }
    }
    else loadTexture(mainRend, *path, button, 255);
    if(event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
            SDL_RemoveTimer(accTimeIncr);
    return;
}

    //self explanatory
void listOfWhatButtonsDo(int* index, int* sub_index){
    //Increase timer values
    if(*index == 0){
        if(*sub_index == 0)
            (HOUR == 24) ? HOUR = 0 : HOUR++;
        if(*sub_index == 1)
            (MIN == 59) ? MIN = 0 : MIN++;
        if(*sub_index == 2)
            (SEC == 59) ? SEC = 0 : SEC++; 
        MILISEC = 1000;
    }
    //Decrease timer values
    if(*index == 1){
        if(*sub_index == 0)
            (HOUR == 0) ? HOUR = 24 : HOUR--;
        if(*sub_index == 1)
            (MIN == 0) ? MIN = 59 : MIN--;
        if(*sub_index == 2)
            (SEC == 0) ? SEC = 59 : SEC--;
        MILISEC = 1000;
    }
}

    //Get color of the hovered pixel of an image, pixel position provided that's relative to the window position 
void GetColor(int *img_index, SDL_Surface* surface, int button_index, int *x, int *y, int *r, int *g, int *b, int *a){
    //Load and prepare variables
    if(img_index != nullptr && *img_index >= 0) tSurface = SDL_LoadBMP(BMP.PATH[*img_index].c_str());
    else if(surface != nullptr) tSurface = surface;
    else std::cout << "Invalid surface or image_index provided to GetColor(): " << SDL_GetError() << "\n";
    if(button_index < 0) std::cout << "Invadild button index provided to GetColor(" << img_index << ", " << surface << ", " << button_index << ", " << x << ", "<< y << ")\n"; 
    SDL_LockSurface(tSurface);

    //Set up positions relative to the image
    *x = (*x - rect[button_index].x) * (BMP.IMGSIZE[*img_index].first/BMP.BUTTONSIZE[button_index].first);
    *y = (*y - rect[button_index].y) * (BMP.IMGSIZE[*img_index].second/BMP.BUTTONSIZE[button_index].second);
    
    //Get pixel information
    int BPP = tSurface->format->BitsPerPixel;
    Uint8* pixel = (Uint8*)tSurface->pixels + *y * tSurface->pitch + *x * BPP;
    Uint32 DATA = *(Uint32*)pixel;

    //Get color values and finish up
    SDL_GetRGBA(DATA, tSurface->format, (Uint8*)r, (Uint8*)g, (Uint8*)b, (Uint8*)a);
    SDL_UnlockSurface(tSurface);
    return;
}
    //Checks if cursor is on a specific button
bool isOnButton(int *x, int *y, SDL_Rect* button){
    if(*x >= button->x && *x < button->w + button->x && *y >= button->y && *y < button->h + button->y) return true;
    return false;
}

    //Send a texture to a buffer from a specified .bmp image
void loadTexture(SDL_Renderer* renderer, std::string PATH, SDL_Rect* rectangle, Uint8 a){
    SDL_FreeSurface(tSurface);
    tSurface = SDL_LoadBMP(PATH.c_str()); if(tSurface == nullptr) std::cout << "Failed to load " << PATH << ": " << SDL_GetError() <<"\n";
    if(a != -1) SDL_SetSurfaceAlphaMod(tSurface, a);
    SDL_DestroyTexture(tTexture);
    tTexture = SDL_CreateTextureFromSurface(renderer, tSurface);
    SDL_SetTextureBlendMode(tTexture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(renderer, tTexture, NULL, rectangle);
    return;
}   
    //Allow to drag the window
SDL_HitTestResult callBack(SDL_Window* window, const SDL_Point* area, void*data){
    if(area->x > 0 && area->x <wWIDTH - 2*border.closeButtonWidth-1 && area->y < border.topBarPixels)
        return SDL_HITTEST_DRAGGABLE;
    return SDL_HITTEST_NORMAL;
}
    //Initialize various variables and libraries
void INITIALIZE(SDL_Window**mainWind, SDL_Renderer**mainRend, TTF_Font**mFont){
    //Initialize libraries
    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0) std::cout<<"SDL FAILED to initialize: " << SDL_GetError() << "\n";
    if(TTF_Init()!= 0) std::cout<<"TTF FAILED to initialize: " << TTF_GetError() << "\n";
    //Load fonts
    *mFont = TTF_OpenFont(mainFont_PATH.c_str(), 38);
    if(mFont==nullptr) std::cout << "FAILED to load font 'OpenSans-Regular.ttf': " << TTF_GetError() << "\n";

    //Create window, renderer and event handler;
    *mainWind = SDL_CreateWindow("Timer v1.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wWIDTH, wHEIGHT, SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS);
    *mainRend = SDL_CreateRenderer(*mainWind, -1, 0);
    //Hit test
    SDL_SetWindowHitTest(*mainWind, callBack, nullptr);
    
    //load sounds
    soundController.createNewSound("./assets/sound.wav", 30);

    //Define SDL_Rects
    //Hour
    drawButton(*mainRend, BUTTON_DYNAMIC_POSITION, 1, round(29), 90, -1, -1, 200, 300, 45, 55, nullptr);
    //:
    drawButton(*mainRend, BUTTON_DYNAMIC_POSITION, 2, round(73), 90, -1, -1, 200, 300, 8, 55, nullptr);
    //Min
    drawButton(*mainRend, BUTTON_DYNAMIC_POSITION, 3, round(80), 90, -1, -1, 200, 300, 45, 55, nullptr);
    //:
    drawButton(*mainRend, BUTTON_DYNAMIC_POSITION, 4, round(124), 90, -1, -1, 200, 300, 8, 55, nullptr);
    //Sec
    drawButton(*mainRend, BUTTON_DYNAMIC_POSITION, 5, round(131), 90, -1, -1, 200, 300, 45, 55, nullptr);
    //Arrow up buttons
    drawButton(*mainRend, BUTTON_FIXED_RELATIVE, 6, 6, 50, -1, -1, -1, -1, 33, 14, &rect[1]);
    drawButton(*mainRend, BUTTON_FIXED_RELATIVE, 7, 6, 50, -1, -1, -1, -1, 33, 14, &rect[3]);
    drawButton(*mainRend, BUTTON_FIXED_RELATIVE, 8, 6, 50, -1, -1, -1, -1, 33, 14, &rect[5]);
    //Arrow down buttons
    drawButton(*mainRend, BUTTON_FIXED_RELATIVE, 9, 0, 16, -1, -1, -1, -1, 33, 14, &rect[6]);
    drawButton(*mainRend, BUTTON_FIXED_RELATIVE, 10, 0, 16, -1, -1, -1, -1, 33, 14, &rect[7]);
    drawButton(*mainRend, BUTTON_FIXED_RELATIVE, 11, 0, 16, -1, -1, -1, -1, 33, 14, &rect[8]);
    //Pause/Continue
    drawButton(*mainRend, BUTTON_DYNAMIC_POSITION, 0, round(82.5), 220, -1, -1, 200, 300, 35, 35, nullptr);
    //repeat;
    drawButton(*mainRend, BUTTON_STATIC, 12, -1, -1, 26,25, -1,-1, 26,20, nullptr);
    //Work mode
    drawButton(*mainRend, BUTTON_STATIC, 13, border.borderPixelsWide + 5, border.topBarPixels + 5, -1, -1, -1, -1, 85, 17, nullptr);
    //Current breakmode?
    int tmp_width = 65;
    drawButton(*mainRend, BUTTON_FIXED_RELATIVE, 14, rect[3].w/2-tmp_width/2, -7, -1, -1, -1 , -1, tmp_width, 18, &rect[3]);

    //Define the areas (points, rects) of buttons
    minimizeAppArea[0].x = wWIDTH-1-2*border.closeButtonWidth; minimizeAppArea[0].y = 0; minimizeAppArea[1].x = wWIDTH-2-border.closeButtonWidth; minimizeAppArea[1].y = border.topBarPixels;
    closeAppArea[0].x = wWIDTH-1-border.closeButtonWidth; closeAppArea[0].y = 0; closeAppArea[1].x = wWIDTH-1; closeAppArea[1].y = border.topBarPixels;

    //Check for errors
    if(mainWind == NULL) std::cout <<"FAILED to create a window: " << SDL_GetError() << "\n"; 
    if(mainRend == NULL) std::cout <<"FAILED to create a renderer: " << SDL_GetError() << "\n";
}

int timer_color_change_repetitions = 0;
Uint32 bg_color_change_at_end(Uint32 interval, void* nothing){
    timer_color_change_repetitions++;
    //COMPARING ONLY RED COLOR
    if(border.bg_r == border.basebg_r){
        border.bg_r = border.t_bg_r;
        border.bg_g = border.t_bg_g;
        border.bg_b = border.t_bg_b;
    }
    else{
        border.bg_r = border.basebg_r;
        border.bg_g = border.basebg_g;
        border.bg_b = border.basebg_b;
    }

    if(timer_color_change_repetitions > 7) {timer_color_change_repetitions = 0;return 0;}
    else return 250;
}
//call when timer hits 0;
void timer_finished(){
    CURRENTBREAK = false;
    if((!ONREPEAT && !breakModeOn) || (!ONREPEAT && breakModeOn && breakModeStatus == 1)){
        BMP.button_pressed[0] = false;
        breakModeStatus = 0;
    }
    else if((ONREPEAT && !breakModeOn) || (ONREPEAT && breakModeOn && breakModeStatus == 1)){
        SDL_RemoveTimer(mainTimer);
        breakModeStatus = 0;
        HOUR = start_HOUR;
        MIN = start_MIN;
        SEC = start_SEC;
        MILISEC = 1000;
        mainTimer = SDL_AddTimer((1/float(UPS))*1000, seconds_timer, nullptr);
    }
    else if(breakModeOn && breakModeStatus == 0){
        CURRENTBREAK = true;
        SDL_RemoveTimer(mainTimer);
        breakModeStatus = 1;
        HOUR = break_HOUR;
        MIN = break_MIN;
        SEC = break_SEC;
        MILISEC = 1000;
        mainTimer = SDL_AddTimer((1/float(UPS))*1000, seconds_timer, nullptr);
    }
    SDL_RemoveTimer(backgroundColorChange);
    timer_color_change_repetitions = 0;
    border.bg_r = border.basebg_r;
    border.bg_g = border.basebg_g;
    border.bg_b = border.basebg_b;
    backgroundColorChange = SDL_AddTimer(0, bg_color_change_at_end, nullptr);
    soundController.playSound("sound.wav");

    return;
}

    //Main Timer ( updates the application at the rate of UPS)
Uint32 seconds_timer(Uint32 interval, void* nothing){
    if(BMP.button_pressed[0]){
        TIME = 1000*(60*60*HOUR + 60*MIN + SEC) + MILISEC - interval;
            //Update time values
            if(TIME<0) TIME = 0;
            HOUR = (TIME/1000)/(60*60);
            MIN = (TIME/1000)%(60*60)/60;
            SEC = (TIME/1000)%(60*60)%60;
            MILISEC = TIME%1000;
            if(TIME<=0){
                timer_finished();
                return 0;
            }
    }
    return interval;
}
    //Load the countdown numbers
void countdownText(int NUMBER, SDL_Renderer* renderer, TTF_Font* font){
    s_numbs = std::to_string(NUMBER);
    if(s_numbs=="") s_numbs = "00";
    else if(NUMBER == 0) s_numbs += "0";
    else if(NUMBER < 10) s_numbs = "0" + s_numbs;
    SDL_FreeSurface(tSurface);
    tSurface = TTF_RenderUTF8_Blended(font, s_numbs.c_str(), {mCR,mCG,mCB,255});
    SDL_DestroyTexture(tTexture);
    tTexture = SDL_CreateTextureFromSurface(renderer, tSurface);
}
void loadText(std::string text, SDL_Renderer *renderer, TTF_Font * font){
    SDL_FreeSurface(tSurface);
    tSurface = TTF_RenderUTF8_Blended(font, text.c_str(), {mCR,mCG,mCB,255});
    SDL_DestroyTexture(tTexture);
    tTexture = SDL_CreateTextureFromSurface(renderer, tSurface);
}
Uint8 tempAlfa;
void breakMode(SDL_Renderer * rend, TTF_Font *font, SDL_Event evt){
    if(breakModeOn){
        loadText("Break mode: on", rend, font);
        SDL_SetTextureAlphaMod(tTexture, 150);
    }
    else loadText("Break mode: off", rend, font);

    if(isOnButton(&cMouseX, &cMouseY, &rect[13])){
        SDL_GetTextureAlphaMod(tTexture, &tempAlfa);
        SDL_SetTextureAlphaMod(tTexture, int(tempAlfa) - 50);
        if(evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT)
            breakModeOn = !breakModeOn;
    }

    SDL_RenderCopy(rend, tTexture, NULL,&rect[13]);
}

int main(int argc, char *argv[]){
    //Hide console ( once hidden terminal stops outputting )
    FreeConsole();
    //Core variables
    SDL_Window* mainWind; SDL_Renderer *mainRend; TTF_Font *mFont; SDL_Event event;
    INITIALIZE(&mainWind, &mainRend, &mFont);

    //Main loop
    while(true){
        SDL_PollEvent(&event);

    //Render - drawing to buffer
        //Clear the application and draw a background
        SDL_SetRenderDrawColor(mainRend, border.bg_r, border.bg_g, border.bg_b, 255);
        SDL_RenderClear(mainRend);
        //Recolor the top bar
        for(int i = 0; i < border.topBarPixels;i++){
            SDL_SetRenderDrawColor(mainRend, border.tb_r+i*0.6, border.tb_g+i*0.6, border.tb_b+i*0.6, 255);
            SDL_RenderDrawLine(mainRend, 0, i, wWIDTH-1, i);
        }
        //Draw a border 'bwPixels' -wide, not using SDL_DrawRects because no fancy colors then;
        for(int index = 0; index < border.borderPixelsWide; index++){
            SDL_SetRenderDrawColor(mainRend, 26+border.blendFactor*index, 26+border.blendFactor*index, 26+border.blendFactor*index, 255);
            rWBorder.x = index; rWBorder.y = border.topBarPixels+index; rWBorder.w = wWIDTH-index*2; rWBorder.h = wHEIGHT-index*2-border.topBarPixels;
            SDL_RenderDrawRect(mainRend, &rWBorder);
        }

        //Get mouse position relative to the window
        SDL_GetMouseState(&cMouseX, &cMouseY);

        //Draw time values
        SDL_SetRenderDrawColor(mainRend, 255, 255, 255, 255);     
        countdownText(HOUR, mainRend, mFont); SDL_RenderCopy(mainRend, tTexture, NULL,&rect[1]);
        countdownText(MIN, mainRend, mFont); SDL_RenderCopy(mainRend, tTexture, NULL,&rect[3]);
        countdownText(SEC, mainRend, mFont); SDL_RenderCopy(mainRend, tTexture, NULL,&rect[5]);
        //Draw two ':'
        loadText(":", mainRend, mFont); SDL_RenderCopy(mainRend, tTexture, NULL,&rect[2]); SDL_RenderCopy(mainRend, tTexture, NULL,&rect[4]);
        //Draw the Break mode text
        breakMode(mainRend, mFont, event);


        //Minimise button
            //Update colors if hovering, minimise if pressing LMB
        if(cMouseX >= minimizeAppArea[0].x && cMouseX <=minimizeAppArea[1].x && cMouseY >= minimizeAppArea[0].y && cMouseY <=minimizeAppArea[1].y){
            border.CURRENT_topBarButton_r = border.HOVERED_topBarButton_r; border.CURRENT_topBarButton_g = border.HOVERED_topBarButton_g; border.CURRENT_topBarButton_b = border.HOVERED_topBarButton_b; 
            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                SDL_MinimizeWindow(mainWind);
        }
        else{
            border.CURRENT_topBarButton_r = border.BASE_topBarButton_r; border.CURRENT_topBarButton_g = border.BASE_topBarButton_g; border.CURRENT_topBarButton_b = border.BASE_topBarButton_b;}
            //Draw a 'minimize' button and its symbol
        for(int i = 0; i < border.closeButtonWidth; i++){
            SDL_SetRenderDrawColor(mainRend, border.CURRENT_topBarButton_r+i*0.5, border.CURRENT_topBarButton_g+i*0.5, border.CURRENT_topBarButton_b+i*0.5, 255);
            SDL_RenderDrawLine(mainRend, wWIDTH-2-i-border.closeButtonWidth, 0, wWIDTH-2-i-border.closeButtonWidth, border.topBarPixels-1);
        }
        SDL_SetRenderDrawColor(mainRend, border.topBarSymbol_r, border.topBarSymbol_g, border.topBarSymbol_b, 255);
        SDL_RenderDrawLine(mainRend, minimizeAppArea->x + 5, 5, minimizeAppArea->x + border.closeButtonWidth-5, 5); //<- Minimise button symbol position
        
        //Close button
            //Update colors if hovering, close if pressing LMB  
        if(cMouseX >= closeAppArea[0].x && cMouseX <=closeAppArea[1].x && cMouseY >= closeAppArea[0].y && cMouseY <=closeAppArea[1].y){
            border.CURRENT_topBarButton_r = border.HOVERED_topBarButton_r; border.CURRENT_topBarButton_g = border.HOVERED_topBarButton_g; border.CURRENT_topBarButton_b = border.HOVERED_topBarButton_b;
            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                break;
        }
        else{
            border.CURRENT_topBarButton_r = border.BASE_topBarButton_r; border.CURRENT_topBarButton_g = border.BASE_topBarButton_g; border.CURRENT_topBarButton_b = border.BASE_topBarButton_b;}
            //Draw a 'close' button and its symbol
        for(int i = 0; i < border.closeButtonWidth; i++){
            SDL_SetRenderDrawColor(mainRend, border.CURRENT_topBarButton_r+i*0.5, border.CURRENT_topBarButton_g+i*0.5, border.CURRENT_topBarButton_b+i*0.5, 255);
            SDL_RenderDrawLine(mainRend, wWIDTH-1-i, 0, wWIDTH-1-i, border.topBarPixels-1);
        }
        SDL_SetRenderDrawColor(mainRend, border.topBarSymbol_r, border.topBarSymbol_g, border.BASE_topBarButton_b, 255);
        SDL_RenderDrawLine(mainRend, closeAppArea->x + 6, 3, closeAppArea->x + 10, 7); //<- Close button symbol position
        SDL_RenderDrawLine(mainRend, closeAppArea->x + 10, 3, closeAppArea->x + 6, 7); //<- Close button symbol position
        
        //Load images
            //continue/stop countodwn
        if(BMP.button_pressed[0]){//running.bmp
            //if on button
            if(isOnButton(&cMouseX, &cMouseY, &rect[0])){
                loadTexture(mainRend, BMP.PATH[2], &rect[0], -1);
                //if pressed
                if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
                    BMP.button_pressed[0] = false;
                    SDL_RemoveTimer(mainTimer);
                }
            }
            else
                loadTexture(mainRend, BMP.PATH[0], &rect[0], -1);
        }
        else{//paused.bmp
            //if on button
            if(isOnButton(&cMouseX, &cMouseY, &rect[0])){
                loadTexture(mainRend, BMP.PATH[3], &rect[0], -1);
                //if pressed
                if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT &&( HOUR != 0 || MIN != 0 || SEC != 0)){
                    start_HOUR = HOUR;
                    start_MIN = MIN;
                    start_SEC = SEC;
                    BMP.button_pressed[0] = true;
                    mainTimer = SDL_AddTimer((1/float(UPS))*1000, seconds_timer, nullptr);
                }
            }
            else
                loadTexture(mainRend, BMP.PATH[1], &rect[0], -1);
        }

        //mention if currently its break mode
        if(CURRENTBREAK){
            loadText("Break time", mainRend, mFont); 
            SDL_RenderCopy(mainRend, tTexture, NULL, &rect[14]);
        }
        //Arrow buttons
        if(!BMP.button_pressed[0]){ //if countdown -isn't- going
            //up:
            buttonFunctionallity(mainRend, &BMP.PATH[4], &rect[6], &cMouseX, &cMouseY, 130, &event, 0, 0);
            buttonFunctionallity(mainRend, &BMP.PATH[4], &rect[7], &cMouseX, &cMouseY, 130, &event, 0, 1);
            buttonFunctionallity(mainRend, &BMP.PATH[4], &rect[8], &cMouseX, &cMouseY, 130, &event, 0, 2);
            //Down:
            buttonFunctionallity(mainRend, &BMP.PATH[5], &rect[9], &cMouseX, &cMouseY, 130, &event, 1, 0);
            buttonFunctionallity(mainRend, &BMP.PATH[5], &rect[10], &cMouseX, &cMouseY, 130, &event, 1, 1);
            buttonFunctionallity(mainRend, &BMP.PATH[5], &rect[11], &cMouseX, &cMouseY, 130, &event, 1, 2);
        }
        //repeat
        if(isOnButton(&cMouseX, &cMouseY, &rect[12])){
            if(ONREPEAT)loadTexture(mainRend, BMP.PATH[6], &rect[12], 50);
            else loadTexture(mainRend, BMP.PATH[6], &rect[12], 150);
            //if pressed
            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                ONREPEAT = !ONREPEAT;
        }
        else if(ONREPEAT) loadTexture(mainRend, BMP.PATH[6], &rect[12], 80);
        else loadTexture(mainRend, BMP.PATH[6], &rect[12], -1);
    //Ending drawing

    if(event.type == SDL_QUIT) break;
    //Update the application
        SDL_RenderPresent(mainRend);
    //Wait time
        SDL_Delay(int((1/float(UPS))*1000));
    }

    //Quit all processes
    SDL_RemoveTimer(mainTimer);
    TTF_CloseFont(mFont);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
//Unused code:
////Time difference logic
    // now = std::chrono::system_clock::now();
    // if(static_cast<std::chrono::duration<float>>(now-ex_time).count() >= 1){
    //     ex_time = now;
    //     sec_passed = true;
    // }

//TODO list: 
    //when unfocused, the buttons dont react immedaitelly. tryyy to fix it ._.
    // a toast/-https://learn.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/send-local-toast-cpp-uwp?tabs=builder-syntax
    //tray 
    //break mode? (10min timer, 5 min 'rest')
    //doesnt change to the current audio device 
    // sounds too loud, aut ochange default device