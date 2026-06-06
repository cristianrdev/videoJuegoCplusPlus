# Arquitectura de `src`

Este proyecto usa una arquitectura modular para un shooter vertical arcade en C++17 y SFML 3. La idea principal es que `PlayState` coordine la partida, mientras que los sistemas especializados resuelven responsabilidades concretas como spawns, patrones de disparo, movimiento, colisiones y eventos.

La resolucion logica del juego es `240x320`, y todo el gameplay debe moverse con `deltaTime`.

## Vista General

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

## `main.cpp`

Punto de entrada de la aplicacion.

Responsabilidades:

- Crear la instancia principal de `Game`.
- Iniciar el loop principal mediante `game.run()`.

No debe contener logica de gameplay, carga de assets, configuracion de stages ni reglas de enemigos.

## `core/`

Contiene infraestructura general del juego.

Archivos actuales:

- `Game.hpp/.cpp`
- `AssetManager.hpp/.cpp`

Responsabilidades:

- Crear y administrar la ventana SFML.
- Ejecutar el game loop.
- Calcular y entregar `deltaTime`.
- Manejar input global como cerrar con `Esc`, pausa, fullscreen o modos de escalado.
- Renderizar el juego a la resolucion logica y presentarlo escalado.
- Administrar recursos compartidos mediante `AssetManager`.

Reglas:

- `core` no debe tener logica especifica de enemigos, balas, stages o colisiones.
- Las texturas y recursos deben cargarse mediante `AssetManager`.
- No se deben cargar texturas dentro del loop de gameplay.

## `states/`

Contiene estados o pantallas del juego.

Archivos actuales:

- `PlayState.hpp/.cpp`

Responsabilidades de `PlayState`:

- Coordinar el gameplay activo.
- Actualizar sistemas en un orden determinista.
- Mantener las colecciones principales de entidades mientras no exista un `World` o `EntityRegistry`.
- Delegar responsabilidades a sistemas especializados.
- Consumir eventos de `EventQueue` y reaccionar a ellos.
- Separar claramente `update()` y `render()`.

`PlayState` puede coordinar, pero debe evitar absorber logica que pueda vivir en un sistema.

Ejemplo de responsabilidades que no deberian crecer dentro de `PlayState`:

- Crear patrones de balas manualmente.
- Definir timelines de stages en codigo.
- Resolver colisiones directamente.
- Cargar texturas durante la partida.

## `entities/`

Contiene objetos del mundo del juego.

Archivos actuales:

- `Player.hpp/.cpp`
- `Enemy.hpp/.cpp`
- `LaserNormal.hpp/.cpp`
- `EnemyBullet.hpp/.cpp`
- `EnemyLaser.hpp/.cpp`
- `Explosion.hpp/.cpp`
- `BackgroundElement.hpp/.cpp`

Responsabilidades:

- Representar estado local de una entidad.
- Guardar posicion, vida, sprite, hitbox, timers internos simples e identificadores de configuracion.
- Implementar `update()` y `render()` cuando corresponda.

Reglas:

- Las entidades deben ser livianas.
- Una entidad no debe cargar texturas.
- Una entidad no debe conocer el stage completo.
- Una entidad no debe crear oleadas ni administrar sistemas globales.
- Los IDs como `patternId`, `movementId` o `enemyId` permiten que otros sistemas decidan comportamiento desde JSON.

## `systems/`

Contiene logica que procesa entidades o coordina reglas de gameplay.

Archivos actuales:

- `StageDirector.hpp/.cpp`
- `BackgroundElementDirector.hpp/.cpp`
- `EnemySpawner.hpp/.cpp`
- `BulletPatternSystem.hpp/.cpp`
- `MovementPatternSystem.hpp/.cpp`
- `CollisionSystem.hpp/.cpp`
- `EventQueue.hpp/.cpp`

Responsabilidades principales:

- `StageDirector`: lee el JSON del stage y decide cuando aparecen enemigos.
- `BackgroundElementDirector`: lee el JSON de elementos de fondo y decide cuando aparecen.
- `EnemySpawner`: crea enemigos usando `AssetManager`, `EnemyConfigSystem` y eventos del stage.
- `BulletPatternSystem`: genera disparos enemigos a partir de patrones configurados por JSON.
- `MovementPatternSystem`: calcula posiciones de enemigos segun patrones configurados por JSON.
- `CollisionSystem`: detecta impactos y publica eventos como `EnemyDestroyedEvent` o `PlayerHitEvent`.
- `EventQueue`: desacopla lo que ocurre de quien reacciona.

