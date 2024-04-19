# CMakeList.txt for HAL (Hardware Abstraction Layer)
#   Build a library (`hal`) which exposes the header files as "hal/*.h"
#   Use header as: #include "hal/button.h"

include_directories(hal/include)
file(GLOB MY_SOURCES "src/*.cpp")

add_library(hal STATIC ${MY_SOURCES})

target_link_libraries(hal PUBLIC common)
target_include_directories(hal PUBLIC include)
