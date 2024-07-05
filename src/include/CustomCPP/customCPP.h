#ifndef CUSTOMCPP_H
#define CUSTOMCPP_H

//Standard libraries
#include<fstream>
#include<type_traits>
#include<vector>
#include<string>
#include<cmath>
#include<chrono>
#include<bitset>
#include<filesystem>
#include<algorithm>
#include<iostream>

//OS specific libraries
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef UNICODE
#define UNICODE
#endif
#include<Windows.h>
#include<memoryapi.h>
#include<shlobj.h>
#include<sstream>

///////////////////

namespace customcpp{

    struct pixel{
        int x, y;
    };
    struct color{
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    //////////////////////

    /**
     * Returns the -almost- exact number of updates per second.
     * This function should be called once per update in your main loop.
     * @param _cout `true` if you want to get a console message: `UPS: upsValue`.
    */
    int upsAverage(bool _cout);

///////////////////////////////

    /**
     * Returns a number value represented as a string.
     * @param a number.
     * @return a number as a string.
    */
    std::string intToString(int numb);
    /**
     * Returns an integer value from a given string.
     * @param a string holding a number.
     * @return a number as an int.
    */
    int stringToInt(std::string string);

    /**
     * Returns a number of `double` type represented as a string.
     * @param in a `double` type number.
     * @param precision number of fractional values.
     * @return a `double` type numbers as a string.
    */
    std::string doubleToString(double in, int precision);
    /**
     * Returns a number of `double` type from a given string.
     * Fractional part can be seperated either by a comma or a dot.
     * @param in a string holding a number.
     * @return a number of type `double`.
    */
    double stringToDouble(std::string in);

    /** 
     * TODO: a memory leak?
     * Converts a char* to LPWSTR (wchar_t*) type.
     * @param charArr the pointer to a char array to convert.
     * @return returns converted array. 
    */
    wchar_t* charToLPWSTR(const char* charArr);

///////////////////////////////////////////////

    /** 
     * TODO: a memory leak?
     * Append char* to a given character array.
     * @param array the char* object to which `to_add` is appended.
     * @param to_add the data to append.
     * @return Returns a new char* with the appended characters.
    */
    char* append_char_p(const char* array, const char* to_add);

///////////////////////////////////////////////////////////////
    //Windows specific

    /**
     * Creates a window's "Browse For Folder" window.
     * @param saved_path path to a folder which will be selected on start.
     * @return Returns a path to a selected folder.
     */
    std::string browseFolder(std::string saved_path, std::string title);

    /**
     * Returns a color struct of pixel colors.
     * @param hWnd a handle to a window. With sdl, you can get it by calling `customsdl::getWindowHandle` function.
     * @param wind_Width the width in pixels of the main window.
     * @param wind_Height the height in pixels of the main window.
     */
    color* getScreenPixels(HWND *hWnd, int wind_Width, int wind_Height);

    /**
     * Make it so a window will ignore -and effectivelly turn transparent- a specified color.
     * @param windowHandle a handle to a window. With sdl, you can get it by calling `customsdl::getWindowHandle` function.
     * @param ignored_rgb is the color the window will completely ignore.
     */
    bool windowIgnoreColor(HWND *windowHandle, color ignored_rgb);

//////////////////////////////////////////////////////////////////
        //Algorithms: TODO: DYNAMIC VARIABLES
    
    /**
     * Sorts a vector array with `mergesort` algorithm.
     * @param array an integer std::vector array.
     */
    std::vector<int> mergesort(std::vector<int> array);

    /** TODO: other pair gets reversed with each sort. Manages to sort backwards.
     * Sorts a vector array with `mergesort` algorithm.
     * Provided parameter is a vector pair of two numbers and function will sort either the first or the second member.
     * Because `mergesort` is stable, the order of identical values won't change.
     * @param array an `std::vector<std::pair>` of int and float values.
     * @param sort_the_first_questionmark `true` to sort by the first member in std::pair and `false` to sort by the second.
     */
    std::vector<std::pair<int, float>> mergesort(std::vector<std::pair<int, float>> array, bool sort_the_first_questionmark);
}

#endif