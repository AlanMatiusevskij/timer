all:
	i686-w64-mingw32-g++ -I src/include -L src/lib -o app main.cpp ./src/include/CustomCPP/customCPP.cpp ./src/include/CustomSDL/customSDL.cpp -lmingw32 -lSDL2main -lSDL2 -lgdi32 -lfreetype