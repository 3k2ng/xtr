cmake_minimum_required(VERSION 3.19)
project(xtr)

include(FetchContent)
set(FETCHCONTENT_QUIET NO)
set(CMAKE_CXX_STANDARD 20)

find_package(SDL2)
if (NOT ${SDL2_FOUND})
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG 79ec168f3c1e2fe27335cb8886439f7ef676fb49 # 2.30.8
    )
    FetchContent_MakeAvailable(SDL2)
    set(SDL2_INCLUDE_DIRS "${SDL2_SOURCE_DIR}/include")
endif()

find_package(SDL2_image)
if (NOT ${SDL2_image_FOUND})
    FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
        GIT_TAG abcf63aa71b4e3ac32120fa9870a6500ddcdcc89 # 2.8.2
    )
    FetchContent_MakeAvailable(SDL2_image)
    set(SDL2_image_INCLUDE_DIRS "${SDL2_image_SOURCE_DIR}/include")
endif()

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG cb16568fca5297512ff6a8f3b877f461c4323fbe # 1.91.3
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863 # 1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
    GIT_TAG 2945a967c5303b2c8c14174117c45f3302591150 # 2.0.0rc13
)
FetchContent_MakeAvailable(tinyobjloader)

add_executable(${PROJECT_NAME}
    "./xtr_main.cpp"
    "./src/xtr_app.cpp"
    "./external/glad/src/gl.c"
    "./external/miniply/src/miniply.cpp"
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(${PROJECT_NAME} PRIVATE
    "./include"
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_image_INCLUDE_DIRS}
    ${imgui_SOURCE_DIR}
    "${imgui_SOURCE_DIR}/backends"
    ${glm_SOURCE_DIR}
    ${tinyobjloader_SOURCE_DIR}
    "./external/glad/include"
    "./external/miniply/include"
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
)

file(GLOB texture_files "./data/textures/*")
foreach(file ${texture_files})
    file(RELATIVE_PATH file ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    configure_file(${file} ${file} COPYONLY)
endforeach()

file(GLOB model_files "./data/models/*")
foreach(file ${model_files})
    file(RELATIVE_PATH file ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    configure_file(${file} ${file} COPYONLY)
endforeach()

file(GLOB shader_files "./data/shaders/*")
foreach(file ${shader_files})
    file(RELATIVE_PATH file ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    configure_file(${file} ${file} COPYONLY)
endforeach()

if (WIN32)
    # copy the .dll file to the same folder as the executable
    add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2::SDL2>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)
    add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2_image::SDL2_image>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)
endif()
