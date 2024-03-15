# STPlot - STM32 debugger
## About
Plot variables from running firmware!

1. Open elf file
2. Add variables from elf file to create Channels
3. Create Views
4. Add Channels to Views
5. Save/Load configuration
5. Connect to target using STlink or UDP SHnet Debug protocol
4. Record data

## Dependancies
- Qt5
- QCustomPlot (https://www.qcustomplot.com/)
- Dwarves (https://github.com/acmel/dwarves)
- stlink-lib (https://github.com/stlink-org/stlink)
- qt-advanced-docking-system (https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System)
- PEK_Libs (gitlab2.pekauto.com)
- QtnProperty (https://github.com/qtinuum/QtnProperty)

## Building
1. Clone this repository
2. git submodule update --init --recursive

#### CMake
1. cd stplot-gui
2. mkdir build
3. cd build
4. cmake ..
5. make
6. ./stplot-gui

#### QtCreator

### Supported platforms
- Linux
