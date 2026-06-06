---
name: sfml-architecture
description: Use this skill when creating, initializing, documenting, or refactoring C++17 + SFML game architecture, especially 2D arcade/shmup projects with clean folders, Game/PlayState separation, AssetManager, entities, systems, config loaders, EventQueue, EnemySpawner, CollisionSystem, and data-driven gameplay.
---

# SFML Architecture

Usar esta skill para crear o refactorizar proyectos C++17 + SFML con una arquitectura limpia, modular y escalable. La base recomendada esta pensada para shooters verticales arcade, pero sirve como patron general para juegos 2D con estados, entidades, sistemas, configuracion externa y assets administrados.

## Principios

- Separar infraestructura, estado de juego, entidades, sistemas y configuracion.
- Mantener `Game` como infraestructura: ventana, loop, deltaTime, input global y presentacion.
- Mantener `PlayState` como coordinador de gameplay, no como contenedor de toda la logica.
- Mantener entidades livianas: posicion, sprite, hitbox, vida, IDs y timers simples.
- Usar sistemas para reglas que procesan entidades: spawns, movimiento, disparos, colisiones, eventos, efectos.
- Usar `AssetManager` para cargar recursos antes del loop de gameplay.
- No cargar texturas, sonidos ni fuentes dentro de `update()` o `render()`.
- Usar `deltaTime` para todo movimiento, cooldown, animacion y temporizador.
- Separar siempre `update()` y `render()`.
- Preferir JSON/config externa para parametros de gameplay.
- Publicar eventos importantes con `EventQueue` antes que acoplar sistemas directamente.

## Estructura Recomendada

```txt
src/
+-- main.cpp
+-- background/
+-- config/
+-- core/
+-- entities/
+-- states/
+-- systems/
+-- ui/
```

## Responsabilidades Por Carpeta

### `main.cpp`

Punto de entrada.

Debe:

- Crear `Game`.
- Ejecutar `game.run()`.

No debe:

- Contener gameplay.
- Cargar assets.
- Definir stages, enemigos, patrones o reglas.

### `core/`

Infraestructura general.

Clases tipicas:

- `Game`
- `AssetManager`
- `AudioManager`
- `Config`

Debe:

- Crear y administrar la ventana SFML.
- Ejecutar el game loop.
- Calcular `deltaTime`.
- Manejar input global: cerrar, pausa, fullscreen, modos de escalado.
- Renderizar a resolucion logica y presentar escalado manteniendo aspect ratio.
- Administrar recursos compartidos.

No debe:

- Contener logica especifica de enemigos, balas, stages, colisiones o score.

### `states/`

Pantallas o modos del juego.

Clases tipicas:

- `PlayState`
- `MenuState`
- `PauseState`
- `GameOverState`

`PlayState` debe:

- Coordinar el gameplay activo.
- Actualizar sistemas en orden determinista.
- Mantener colecciones principales si aun no existe `World` o `EntityRegistry`.
- Delegar a sistemas especializados.
- Consumir eventos de `EventQueue`.
- Separar `update()` y `render()`.

`PlayState` no debe crecer con:

- Patrones de balas manuales.
- Timelines de stage hardcodeados.
- Colisiones directas.
- Carga de texturas durante la partida.
- Audio, score o efectos acoplados directamente a colisiones.

### `entities/`

Objetos del mundo del juego.

Clases tipicas:

- `Player`
- `Enemy`
- `Bullet`
- `Laser`
- `Explosion`
- `PowerUp`
- `BackgroundElement`

Debe:

- Guardar estado local: posicion, vida, sprite, hitbox, timers simples.
- Exponer `update()` y `render()` cuando corresponda.
- Guardar IDs como `enemyId`, `patternId`, `movementId`, `projectileId`.

No debe:

- Cargar recursos.
- Conocer el stage completo.
- Crear oleadas.
- Resolver colisiones globales.
- Administrar score, audio o otros sistemas.

### `systems/`

Reglas que procesan entidades o coordinan gameplay.

Sistemas recomendados:

- `StageDirector`: decide cuando ocurren eventos del stage desde JSON.
- `EnemySpawner`: crea enemigos desde eventos/config.
- `BulletPatternSystem`: genera disparos desde patrones data-driven.
- `MovementPatternSystem`: calcula movimiento desde config.
- `CollisionSystem`: detecta impactos y publica eventos.
- `EventQueue`: desacopla lo que ocurre de quien reacciona.
- `EffectSystem`: crea explosiones/particulas al consumir eventos.
- `ProjectileSystem`: actualiza y limpia proyectiles.
- `ScoreSystem`: escucha eventos y suma puntos.
- `AudioSystem`: escucha eventos y reproduce sonidos.
- `BackgroundElementDirector`: orquesta elementos de fondo desde JSON.

Reglas:

- Cada sistema debe tener una responsabilidad clara.
- Evitar sistemas que lo sepan todo.
- Cuando algo importante ocurra, publicar un evento.
- Preferir datos externos para patrones, tiempos, stats, rutas de assets y parametros.

### `config/`

Lectores y validadores de configuracion.

Clases tipicas:

- `EnemyConfigSystem`
- `PlayerConfigSystem`
- `ProjectileConfigSystem`
- `StageConfigSystem`
- `MovementConfigSystem`

Debe:

- Leer JSON/TOML/u otro formato.
- Convertir configuracion externa a estructuras C++.
- Centralizar valores de gameplay: vida, velocidad, dano, cadencia, rutas de texturas, hitboxes.