Reglas:

- Los sistemas deben tener una responsabilidad clara.
- Los patrones de balas deben ser data-driven.
- Los movimientos configurables deben vivir en JSON siempre que sea razonable.
- Cuando algo importante ocurra, preferir publicar un evento antes que acoplar sistemas directamente.

## `config/`

Contiene sistemas que leen configuracion desde archivos JSON.

Archivos actuales:

- `EnemyConfigSystem.hpp/.cpp`
- `PlayerConfigSystem.hpp/.cpp`
- `ProjectileConfigSystem.hpp/.cpp`

Responsabilidades:

- Leer configuraciones externas.
- Exponer datos limpios al resto del juego.
- Evitar valores hardcodeados para vida, texturas, velocidad, cadencia, dano y parametros similares.

Los archivos JSON viven en la carpeta raiz `config/`, mientras que estas clases viven en `src/config/` y se encargan de interpretarlos.

## `background/`

Contiene logica especializada del fondo.

Archivos actuales:

- `Starfield.hpp/.cpp`

Responsabilidades:

- Generar y actualizar estrellas procedurales.
- Renderizar efectos de fondo que no sean entidades principales del gameplay.

Los elementos de fondo orquestados por JSON, como rocas flotantes, actualmente usan `BackgroundElement` en `entities/` y `BackgroundElementDirector` en `systems/`.

## `ui/`

Carpeta reservada para interfaz de usuario.

Uso recomendado:

- HUD de vida, score, stage time y FPS.
- Pantallas de pausa.
- Game over.
- Menus.
- Indicadores visuales no ligados a una entidad concreta.

Actualmente puede estar vacia. La idea es evitar que `Game` o `PlayState` acumulen demasiado codigo de UI a medida que crezca el juego.

## Flujo de Update Recomendado

El orden debe ser determinista y facil de razonar:

```txt
1. StageDirector / BackgroundElementDirector
2. Spawns mediante EnemySpawner
3. Starfield y elementos de fondo
4. Player
5. Proyectiles del jugador
6. Enemigos y MovementPatternSystem
7. BulletPatternSystem para disparos enemigos
8. Balas y lasers enemigos
9. Explosiones y efectos
10. Limpieza de entidades fuera de pantalla
11. CollisionSystem
12. EventQueue
13. Limpieza de entidades destruidas
```

## Flujo de Eventos

`EventQueue` comunica eventos importantes sin acoplar directamente los sistemas.

Eventos actuales:

- `EnemyDestroyedEvent`
- `PlayerHitEvent`

Ejemplo de flujo:

```txt
CollisionSystem detecta impacto
        |
EventQueue publica EnemyDestroyedEvent
        |
PlayState consume el evento
        |
PlayState crea la explosion correspondiente
```

En fases futuras, otros sistemas podran reaccionar al mismo evento:

- `EffectSystem`: crear explosiones o particulas.
- `ScoreSystem`: sumar puntos.
- `AudioSystem`: reproducir sonidos.
- `PowerUpSystem`: decidir drops.

## Criterios Para Agregar Codigo Nuevo

Agregar en `entities/` cuando:

- Sea un objeto del mundo del juego.
- Tenga posicion, hitbox, sprite, vida o estado propio.

Agregar en `systems/` cuando:

- Procese varias entidades.
- Aplique reglas del juego.
- Coordine comportamiento data-driven.
- Publique o consuma eventos.

Agregar en `config/` cuando:

- Lea o valide JSON.
- Convierta datos externos en estructuras C++.

Agregar en `core/` cuando:

- Sea infraestructura general del juego.
- No dependa de reglas especificas de gameplay.

Agregar en `ui/` cuando:

- Sea informacion visual para el jugador.
- No pertenezca al sprite o estado interno de una entidad.

## Direccion de Crecimiento

La arquitectura actual permite crecer sin reescribir todo. Las siguientes extracciones naturales serian:

- `EffectSystem`: crear explosiones y efectos al consumir eventos.
- `EnemyShootingSystem`: sacar la coordinacion de disparos enemigos desde `PlayState`.
- `ProjectileSystem`: actualizar y limpiar proyectiles del jugador y enemigos.
- `ScoreSystem`: escuchar `EnemyDestroyedEvent`.
- `AudioSystem`: escuchar eventos y reproducir sonidos.
- `World` o `EntityRegistry`: agrupar colecciones de entidades para reducir el tamano de `PlayState`.

La regla general es extraer un sistema cuando una responsabilidad empieza a repetirse o a crecer, no antes.
