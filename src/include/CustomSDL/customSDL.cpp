#include <CustomSDL/customSDL.h>
using namespace customcpp;

////////////////////////////////

customsdl::UI *customsdl::UI::ui = nullptr;

///////////////////////////////////////////

void customsdl::UI::SETUP(SDL_Renderer *renderer, SDL_Event *event, int width, int height, color outline_color, color hovered_color, color bar_color){
    static color outline_clr_p = outline_color;
    static color hovered_clr_p = hovered_color;
    static color bar_clr_p = bar_color;

    ui->vars = {&outline_clr_p, &hovered_clr_p, &bar_clr_p, width, height, renderer, event};
}

SDL_Surface *customsdl::UI::text::getSurface(std::string text, color rgba, SDL_Rect textBox, font *fontData, Uint32 flags){
    if(last.text == text && compareRects(textBox, last.TextBox) && last.fontData.fontPath == fontData->fontPath && last.fontData.fontSize == fontData->fontSize && last.rgba.r == rgba.r && last.rgba.g == rgba.g && last.rgba.b == rgba.b && last.rgba.a == rgba.a && last.surface != nullptr)
        return last.surface;
    wordLength.clear();
    SDL_DestroyTexture(last.texture);
    SDL_FreeSurface(last.surface);
    last.texture = nullptr;
    std::vector<std::string> words;
    std::string ind_word{""};

    FT_FaceRec_* FACE = ui->useFont(fontData->fontPath, fontData->fontSize);

    //Save each word and whitespaces in a vector
    for(char symb : text){
        if(symb == '\n'){
            words.push_back(ind_word);
            ind_word = "\n";
        }
        else if(symb == ' '){
            words.push_back(ind_word);
            ind_word = "";
        }
        else ind_word += symb;
    }
    //save the last one.
    words.push_back(ind_word);

    //Find the total width and height.
    int cWidth = 1;
    int cHeight = 1;
    int wordWidth = 0;
    int maxWidth = 0;
    int maxHeight = 0;

    for(std::string word : words){
        for(char symb : word){
            FT_Load_Char(FACE, symb, FT_LOAD_BITMAP_METRICS_ONLY);
            cWidth+=FACE->glyph->bitmap.width;
            wordWidth+=FACE->glyph->bitmap.width;
            if(((flags & AUTONEWLINES) && cWidth >= textBox.w) || symb == '\n'){
                cHeight += fontData->fontSize+1;
                cWidth = wordWidth+1;
            }
        }
        wordLength.push_back(wordWidth);
        wordWidth = 0;
        cWidth+=fontData->fontSize/4;
        maxWidth = std::max(maxWidth, cWidth);
    }
    maxHeight = cHeight+fontData->fontSize+4;

    //Create a surface where the sentence will be stored.
    surfaceManipulation manip;
    manip.createSurface(maxWidth, maxHeight, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_UnlockSurface(manip.active_surface);
    SDL_SetSurfaceBlendMode(manip.active_surface, SDL_BlendMode::SDL_BLENDMODE_BLEND);

    //Load each glyph surface and merge them into a single surface.
    cWidth = 1; cHeight = 1;
    int belowBaseLine{};

    for(int i = 0; i < words.size(); i++){
        if(((flags & AUTONEWLINES) && i != 0 && cWidth+wordLength[i] >= textBox.w) || (words[i][0] == '\n' && i!=0)){
            cHeight+=fontData->fontSize+1;
            cWidth = 1;
        }
        for(char symb : words[i]){
            FT_Load_Char(FACE, symb, FT_LOAD_RENDER);
            belowBaseLine = (FACE->glyph->metrics.height - FACE->glyph->metrics.horiBearingY)/55;
            SDL_Surface *glyph;

            glyph = ui->surf8bitTo32bit(SDL_CreateRGBSurfaceFrom(FACE->glyph->bitmap.buffer, FACE->glyph->bitmap.width, FACE->glyph->bitmap.rows, 8, FACE->glyph->bitmap.pitch, 0, 0, 0, 0xFF));
            blitSurface({cWidth, cHeight-glyph->h+fontData->fontSize-fontData->fontSize/5+belowBaseLine}, manip.active_surface, glyph);
            SDL_FreeSurface(glyph);

            //Update some info.
            cWidth+=FACE->glyph->bitmap.width;
        }
        cWidth+=fontData->fontSize/4;
    }
    
    //Store values
    last.rgba = rgba;
    last.surface = manip.active_surface;
    last.fontData = *fontData;
    last.text = text;
    last.textDimensions = {textBox.x, textBox.y, maxWidth, maxHeight};
    last.TextBox = textBox;

    //Modulate RGB values.
    for(int x = 0; x < maxWidth; x++){
        for(int y = 0; y < maxHeight; y++){
            color src = getSurfaceColors({x, y}, manip.active_surface, false);
                //Roughing up the edges, because anti aliasing makes them look terrible. More or less works, for now.
            int buffed = src.a; buffed *=2; if(buffed > 255) buffed = 255;
            buffed = buffed*(float)rgba.a/255;
            manip.drawToSurface({x, y}, {(Uint8)(int(src.r*(float)rgba.r/255) << 0), (Uint8)(int(src.g*(float)rgba.g/255) << 0), (Uint8)(int(src.b*(float)rgba.b/255) << 0), (Uint8)(buffed << 0)});
        }
    }

    return manip.active_surface;
}

 SDL_Texture *customsdl::UI::text::getTexture(std::string text, color rgba, SDL_Rect textBox, font *fontData, Uint32 flags){
    if(last.text == text && compareRects(textBox, last.TextBox) && last.fontData.fontPath == fontData->fontPath && last.fontData.fontSize == fontData->fontSize && last.rgba.r == rgba.r && last.rgba.g == rgba.g && last.rgba.b == rgba.b && last.rgba.a == rgba.a && last.texture != nullptr)
        return last.texture;
    last.texture = SDL_CreateTextureFromSurface(vars.renderer, getSurface(text, rgba, textBox, fontData, flags));
    SDL_SetTextureBlendMode(last.texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);

    return last.texture;
 }

SDL_Surface* customsdl::UI::surf8bitTo32bit(SDL_Surface* _8bit){
    SDL_SetPaletteColors(_8bit->format->palette, _8bitpalletecolors, 0, 256);
    //SDL_SetSurfaceBlendMode(_8bit, SDL_BlendMode::SDL_BLENDMODE_BLEND); 
    SDL_Surface* _return = SDL_ConvertSurfaceFormat(_8bit, SDL_PIXELFORMAT_RGBA32, 0);

    SDL_FreeSurface(_8bit);
    return _return;
}

void customsdl::UI::text::renderText(std::string text, color rgba, SDL_Rect textBox, font *fontData, Uint32 flags){
    last.texture = getTexture(text, rgba, textBox, fontData, flags);
    SDL_Rect dest = textBox;
    if(flags & UIFLAGS::XCENTERED) dest.x = textBox.x + textBox.w/2 - last.textDimensions.w/2;
    if(flags & UIFLAGS::YCENTERED) dest.y = textBox.y + textBox.h/2 - last.textDimensions.h/2;
    
    dest.x += 1;
    dest.y += 1;
    dest.w = last.textDimensions.w;
    dest.h = last.textDimensions.h;
    SDL_RenderCopy(vars.renderer, last.texture, NULL, &dest);
    return;
}

void customsdl::UI::scrollBox::renderTextureScrollBox(SDL_Rect boxToRenderIn, int *textureWidth, int *textureHeight, SDL_Texture *texture, Uint32 flags){
    if(flags & XCENTERED) boxToRenderIn.x = ui->vars.WIDTH/2 - (*textureWidth)/2;
    if(flags & YCENTERED) boxToRenderIn.y = ui->vars.HEIGHT/2 - (*textureHeight)/2;

    if(!(flags & NOOUTLINE)){
        SDL_SetRenderDrawColor(vars.renderer, vars.outline_color->r, vars.outline_color->g, vars.outline_color->b, vars.outline_color->a);
        SDL_RenderDrawRect(vars.renderer, &boxToRenderIn);
    }

    scrollBoxCore(boxToRenderIn, textureWidth, textureHeight, texture, flags);


    SDL_Rect renderArea = {int(shiftx*ratiox), int(shifty*ratioy), *textureWidth, std::min(boxToRenderIn.h, *textureHeight)};
    SDL_Rect destArea = {boxToRenderIn.x, boxToRenderIn.y, *textureWidth, std::min(boxToRenderIn.h, *textureHeight)};
    SDL_RenderCopy(vars.renderer, texture, &renderArea, &destArea);
    return;
}

void customsdl::UI::scrollBox::renderButtonScrollBox(SDL_Rect boxToRenderIn, std::vector<std::string> entries, color rgba, font *fontData, void (*onClick)(std::string), Uint32 flags){
    static customsdl::UI::text text_;
    if(prev_in != entries){
        prev_compact_in = "";
        prev_in = entries;
        for(std::string word : entries){
            for(char symb : word) if(symb != '\n') prev_compact_in += symb;
            prev_compact_in += '\n';
        }
            //Remove the last un-intended new line.
        prev_compact_in.pop_back();
    }

    //Variable comment.
    if(!(flags & NOBUTTONTEXT))
        renderTextureScrollBox(boxToRenderIn, &text_.last.textDimensions.w, &text_.last.textDimensions.h, text_.getTexture(prev_compact_in, rgba, boxToRenderIn, fontData, 0), flags);

    SDL_GetMouseState(&x, &y);
    if(onRect(boxToRenderIn, x, y)){        
        //Hover
            //Which button in the list.
        int n = (y-boxToRenderIn.y+shifty*ratioy)/(fontData->fontSize+1);
        if(n < entries.size() && !(flags & NOHOVER)){
            SDL_SetRenderDrawColor(vars.renderer, vars.hovered_color->r, vars.hovered_color->g, vars.hovered_color->b, vars.hovered_color->a);

            int y_hover = std::max(int(boxToRenderIn.y + n*(fontData->fontSize+1)-shifty*ratioy), boxToRenderIn.y);
            int h_hover = fontData->fontSize + 1;
            int offsetTop = int(shifty*ratioy)%(fontData->fontSize+1);
            int offsetBottom = boxToRenderIn.h%(fontData->fontSize+1);
            
            if(y < boxToRenderIn.y + fontData->fontSize + 1 - offsetTop) h_hover -= offsetTop;

            //TODO: This sometimes breaks
            else if(y >= boxToRenderIn.y + boxToRenderIn.h - offsetBottom + offsetTop) h_hover -= offsetBottom - offsetTop;

            SDL_Rect hover_area = {boxToRenderIn.x, y_hover, boxToRenderIn.w, h_hover};
            SDL_RenderFillRect(vars.renderer, &hover_area);
        }

        //Click
        if(vars.event->type == SDL_MOUSEBUTTONDOWN && vars.event->button.button == SDL_BUTTON_LEFT && n < entries.size())
            onClick(entries[n]);
    }

    return;
}

void customsdl::UI::scrollBox::scrollBoxCore(SDL_Rect boxToRenderIn, int *textureWidth, int *textureHeight, SDL_Texture *texture, Uint32 flags){
    //VERTICAL
    ratioy = double(*textureHeight)/boxToRenderIn.h;
    int bar_height = boxToRenderIn.h/ratioy;

    y_bar = {boxToRenderIn.x+boxToRenderIn.w, boxToRenderIn.y+shifty, 8, bar_height};

    if(vars.event->type == SDL_MOUSEBUTTONDOWN && vars.event->button.button == SDL_BUTTON_LEFT) if(onRect(y_bar)){clicked = true; SDL_GetMouseState(NULL, &clickedy);}
    if(vars.event->type == SDL_MOUSEBUTTONUP && vars.event->button.button == SDL_BUTTON_LEFT) clicked = false;
    if(clicked){
        SDL_GetMouseState(NULL, &y);
        shifty += y-clickedy;
        clickedy = y;

        if(shifty < 0) shifty = 0;
        else if(shifty+bar_height >= boxToRenderIn.h) shifty = boxToRenderIn.h-bar_height; 
    }

    if(!(flags & UIFLAGS::NOVERTICALSCROLLBAR)){
        SDL_SetRenderDrawColor(vars.renderer, vars.bar_color->r, vars.bar_color->g, vars.bar_color->b, vars.bar_color->a);
        if(ratioy > 1) SDL_RenderFillRect(vars.renderer, &y_bar);
    }

    //TODO: HORIZONTAL

    return;
}

FT_FaceRec* customsdl::UI::useFont(std::string path, int fontsize){
    //Check if a specific sized font is already loaded.
    for(activeFaces_struct &obj : active_faces)
        if(obj.path == path && obj.fontSize == fontsize) return obj.face;
    
    //Otherwise, load a font of specific size.
    int i = active_faces.size();
    active_faces.push_back({path, fontsize});

    FT_Open_Args args;
    args.flags = FT_OPEN_PATHNAME;
    args.pathname = path.data();

    FT_Init_FreeType(&active_faces[i].ft);
    if(FT_Open_Face(active_faces[i].ft, &args, 0, &active_faces[i].face)) std::cout << "Failed to load font ('" << path << "') of size " << fontsize << ". Tried index: " << i <<"\n";
    
    active_faces[i].face->glyph->format = FT_GLYPH_FORMAT_BITMAP;
    FT_Set_Pixel_Sizes(active_faces[i].face, fontsize, fontsize);

    return active_faces[i].face;
}

void customsdl::UI::button::renderButton(SDL_Rect rect, void (*onClick)(void*), void* data, Uint32 flags){
    if(flags & XCENTERED) rect.x = vars.WIDTH/2 - rect.w/2; 
    if(flags & YCENTERED) rect.y = vars.HEIGHT/2 - rect.h/2;
    
    if(vars.event->type == SDL_MOUSEBUTTONUP && vars.event->button.button == SDL_BUTTON_LEFT)
        if(onRect(rect)) onClick(data);

    if(onRect(rect)){
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK){
            SDL_SetRenderDrawColor(vars.renderer, vars.hovered_color->r, vars.hovered_color->g, vars.hovered_color->b, vars.hovered_color->a*3/2);
            SDL_RenderFillRect(vars.renderer, &rect);
        }
        else if(!(flags & NOHOVER)){
            SDL_SetRenderDrawColor(vars.renderer, vars.hovered_color->r, vars.hovered_color->g, vars.hovered_color->b, vars.hovered_color->a);
            SDL_RenderFillRect(vars.renderer, &rect);
        }
    }

    if(!(flags & NOOUTLINE)){
        SDL_SetRenderDrawColor(vars.renderer, vars.outline_color->r, vars.outline_color->g, vars.outline_color->b, vars.outline_color->a);
        SDL_RenderDrawRect(vars.renderer, &rect);
    }
}

