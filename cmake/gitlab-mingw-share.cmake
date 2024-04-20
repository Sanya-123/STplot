# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   ${CI_MXE_PATH}/usr/bin/x86_64-w64-mingw32.shared-gcc)
set(CMAKE_CXX_COMPILER ${CI_MXE_PATH}/usr/bin/x86_64-w64-mingw32.shared-g++)
set(CMAKE_RC_COMPILER  ${CI_MXE_PATH}/usr/bin/x86_64-w64-mingw32.shared-windres)

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH  ${CI_MXE_PATH}/usr/x86_64-w64-mingw32.shared
    ${CI_MXE_PATH}/usr/x86_64-w64-mingw32.shared/qt5)

## adjust the default behavior of the FIND_XXX() commands:
## search programs in the host environment
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

## search headers and libraries in the target environment
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY) 

set(CMAKE_PREFIX_PATH ${CI_MXE_PATH}/usr/x86_64-w64-mingw32.shared/qt5)
#set(CMAKE_PREFIX_PATH /opt/Qt/5.15.2/gcc_64/lib/cmake/ /home/user/mingw-install/mingw81_32/lib/cmake/)
#set(CMAKE_PREFIX_PATH /opt/Qt/5.15.2/gcc_64/lib/cmake/)

