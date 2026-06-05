cmake_minimum_required(VERSION 3.20)

project(videoJuegoCplusPlus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(videoJuegoCplusPlus
    src/main.cpp
    src/Game.cpp
    src/AssetManager.cpp
    src/Player.cpp
    src/LaserNormal.cpp
    src/Enemy.cpp
    src/EnemyBullet.cpp
    src/EnemyConfigSystem.cpp
    src/EnemyLaser.cpp
    src/BackgroundElement.cpp
    src/BackgroundElementDirector.cpp
    src/BulletPatternSystem.cpp
    src/StageDirector.cpp
    src/MovementPatternSystem.cpp
    src/Explosion.cpp
    src/Starfield.cpp
)

target_include_directories(videoJuegoCplusPlus PRIVATE src)
target_link_libraries(videoJuegoCplusPlus PRIVATE SFML::Graphics SFML::Window SFML::System)

add_custom_command(TARGET videoJuegoCplusPlus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:videoJuegoCplusPlus>/assets
)

add_custom_command(TARGET videoJuegoCplusPlus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        $<TARGET_FILE_DIR:videoJuegoCplusPlus>/config
)
