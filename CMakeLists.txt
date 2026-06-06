cmake_minimum_required(VERSION 3.20)

project(videoJuegoCplusPlus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(videoJuegoCplusPlus
    src/main.cpp
    src/core/Game.cpp
    src/core/AssetManager.cpp
    src/entities/Player.cpp
    src/entities/LaserNormal.cpp
    src/entities/Enemy.cpp
    src/entities/EnemyBullet.cpp
    src/entities/EnemyLaser.cpp
    src/entities/BackgroundElement.cpp
    src/entities/Explosion.cpp
    src/systems/BackgroundElementDirector.cpp
    src/systems/BulletPatternSystem.cpp
    src/systems/StageDirector.cpp
    src/systems/MovementPatternSystem.cpp
    src/config/EnemyConfigSystem.cpp
    src/config/PlayerConfigSystem.cpp
    src/config/ProjectileConfigSystem.cpp
    src/background/Starfield.cpp
)

target_include_directories(videoJuegoCplusPlus PRIVATE
    src
    src/core
    src/entities
    src/systems
    src/config
    src/background
    src/ui
)
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
