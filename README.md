# USAL-TheRealCodeWars

## Demo
The video TheRealCodeWars - Demo has a demonstration of the project. There are some things that the recording program did not capture such as the lists options or the file explorer to upload a file.

## Installation
Make sure you have Msys2 installed on windows, also install the packages (needed)
```
pacman -S mingw-w64-x86_64-sqlite3
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-gcc
```
For easier installation, add your {PATH_TO}/msys64/mingw64/bin to the PATH variable.

Download source code and once you are on the root of the project do:
```
cd code
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```
And with that you will have 'TheRealCodeWars.exe' which can be executed to have your own server for TheRealCodeWars.
Not confirmed, but server should only work in windows.

To execute matches is is also needed to have python installed and added to the PATH (bots are written in python).
PATH is needed because the bot.py will be executed from the server like this:

```
python bot.py
```
