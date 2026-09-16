cmake_minimum_required(VERSION 4.3)
project(tap_tempo C)

set(CMAKE_C_STANDARD 11)

add_executable(tap_tempo src/main.c
        src/tap_tempo_fsm/tap_tempo_fsm.c
        include/tap_tempo_fsm/tap_tempo_fsm.h
        src/tap_tempo_fsm/tap_tempo_fsm_running.c
        src/tap_tempo_fsm/tap_tempo_fsm_reading.c
)

target_sources(tap_tempo
        PRIVATE
        platform/stm32f4xx_startup.c
        drivers/src/button.c
        utils/src/timer.c
        utils/src/fsm.c
)

set_source_files_properties(platform/stm32f4xx_startup.c
        PROPERTIES COMPILE_OPTIONS "-fno-tree-loop-distribute-patterns;-save-temps=obj"
)

include(FetchContent)

FetchContent_Declare(
        f401_re_hal
        GIT_REPOSITORY https://github.com/tom-lyden/f401_re_hal.git
        GIT_TAG master
)

FetchContent_MakeAvailable(f401_re_hal)

target_link_libraries(tap_tempo
        f401_re_hal
)

target_include_directories(tap_tempo
        PRIVATE
        include
        include/tap_tempo_fsm
        utils/include
        drivers/include
)
