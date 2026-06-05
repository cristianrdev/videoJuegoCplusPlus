# Proyecto

Shooter vertical arcade inspirado en Batsugun.

Tecnologías:

- C++17
- SFML 3
- CMake

Skills disponibles:

- sfml-architecture
- sfml-shmup-architecture
- bullet-pattern-designer
- pixel-art-shmup-style

Reglas generales:

- Resolución lógica 240x320.
- Nave jugador 32x32.
- Todo patrón de balas debe ser data-driven.
- Stages deben ser configurables mediante JSON.
- Utilizar StageDirector para progresión.
- Utilizar EventQueue para desacoplamiento.
- Mantener entidades simples.

This project is a 2D vertical arcade shooter built with C++17 and SFML.

General architecture:

- PlayState coordinates gameplay.
- StageDirector controls level progression.
- EventQueue decouples systems.
- EnemySpawner creates enemies.
- BulletPatternSystem handles all shooting logic.
- CollisionSystem resolves collisions.
- AssetManager loads all resources.

Always:

- Use deltaTime for movement.
- Separate update() and render().
- Load resources through AssetManager.
- Prefer JSON configuration over hardcoded gameplay.
- Keep entities lightweight.

Never:

- Load textures during gameplay loops.
- Hardcode stage timelines in PlayState.
- Hardcode bullet logic inside enemies.
- Mix rendering and gameplay logic.