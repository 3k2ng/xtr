cmake_minimum_required(VERSION 3.25)
project(xtr)

include(FetchContent)
set(FETCHCONTENT_QUIET NO)

find_package(SDL2 QUIET)
if (NOT ${SDL2_FOUND})
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG 79ec168f3c1e2fe27335cb8886439f7ef676fb49 # 2.30.8
    )
    FetchContent_MakeAvailable(SDL2)
    set(SDL2_INCLUDE_DIRS "${SDL2_SOURCE_DIR}/include")
    set(SDL2_LIBRARIES SDL2::SDL2)
endif()

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG cb16568fca5297512ff6a8f3b877f461c4323fbe # 1.91.3
)
FetchContent_MakeAvailable(imgui)

add_executable(xtr
    "./xtr_main.cpp"
    "./external/glad/src/gl.c"
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(xtr PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${imgui_SOURCE_DIR}
    "${imgui_SOURCE_DIR}/backends"
    "./external/glad/include"
)
target_link_libraries(xtr PRIVATE ${SDL2_LIBRARIES})
