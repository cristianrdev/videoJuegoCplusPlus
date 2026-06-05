---

name: pixel-art-shmup-style
description: Utilizar esta skill cuando se diseñen sprites, naves, enemigos, jefes, balas, power-ups, fondos, tiles, props, explosiones y entornos pixel art para un shooter vertical 2D en C++ con SFML, usando una resolución lógica arcade inspirada en Batsugun de 240x320 píxeles verticales.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# Pixel Art Shmup Style

## Objetivo

Mantener consistencia visual en todos los gráficos del juego.

El juego usa una resolución lógica inspirada en Batsugun:

```txt
Resolución lógica vertical: 240x320 px
Área visible de juego: 240 px ancho x 320 px alto
Estilo: arcade shooter vertical 16-bit / pixel art
```

Todos los sprites deben diseñarse pensando en ese marco de resolución.

---

## Regla principal

Escalar el render target lógico 240x320 a una ventana 1080p.
Priorizar pixel perfect:
- renderizar internamente a 240x320
- escalar con nearest-neighbor
- usar escala entera x3 cuando sea posible: 720x960
- centrar la imagen dentro de una ventana 1920x1080 o 1080x1080 según configuración
- no deformar aspect ratio

No crear sprites demasiado grandes ni demasiado pequeños para la pantalla.

Cada sprite debe respetar el porcentaje que ocupa dentro de una pantalla de 240x320.

---

# Escala base

## Jugador

```txt
Nave del jugador: 32x32 px
Hitbox real: 4x4 a 8x8 px
Área visual máxima recomendada: 32x32 px
```

Reglas:

* La nave debe mirar hacia arriba.
* Debe ser simétrica o casi simétrica.
* Debe tener silueta clara.
* El centro visual debe coincidir con el centro del hitbox.
* No llenar completamente los 32x32 px; dejar aire visual.
* La cabina puede ocupar entre 6x8 y 10x12 px.

---

## Balas del jugador

```txt
Bala pequeña: 4x8 px
Bala mediana: 8x12 px
Bala grande: 12x16 px
Láser delgado: 4x24 px
Láser ancho: 8x32 px
```

Reglas:

* Deben diferenciarse claramente de las balas enemigas.
* Usar colores más brillantes/fríos para jugador.
* No tapar demasiado la nave.
* Deben ser legibles en movimiento.

---

## Balas enemigas

```txt
Bala pequeña: 4x4 px
Bala estándar: 6x6 px
Bala grande: 8x8 px
Orbe peligroso: 12x12 px
Misil: 8x16 px
```

Reglas:

* Deben ser muy legibles.
* Deben contrastar con el fondo.
* No usar el mismo color principal que las balas del jugador.
* La forma debe indicar peligro.
* La hitbox puede ser menor que el sprite visible.

---

## Enemigos pequeños

```txt
Tamaño recomendado: 16x16 px a 24x24 px
```

Uso:

* Enemigos rápidos.
* Oleadas.
* Naves débiles.
* Drones.

Reglas:

* Deben morir rápido.
* Silueta simple.
* Pocos detalles.
* Fácil lectura en grupos.

---

## Enemigos medianos

```txt
Tamaño recomendado: 32x32 px a 48x48 px
```

Uso:

* Enemigos con más vida.
* Subunidades.
* Torres.
* Naves de ataque.

Reglas:

* Pueden tener más detalle.
* Deben tener un punto visual claro de disparo.
* No deben confundirse con el jugador.

---

## Enemigos grandes

```txt
Tamaño recomendado: 64x64 px a 96x96 px
```

Uso:

* Minibosses.
* Naves pesadas.
* Tanques voladores.
* Estructuras destructibles.

Reglas:

* Deben ocupar espacio importante.
* Deben tener partes reconocibles.
* Pueden tener puntos débiles.
* Evitar cubrir más de un tercio de pantalla salvo en boss.

---

## Bosses

```txt
Boss pequeño: 96x96 px
Boss mediano: 128x128 px
Boss grande: 160x128 px
Boss máximo recomendado: 200x160 px
```

Reglas:

* Un boss puede ocupar gran parte del ancho de pantalla.
* Debe dejar espacio jugable.
* Debe tener zonas claras de disparo.
* Debe tener silueta fuerte.
* Las partes móviles deben estar separadas en sprites si se animan.

---

# Power-ups

```txt
Power-up pequeño: 16x16 px
Power-up estándar: 24x24 px
Power-up grande/especial: 32x32 px
```

Tipos sugeridos:

```txt
power
speed
bomb
shield
score
life
option
```

Reglas:

* Deben ser más amigables visualmente que las balas enemigas.
* Deben tener contorno claro.
* Deben flotar o parpadear.
* Deben ser reconocibles incluso en 16x16 px.

---

# Explosiones y efectos

## Explosión pequeña

```txt
Sprite frame: 16x16 px
Frames: 4 a 6
```

## Explosión mediana