bool customsdl::onRect(SDL_Rect rect){
    int x, y;
    SDL_GetMouseState(&x, &y);
    return onRect(rect, x, y);
}

bool customsdl::onRect(SDL_Rect rect, int x, int y){
    if(x >= rect.x && x < rect.x+rect.w && y >= rect.y && y < rect.y+rect.h) return true;
    return false;
}

bool customsdl::compareRects(SDL_Rect rect1, SDL_Rect rect2){
    if(rect1.x == rect2.x && rect1.y == rect2.y && rect1.h == rect2.h && rect1.w == rect2.w) return true;
    return false;
}

color customsdl::getSurfaceColors(pixel coords, SDL_Surface *surface, bool freeSurface){
    Uint8 red{}, green{}, blue{}, alfa{};
    Uint32 values = *(Uint32*)((Uint8*)((Uint8*)surface->pixels + coords.y*surface->pitch + coords.x*surface->format->BytesPerPixel));
    SDL_GetRGBA(values, surface->format, (Uint8*)(&red), (Uint8*)(&green), (Uint8*)(&blue), (Uint8*)(&alfa));
    if(freeSurface) SDL_FreeSurface(surface);
    return {red, green, blue, alfa};
}

void customsdl::blitSurface(pixel coords, SDL_Surface *to, SDL_Surface *from){
    SDL_Rect areaTo = {coords.x, coords.y, to->w, to->h};
    SDL_BlitSurface(from, NULL, to, &areaTo);
}