No debe:

- Renderizar.
- Crear entidades directamente salvo que sea parte explicita de una factory.
- Resolver gameplay frame a frame.

### `background/`

Logica especializada de fondo.

Ejemplos:

- `Starfield`
- fondos proceduralmente animados
- capas de parallax

Los elementos de fondo que tienen posicion, sprite e hitbox opcional pueden vivir como entidades (`BackgroundElement`) y ser orquestados por sistemas.

### `ui/`

Interfaz de usuario.

Debe contener:

- HUD
- vida
- score
- FPS
- stage time
- pausa
- menus
- game over

Evitar que `Game` o `PlayState` acumulen UI a medida que el proyecto crece.

## Flujo De Update Recomendado

Usar un orden determinista:

```txt
1. StageDirector / directores de fondo
2. Spawns mediante EnemySpawner
3. Background / starfield / parallax
4. Player
5. Proyectiles del jugador
6. Enemigos y MovementPatternSystem
7. BulletPatternSystem / disparos enemigos
8. Balas, lasers y proyectiles enemigos
9. Efectos y explosiones
10. Limpieza de entidades fuera de pantalla
11. CollisionSystem
12. EventQueue
13. Limpieza de entidades destruidas
14. UI/HUD state
```

## EventQueue

Usar `EventQueue` para comunicar sucesos importantes sin acoplar sistemas.

Eventos utiles:

- `EnemySpawnedEvent`
- `EnemyDestroyedEvent`
- `PlayerHitEvent`
- `PlayerDeadEvent`
- `BossSpawnedEvent`
- `BossPhaseChangedEvent`
- `BossDefeatedEvent`
- `PowerUpCollectedEvent`
- `StageClearEvent`

Flujo recomendado:

```txt
CollisionSystem detecta impacto
        |
EventQueue publica EnemyDestroyedEvent
        |
EffectSystem crea explosion
ScoreSystem suma puntos
AudioSystem reproduce sonido
PowerUpSystem evalua drop
```

Una implementacion simple puede usar `std::variant` y `drain()`. Mas adelante puede evolucionar a listeners/subscribers si el proyecto lo necesita.

## Criterios Para Ubicar Codigo Nuevo

Agregar en `entities/` cuando:

- Sea un objeto del mundo.
- Tenga posicion, sprite, hitbox, vida o estado propio.

Agregar en `systems/` cuando:

- Procese varias entidades.
- Aplique reglas del juego.
- Coordine comportamiento data-driven.
- Publique o consuma eventos.

Agregar en `config/` cuando:

- Lea o valide archivos de configuracion.
- Convierta datos externos en estructuras C++.

Agregar en `core/` cuando:

- Sea infraestructura general.
- No dependa de reglas especificas de gameplay.

Agregar en `ui/` cuando:

- Sea informacion visual para el jugador.
- No pertenezca al estado interno de una entidad.

## Patron De Crecimiento

Extraer un sistema cuando una responsabilidad:

- Empieza a repetirse.
- Hace crecer demasiado `PlayState`.
- Necesita reaccionar a eventos.
- Tiene datos propios configurables.
- Procesa colecciones de entidades.

Evitar sobre-arquitectura temprana. Primero mantener simple; luego extraer cuando la responsabilidad tenga forma clara.

## Prompt Para Inicializar Proyectos Futuros

Usar este prompt cuando se quiera crear un nuevo proyecto C++17 + SFML con esta arquitectura:

```txt
Usa la skill sfml-architecture.

Objetivo:
Crear la base de un proyecto C++17 + SFML 3 con arquitectura limpia y escalable para un juego 2D.

Requisitos de arquitectura:
1. Crear estructura src/ con:
   - main.cpp
   - core/
   - states/
   - entities/
   - systems/
   - config/
   - background/
   - ui/
2. Implementar Game en core/:
   - ventana SFML
   - game loop
   - deltaTime
   - update/render separados
   - cerrar con Esc
   - AssetManager compartido
3. Implementar PlayState en states/:
   - coordina gameplay
   - no carga recursos dentro del loop
   - delega spawns, colisiones, eventos y patrones a systems/
4. Implementar AssetManager:
   - cargar texturas por ID
   - obtener texturas por ID
   - evitar cargas duplicadas
5. Implementar EventQueue en systems/:
   - eventos con std::variant
   - publish()
   - drain()
6. Implementar sistemas base:
   - StageDirector
   - EnemySpawner
   - CollisionSystem
   - BulletPatternSystem
   - MovementPatternSystem
7. Implementar entidades simples:
   - Player
   - Enemy
   - Projectile/Bullet
8. Agregar carpeta config/ en la raiz del proyecto para JSON:
   - player.json
   - enemies.json
   - projectiles.json
   - bullet_patterns.json
   - movement_patterns.json
   - stage_01_enemies.json
9. Usar CMake con C++17 y SFML 3.
10. Documentar la arquitectura en src/ARCHITECTURE.md.

Reglas:
- Usar deltaTime para movimiento y timers.
- Separar update() y render().
- No cargar texturas en update() ni render().
- Preferir JSON antes que valores hardcodeados.
- Mantener entidades livianas.
- Mantener PlayState como coordinador, no como clase gigante.
- Publicar eventos para enemy destroyed, player hit y player dead.

Antes de modificar codigo, explicar brevemente que archivos se van a crear o modificar.
```
