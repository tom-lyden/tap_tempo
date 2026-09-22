cmake_minimum_required(VERSION 4.3)
project(tap_tempo C)

set(CMAKE_C_STANDARD 11)

add_executable(tap_tempo src/main.c
)

target_sources(tap_tempo
        PRIVATE
        platform/stm32f4xx_startup.c
        drivers/src/button.c
        utils/src/timer.c
        utils/src/cqueue.c
        src/tap_tempo.c
)

set_source_files_properties(platform/stm32f4xx_startup.c
        PROPERTIES COMPILE_OPTIONS "-fno-tree-loop-distribute-patterns;"
)

include(FetchContent)

FetchContent_Declare(
        f401_re_hal
        GIT_REPOSITORY https://github.com/tom-lyden/f401_re_hal.git
        GIT_TAG v0.1.0
)

FetchContent_MakeAvailable(f401_re_hal)

target_link_libraries(tap_tempo
        f401_re_hal
)

target_include_directories(tap_tempo
        PRIVATE
        drivers/include
        utils/include
        include
)