void customsdl::blitSurface(pixel coords, SDL_Surface *to, SDL_Rect areaFrom, SDL_Surface *from){
    SDL_Rect areaTo = {coords.x, coords.y, to->w, to->h};
    SDL_BlitSurface(from, &areaFrom, from, &areaTo);
}

void customsdl::surfaceManipulation::createSurface(int width, int height, int depth, SDL_PixelFormatEnum format){
    active_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, depth, format);
    SDL_SetSurfaceBlendMode(active_surface, SDL_BlendMode::SDL_BLENDMODE_ADD);    
    SDL_LockSurface(active_surface);
}

SDL_Texture* customsdl::surfaceManipulation::createTextureFromSurface(SDL_Renderer *renderer){
    SDL_Texture* _return;
    SDL_UnlockSurface(active_surface);
    _return = SDL_CreateTextureFromSurface(renderer, active_surface);
    SDL_FreeSurface(active_surface);
    return _return;
}

void customsdl::surfaceManipulation::drawToSurface(pixel coords, color RGBA){
    if(RGBA.a == 0) return;
    data = (Uint8*)active_surface->pixels + coords.y*active_surface->pitch + coords.x * active_surface->format->BytesPerPixel;
    *((Uint32*)data) = (Uint32)(RGBA.r << 0 | RGBA.g << 8 | RGBA.b << 16 | RGBA.a << 24);
}

HWND *customsdl::getWindowHandle(SDL_Window *window, int indx){
    //assumes we dont need more than 20 active windows;
    if(indx >= 20 || indx < 0) return nullptr;

    static bool firstLoad = true;
    static HWND* handles[20];

    if(firstLoad){
        std::fill_n(handles, 20, nullptr);
        firstLoad = false;
    }
    if(handles[indx] == nullptr){
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window, &wmInfo);
        handles[indx] = &wmInfo.info.win.window;
    }
    return handles[indx];
}