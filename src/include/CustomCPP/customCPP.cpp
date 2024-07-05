/**
 * Custom functions and classes that are useful for various c++ application.
*/
#include<CustomCPP/customCPP.h>

/////////////////////
//Function to get updates per second:

int customcpp::upsAverage(bool _cout){
    static std::chrono::steady_clock::time_point then;
    static int updates = 0;
    static int lastValue = 0;

    if(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now()-then).count() >= 1){
        lastValue = updates;
        if(_cout) std::cout << "UPS: " << lastValue << "\n";
        then = std::chrono::steady_clock::now();
        updates = 0;
    }
    updates++;
    return lastValue;
}

/////////////////////

std::string customcpp::intToString(int numb){
    std::string _return_backwards = "";

    if(numb == 0)
        _return_backwards += "0";
    while(numb != 0){
        _return_backwards += char(numb%10+48);
        numb/=10;
    }

    std::string _return = "";
    for(int i = _return_backwards.size()-1; i>=0; i--)
        _return+=_return_backwards[i];

    return _return;
}
int customcpp::stringToInt(std::string string){
    if(string.size() == 0) return 1;

    int _return = 0;
    int multiplier = 1;
    for(int i = string.size()-1; i >= 0; i--){
        if(i == 0 && string[i] == '-') _return*=-1;
        else if(i!=0 && string[i] == '-') return 0;

        _return += ((int)string[i]-48)*multiplier;
        multiplier*=10;
    }

    return _return;
}
std::string customcpp::doubleToString(double in, int precision){
    std::string _return = "";
    if(in == 0) return "0";

    //Seperate integer and fractional parts.
    int whole = int(in);
    double fraction = in - whole;

    //Save the integer part.
    _return = intToString(whole);
    _return += ".";

    //Save fractional part to the provided precision.
    _return += intToString(int(fraction*std::pow(10, precision)));

    return _return;
}
double customcpp::stringToDouble(std::string in){
    double _return = 0;
    if(in.size() == 0) return 1;

    int k = 0;
    if(in[0] == '-') k = 1;
    double multiplier = 0.1;
    while(true){
        if(in.size() == k) break;
        if(in[k] == ',' || in[k] == '.')
            break;
        k++;
        multiplier*=10;
    }
    
    for(int i = 0; i<in.size(); i++){
        if(in[i] != '-' && in[i] != ',' && in[i] != '.'){
            _return += ((int)in[i]-48)*multiplier;
            multiplier/=10;
        }
    }

    if(in[0] == '-') _return*=-1;
    return _return;
}

wchar_t* customcpp::charToLPWSTR(const char* charArr){
    int length = MultiByteToWideChar(CP_UTF8, 0, charArr, -1, nullptr, 0);
    wchar_t* wideChar = new wchar_t[length];

    MultiByteToWideChar(CP_UTF8, 0, charArr, -1, wideChar, length);

    return wideChar;
}

////////////////////

char* customcpp::append_char_p(const char* array, const char* to_add){
    size_t length = strlen(array) + strlen(to_add);
    char* _return = new char[length + 1];

    strcpy(_return, array);
    for(int i = 0; i < strlen(to_add); i++)
        _return[i + strlen(array)] = to_add[i];
    _return[length] = '\0';

    return _return;        
}

//Used by `browseFolder` function.
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM LParam, LPARAM lpData){
    if(uMsg == BFFM_INITIALIZED){
        std::string tmp = (const char*)lpData;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }
    return 0;
}

/////////////

std::string customcpp::browseFolder(std::string saved_path, std::string title){
    TCHAR path[MAX_PATH];

    BROWSEINFO bi = {0};

    WCHAR wchr[title.size()];
    MultiByteToWideChar(0, 0, title.c_str(), -1, wchr, title.size()+1);
    bi.lpszTitle = wchr;
    
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseCallbackProc;

    std::wstring wsaved_path(saved_path.begin(),saved_path.end());
    const wchar_t * path_param = wsaved_path.c_str();
    bi.lParam = (LPARAM)path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if(pidl != 0){
        SHGetPathFromIDList(pidl, path);

        IMalloc * imalloc = 0;
        if(SUCCEEDED(SHGetMalloc(&imalloc))){
            imalloc->Free(pidl);
            imalloc->Release();
        }
        std::wstring wStr = path;
        std::string str(wStr.begin(), wStr.end());
        return str;
    }
    
    //Window closed without confirming a selection.
    return saved_path;
}

