# CMakeList.txt for common
#   Build a library (`common`) which exposes the header files as "common/*.h"
#   Use header as: #include "common/utils.h"

include_directories(common/include)
file(GLOB MY_SOURCES "src/*.cpp")

add_library(common STATIC ${MY_SOURCES})

target_include_directories(common PUBLIC include)
