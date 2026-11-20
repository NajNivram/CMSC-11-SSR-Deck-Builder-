cmake_minimum_required(VERSION 3.24)

get_filename_component(PROJECT_NAME_FROM_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

string(REPLACE " " "_" PROJECT_NAME_SAFE "${PROJECT_NAME_FROM_FOLDER}")

project(${PROJECT_NAME_SAFE} LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_FOLDERS
    Source
    Include
    Assets
    Assets/Fonts
    Assets/Sprites
    Assets/Objects
    Assets/Sounds
    Assets/Data
    Assets/Extra
)

foreach(folder ${PROJECT_FOLDERS})
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${folder}")
endforeach()

set(MAIN_C_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Source/main.c")

if(NOT EXISTS "${MAIN_C_PATH}")
    file(WRITE "${MAIN_C_PATH}"
"#include \"raylib.h\"

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, \"TempWindow\");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
")
endif()

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.10
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(SDL3)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp"
)

add_executable(${PROJECT_NAME_SAFE}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_include_directories(${PROJECT_NAME_SAFE} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
)

target_link_libraries(${PROJECT_NAME_SAFE} PRIVATE raylib SDL3::SDL3)

add_custom_command(
    TARGET ${PROJECT_NAME_SAFE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME_SAFE}>/Assets"
    COMMENT "Copying Assets folder..."
)

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME_SAFE})

