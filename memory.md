# Memoria de desarrollo

Proyecto: shooter vertical arcade inspirado en Batsugun.

## Stack y arquitectura

- C++17, SFML 3 y CMake.
- Resolucion logica: 240x320.
- Nave del jugador: 32x32.
- `PlayState` coordina gameplay, pero se debe evitar meter ahi logica que pertenezca a sistemas.
- `StageDirector` controla la progresion del nivel desde JSON.
- `EventQueue` desacopla sistemas y eventos.
- `EnemySpawner` crea enemigos desde configuracion.
- `BulletPatternSystem` maneja patrones de disparo data-driven.
- `CollisionSystem` resuelve colisiones.
- `AssetManager` carga todos los recursos.

## Reglas del proyecto

- Usar `deltaTime` para movimiento.
- Separar `update()` y `render()`.
- Cargar recursos mediante `AssetManager`.
- Preferir JSON para gameplay, stages, patrones y configuraciones.
- Mantener entidades simples.
- No cargar texturas dentro de loops de gameplay.
- No hardcodear timelines de stages en `PlayState`.
- No hardcodear logica de balas dentro de enemigos.
- No mezclar render y gameplay.
- Despues de generar una imagen, pedir aprobacion del diseno antes de integrarla como definitiva.

## Assets y pixel art

- Para crear sprites, fondos, tiles, bosses o assets visuales usar la skill `pixel-art-generation-pipeline`.
- Esa skill prohibe generar sprites con PIL, OpenCV, Canvas, ASCII o dibujo procedural.
- Flujo esperado para sprites:
  - definir tamano exacto,
  - definir cantidad de frames,
  - definir paleta,
  - generar prompt para IA de imagen,
  - generar especificacion tecnica,
  - definir ruta destino.
- Para sprites pixel art en runtime, cuidar grilla y evitar subpixeles, blur o jitter.

## Comandos y validacion

- No ejecutar `cmake --build build`: en este proyecto se queda colgado.
- Si un build interrumpido deja procesos `cmake` o `ninja` vivos, revisarlos con `Get-Process` antes de continuar.
- Validaciones livianas utiles:
  - `git diff --check`
  - `git status --short --untracked-files=all`
  - inspeccionar PNGs con PIL solo para metadatos, tamano y alpha.

## Cambios recientes importantes

- `enemy_green_cargo_tank` usa una explosion propia:
  - asset: `assets/textures/effects/explosion_enemy_green_cargo_tank.png`
  - tamano: `192x96`
  - frames: 3 frames horizontales de `64x96`
  - estilo: explosion grande con humo oliva, fuego amarillo/naranja y fragmentos verdes del tanque.
- En `PlayState`, los proyectiles enemigos ya no se eliminan repentinamente cuando se destruye la nave.
  - `enemyBullets_` sigue actualizandose durante el estado `playerDestroyed_`.
  - Las balas se limpian solo cuando dejan de estar vivas/salen del area logica.
  - `enemyLasers_` todavia se limpian al morir la nave.

## Archivos clave

- `src/states/PlayState.cpp`
- `src/states/PlayState.hpp`
- `src/entities/Explosion.cpp`
- `src/entities/Explosion.hpp`
- `src/entities/Enemy.cpp`
- `config/enemies.json`
- `config/projectiles.json`
- `config/bullet_patterns.json`
- `config/stage_01_enemies.json`
