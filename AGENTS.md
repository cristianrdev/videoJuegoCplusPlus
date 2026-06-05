# Project Instructions

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