```txt
Sprite frame: 32x32 px
Frames: 6 a 8
```

## Explosión grande

```txt
Sprite frame: 64x64 px
Frames: 8 a 12
```

Reglas:

* La explosión debe durar poco.
* No debe ocultar balas peligrosas por demasiado tiempo.
* Usar transparencias o dithering si corresponde.
* Mantener claridad del gameplay por sobre espectacularidad.

---

# Fondos y entornos

## Resolución base del fondo

```txt
Ancho: 240 px
Alto de segmento: 320 px o múltiplos de 320 px
```

Ejemplos:

```txt
background_stage_01.png      240x960 px
background_stage_01_loop.png 240x640 px
```

Reglas:

* El fondo debe desplazarse verticalmente.
* No debe competir visualmente con balas.
* Evitar colores demasiado parecidos a las balas enemigas.
* Mantener contraste moderado.
* Los detalles importantes deben ser menos saturados que sprites activos.

---

## Tiles

```txt
Tile pequeño: 8x8 px
Tile estándar: 16x16 px
Tile grande: 32x32 px
```

Recomendación:

* Usar 16x16 px como tile base.
* Usar 8x8 px para detalles.
* Usar 32x32 px para estructuras grandes.

---

## Props del entorno

```txt
Prop pequeño: 16x16 px
Prop mediano: 32x32 px
Prop grande: 64x64 px
Estructura grande: 96x96 px o más
```

Ejemplos:

```txt
torres
edificios
plataformas
antenas
cráteres
nubes
restos de naves
```

Reglas:

* Los props no interactivos deben verse menos importantes que enemigos.
* Los props destructibles deben tener más contraste.
* Los props no deben confundirse con power-ups o balas.

---

# Paleta y contraste

## Reglas generales

* Jugador: colores fríos o heroicos.
* Enemigos: colores cálidos, metálicos u oscuros.
* Balas enemigas: colores muy visibles.
* Balas del jugador: color distinto al de balas enemigas.
* Fondo: menor saturación que sprites activos.
* Power-ups: colores brillantes y amigables.

---

## Prioridad visual

Orden de importancia visual:

```txt
1. Balas enemigas
2. Jugador
3. Enemigos
4. Balas del jugador
5. Power-ups
6. Explosiones
7. Fondo
8. Props decorativos
```

Si un gráfico compite visualmente con las balas enemigas, debe reducir saturación, brillo o contraste.

---

# Contornos

Usar contornos cuando mejore la lectura.

Recomendaciones:

```txt
Sprites pequeños: contorno de 1 px
Sprites medianos: contorno de 1 px
Bosses: contorno parcial o sombras internas
Balas: contorno muy simple o brillo exterior
```

Evitar contornos demasiado gruesos porque reducen detalle útil.

---

# Animaciones

## Jugador

```txt
Idle: 2 a 4 frames
Movimiento lateral: 2 a 3 frames por dirección
Daño: flash corto
Muerte: explosión mediana/grande
```

## Enemigos pequeños

```txt
Idle/avance: 2 a 4 frames
Destrucción: explosión pequeña
```

## Enemigos medianos

```txt
Idle: 2 a 4 frames
Disparo: 1 a 2 frames
Destrucción: explosión mediana
```

## Boss

```txt
Idle: 2 a 4 frames
Partes móviles: animación separada
Disparo: flash o apertura de cañón
Cambio de fase: efecto visual claro
Destrucción: varias explosiones encadenadas
```

---

# Nombres de archivos

Usar nombres claros y consistentes.

```txt
player_ship_01.png
player_ship_01_bank_left.png
player_bullet_small_blue.png

enemy_small_fighter_01.png
enemy_medium_tank_01.png
enemy_big_carrier_01.png

boss_stage_01_body.png
boss_stage_01_turret_left.png
boss_stage_01_turret_right.png

bullet_enemy_red_small.png
bullet_enemy_orange_big.png
bullet_player_blue_laser.png

powerup_power.png
powerup_bomb.png
powerup_speed.png

bg_stage_01_city.png
tile_stage_01_metal_16.png
prop_stage_01_tower_32.png

explosion_small_16.png
explosion_medium_32.png
explosion_big_64.png
```

---

# Regla de tamaño máximo por pantalla

En una pantalla de 240x320:

```txt
Jugador 32 px ancho    ≈ 13% del ancho
Enemigo pequeño 16 px  ≈ 6.6% del ancho
Enemigo mediano 32 px  ≈ 13% del ancho
Enemigo grande 64 px   ≈ 26% del ancho
Boss 160 px ancho      ≈ 66% del ancho
```

No generar sprites sin considerar estos porcentajes.

---

# Reglas para generación de sprites

Cuando se pida crear un sprite, Codex debe proponer:

```txt
Nombre del sprite
Tamaño exacto en píxeles
Uso dentro del juego
Cantidad de frames
Paleta sugerida
Descripción visual
Hitbox sugerida
Archivo destino
```