customcpp::color* customcpp::getScreenPixels(HWND *hWnd, int wind_Width, int wind_Height){
    HDC hdc, hdcMemory;
    DWORD purposeIsToRemoveWarning;
    BITMAPINFO bitmap;

    //An array containing all pixel rgb values
    //But micorosft is a place with a bunch of weirdos. The origin is on the bottom left corner 
    //instead of the top left and color values are saved in ABGR order instead of RGBA. ..honestly, why?
    BYTE *data;

    hdc = GetDC(HWND_DESKTOP);
    hdcMemory = CreateCompatibleDC(hdc);

    bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
    bitmap.bmiHeader.biWidth = wind_Width;
    bitmap.bmiHeader.biHeight = wind_Height;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    bitmap.bmiHeader.biCompression = BI_RGB;
    bitmap.bmiHeader.biSizeImage = wind_Width * 4 * wind_Height;
    bitmap.bmiHeader.biClrUsed = 0;
    bitmap.bmiHeader.biClrImportant = 0;
    HBITMAP hbitmap = CreateDIBSection(hdcMemory, &bitmap, DIB_RGB_COLORS, (void**)(&data), NULL, purposeIsToRemoveWarning);
    SelectObject(hdcMemory, hbitmap);
    BitBlt(hdcMemory, 0, 0, wind_Width, wind_Height, hdc, 0, 0, SRCCOPY);

    color *pixels = new color[wind_Width * wind_Height];

    //Save pixel RGB values in a more sensible format.
    for(int x = 0; x < wind_Width; x++){
        for(int y = 0; y < wind_Height; y++){
            pixels[(wind_Height-y-1)*wind_Width+x] = {(uint8_t)data[4*(y*wind_Width+x)+2], (uint8_t)data[4*(y*wind_Width+x)+1], (uint8_t)data[4*(y*wind_Width+x)], 255};
        }
    }

    DeleteObject(hbitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(*hWnd, hdc);

    return pixels;
}

bool customcpp::windowIgnoreColor(HWND *windowHandle, color ingored_rgb){
    COLORREF clr = RGB(ingored_rgb.r, ingored_rgb.g, ingored_rgb.b);
    SetWindowLong(*windowHandle, GWL_EXSTYLE, GetWindowLong(*windowHandle, GWL_EXSTYLE) | WS_EX_LAYERED);
    return SetLayeredWindowAttributes(*windowHandle, clr, 0, LWA_COLORKEY);
}

std::vector<int> customcpp::mergesort(std::vector<int> arr){
    if(arr.size() == 1) return arr;

    std::vector<int> _1 = mergesort({arr.begin(), arr.begin() + arr.size()/2});
    std::vector<int> _2 = mergesort({arr.begin() + arr.size()/2, arr.end()});

    std::vector<int> _rtrn;
    int i = 0, x = 0;
    while(i < _1.size() || x < _2.size()){
        if(_1[i] < _2[x] && i < _1.size() || x >= _2.size()){
            _rtrn.push_back(_1[i]);
            i++;
        }
        else{
            _rtrn.push_back(_2[x]);
            x++; 
        }
    }

    return _rtrn;
}

std::vector<std::pair<int, float>> customcpp::mergesort(std::vector<std::pair<int, float>> arr, bool sort_the_first){
    if(arr.size() == 1) return arr;

    std::vector<std::pair<int, float>> _1 = mergesort({arr.begin(), arr.begin() + arr.size()/2}, sort_the_first);
    std::vector<std::pair<int, float>> _2 = mergesort({arr.begin() + arr.size()/2, arr.end()}, sort_the_first);

    std::vector<std::pair<int, float>> _rtrn;
    int i = 0, x = 0;
    while(i < _1.size() || x < _2.size()){
        if(sort_the_first){
            if(_1[i].first < _2[x].first && i < _1.size() || x >= _2.size()){
                _rtrn.push_back(_1[i]);
                i++;
            }
            else{
                _rtrn.push_back(_2[x]);
                x++; 
            }
        }
        else{
            if(_1[i].second < _2[x].second && i < _1.size() || x >= _2.size()){
                _rtrn.push_back(_1[i]);
                i++;
            }
            else{
                _rtrn.push_back(_2[x]);
                x++; 
            }
        }
    }

    return _rtrn;
}