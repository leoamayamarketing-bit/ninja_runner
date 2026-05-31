cmake_minimum_required(VERSION 3.16)
project(NinjaRunner VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/main.cpp
    src/core/Game.cpp
    src/core/InputManager.cpp
    src/core/KeyBindings.cpp
    src/core/AssetManager.cpp
    src/core/AudioManager.cpp
    src/core/FontRenderer.cpp
    src/systems/RenderSystem.cpp
    src/systems/PhysicsSystem.cpp
    src/systems/CollisionSystem.cpp
    src/systems/AnimationSystem.cpp
    src/systems/PlayerControlSystem.cpp
    src/systems/ParallaxSystem.cpp
    src/systems/ParticleSystem.cpp
    src/entities/PlayerFactory.cpp
    src/entities/ObstacleFactory.cpp
    src/levels/Level.cpp
    src/levels/LevelManager.cpp
    src/levels/RooftopDayLevel.cpp
    src/levels/RooftopNightLevel.cpp
    src/ui/HUD.cpp
    src/ui/Menu.cpp
    src/ui/GameOverScreen.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
    message(STATUS "Detected platform: Windows — using bundled SFML + Win32 audio")

    set(SFML_STATIC_LIBRARIES TRUE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SFML_STATIC)

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/../dino_game/sfml/include
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/../dino_game/sfml/lib/libsfml-graphics-s.a
        ${CMAKE_SOURCE_DIR}/../dino_game/sfml/lib/libsfml-window-s.a
        ${CMAKE_SOURCE_DIR}/../dino_game/sfml/lib/libsfml-system-s.a
        ${CMAKE_SOURCE_DIR}/../dino_game/sfml/lib/libfreetype.a
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ws2_32
        winmm
        opengl32
        gdi32
        ole32
        oleaut32
        uuid
    )

    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)

    target_link_options(${PROJECT_NAME} PRIVATE
        -static-libgcc
        -static-libstdc++
        -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
    )

elseif(UNIX)
    message(STATUS "Detected platform: Linux — using system SFML")

    find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
    )

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FREETYPE freetype2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${FREETYPE_LIBRARIES}
        pthread
        dl
    )

    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
else()
    message(FATAL_ERROR "Unsupported platform.")
endif()