Ejemplo:

```txt
Nombre: enemy_small_fighter_01
Tamaño: 24x24 px
Uso: enemigo pequeño de oleada inicial
Frames: 2
Paleta: gris oscuro, rojo, naranja
Hitbox: 14x14 px
Archivo: assets/textures/enemies/enemy_small_fighter_01.png
```

---

# Reglas para evitar inconsistencias

No hacer:

* Nave del jugador de 64x64 px.
* Bala enemiga común de 20x20 px.
* Enemigo pequeño más grande que la nave.
* Power-up más grande que un enemigo mediano.
* Fondo con colores más brillantes que las balas.
* Props decorativos con el mismo contraste que enemigos.
* Boss que ocupe toda la pantalla sin dejar espacio jugable.
* Sprites con resoluciones aleatorias como 37x41 px salvo caso justificado.

Hacer:

* Usar múltiplos comunes: 4, 8, 16, 24, 32, 48, 64, 96, 128.
* Mantener siluetas claras.
* Priorizar legibilidad.
* Diseñar primero en baja resolución.
* Escalar solo con nearest-neighbor.
* Mantener coherencia entre tamaño visual e importancia gameplay.

---

# Escalado

El juego puede renderizar internamente a 240x320 y escalar a:

```txt
480x640   escala x2
720x960   escala x3
960x1280  escala x4
```

Reglas:

* Usar nearest-neighbor.
* No aplicar blur.
* No usar escalado fraccional si se puede evitar.
* Mantener aspect ratio vertical.

---

# Checklist antes de aceptar un sprite

Antes de aceptar un nuevo sprite, validar:

```txt
¿Respeta la resolución lógica 240x320?
¿Su tamaño corresponde a su importancia en gameplay?
¿Tiene silueta clara?
¿Se distingue del fondo?
¿Se distingue de balas y power-ups?
¿Usa dimensiones coherentes?
¿Tiene nombre de archivo correcto?
¿Tiene hitbox sugerida?
¿Tiene cantidad de frames definida?
```

---

# Tabla rápida de tamaños

```txt
Jugador:             32x32
Bala jugador:         4x8, 8x12, 12x16
Bala enemiga:         4x4, 6x6, 8x8, 12x12
Power-up:             16x16, 24x24, 32x32
Enemigo pequeño:      16x16, 24x24
Enemigo mediano:      32x32, 48x48
Enemigo grande:       64x64, 96x96
Miniboss:             96x96
Boss:                 128x128, 160x128, 200x160
Tile:                 8x8, 16x16, 32x32
Prop:                 16x16, 32x32, 64x64, 96x96
Explosión pequeña:    16x16
Explosión mediana:    32x32
Explosión grande:     64x64
Fondo visible:        240x320
Fondo scroll:         240x640, 240x960, 240x1280
```

---

## Herramientas recomendadas para generar sprites

Codex no debe intentar reemplazar una herramienta de arte pixel art.

Codex debe encargarse de:

- Definir tamaño exacto del sprite.
- Definir cantidad de frames.
- Crear prompts consistentes.
- Validar que el sprite respete la resolución lógica 240x320.
- Crear carpetas y nombres de archivos.
- Integrar los PNG al proyecto SFML.
- Generar scripts de postproceso si es necesario.

Herramientas recomendadas:

### GPT Image / ChatGPT Image

Usar para:

- Concept art rápido.
- Sprites base.
- Variantes de naves.
- Enemigos.
- Bosses.
- Power-ups.
- Fondos conceptuales.

Regla:

La imagen generada debe revisarse y limpiarse antes de usarla como asset final.

### PixelLab

Usar para:

- Pixel art más orientado a juegos.
- Tiles.
- Fondos.
- Entornos.
- Spritesheets.
- Variantes de estilo.

### Aseprite

Usar como herramienta principal de acabado.

Usar para:

- Limpiar píxeles incorrectos.
- Ajustar silueta.
- Corregir animaciones.
- Separar frames.
- Revisar transparencia.
- Exportar spritesheets.

### PixelAI para Aseprite

Usar si se quiere generar arte directamente dentro de Aseprite.

Útil para:

- Prototipos rápidos.
- Variantes de sprites.
- Capas nuevas.
- Frames adicionales.

### Pixie.haus

Usar para:

- Sprites pequeños.
- Pixel art con grilla estricta.
- Fondos transparentes.
- Paletas limitadas.
- Prototipos exportables.

## Flujo recomendado

Para cada sprite, Codex debe entregar:

```txt
Nombre:
Tamaño:
Cantidad de frames:
Uso:
Paleta:
Hitbox:
Prompt para IA de imagen:
Ruta destino:
Notas para limpiar en Aseprite:

# Resultado esperado

Cuando Codex use esta skill debe mantener coherencia visual en todos los gráficos pixel art del juego, evitando sprites desproporcionados y respetando siempre la resolución lógica vertical de 240x320 inspirada en Batsugun.
