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
- `enemy_green_cargo_tank` dispara `enemy_cargo_missile` con el mismo comportamiento de `enemy_green_cargo_parallel_missiles`, pero el visual aprobado ahora es un laser naranja vertical.
  - asset: `assets/textures/projectiles/enemy_cargo_missile.png`
  - tamano: `8x26`
  - estilo: borde naranja, cuerpo naranja intenso y nucleo amarillo alargado.
- En `PlayState`, los proyectiles enemigos ya no se eliminan repentinamente cuando se destruye la nave.
  - `enemyBullets_` sigue actualizandose durante el estado `playerDestroyed_`.
  - Las balas se limpian solo cuando dejan de estar vivas/salen del area logica.
  - `enemyLasers_` todavia se limpian al morir la nave.
- Cuando muere `enemy_cargo_sphere_turret`, sus orbes no se eliminan aunque su patron tenga `clear_bullets_on_owner_destroyed: true`.
  - Esto aplica tambien cuando la esfera se destruye de forma forzada al morir el `enemy_green_cargo_tank` que la lleva.
- La tecla `T` alterna modo TATE en `Game`.
  - La simulacion sigue en resolucion logica `240x320`.
  - Se rota el framebuffer final 90 grados y se recalcula la escala entera contra `320x240`.
  - La grilla debug usa la misma transformacion para mantenerse alineada.
  - `GAME OVER` se dibuja en el framebuffer logico para rotar/escalar junto con el juego.
- La nave inicia con laser doble mediante `initial_projectile_count: 2` en `config/player.json`.
  - `Player` limita ese valor entre 1 y 8.
  - Los power-ups `P` siguen duplicando el conteo hasta 8.
  - El dano por proyectil se balancea con `projectile_damage_by_count`: `1=1.0`, `2=1.0`, `4=0.65`, `6=0.50`, `8=0.42`.
  - Si no hay valor para la cantidad actual, se usa `laser_damage`.
- Se preparo un patron data-driven `enemy_armored_flail_tether` de tipo `tethered_flail`.
  - El mangual sigue anclado al enemigo, orbita, se extiende cada 3 segundos, se retrae y vuelve a orbitar.
  - La cadena se renderiza desde runtime y la bola persiste al golpear al jugador.
  - El enemigo aprobado se integro como `enemy_armored_flail_ship`.
  - `enemy_armored_flail_ship` tambien usa `secondary_pattern: enemy_mecha_butterfly_aimed_random_cone` desde la punta de la nariz con offset `(0, 34)`.
  - Los enemigos soportan `secondary_pattern` con timer independiente y `secondary_spawn_offset_x/y` desde `config/enemies.json`.
  - Asset nave: `assets/textures/enemies/enemy_armored_flail_ship.png`, 56x80, nariz hacia abajo y cohetes arriba.
  - Asset bola: `assets/textures/projectiles/enemy_flail_ball.png`, 18x18.
  - Explosion propia: `assets/textures/effects/explosion_enemy_armored_flail_ship.png`, 192x80, 3 frames de 64x80, paleta de acero/plata con chispas azules y nucleo naranja.
  - Spawn de prueba en `config/stage_01_enemies.json` al tiempo 30.0.
- Se integro un planeta tipo Tierra como background element.
  - Asset: `assets/textures/background/earth_like_planet_horizon.png`, 240x320.
  - Tileset: `earth_like_planet`.
  - Spawn en `config/stage_01_background_elements.json` al segundo 60.0, `x=0`, `y=-300`, `speed_y=4.5`.
  - `PlayState` soporta este tileset con tile size especial de 240x320.
  - El asset debe verse como planeta redondo recortado en los laterales, no como ovalo achatado.
  - El alpha del planeta fue extendido hasta los bordes laterales del PNG para evitar franjas verticales negras visibles durante el scroll.
- Se integro `enemy_mechanical_spiked_shell`.
  - Concepto inicial de tortuga guardado en `assets/textures/enemies/concepts/enemy_mechanical_turtle_concept.png`.
  - Asset final: `assets/textures/enemies/enemy_mechanical_spiked_shell.png`, 128x32, 4 frames de 32x32.
  - Es un circulo mecanico con pinchos, tonos metalicos verdes oscuros y centro rojo, sin cabeza/patas/cola.
  - Usa `enemy_mecha_butterfly_aimed_random_cone` y `mechanical_spiked_shell_drop_hold_exit`.
  - Movimiento: baja verticalmente hasta `y=80`, espera 7 segundos y luego sigue bajando hasta salir de pantalla.
  - Dispara desde el centro.
  - Explosion propia: `assets/textures/effects/explosion_enemy_mechanical_spiked_shell.png`, 144x48, 3 frames de 48x48, paleta verde metalica con nucleo rojo/naranja.

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
