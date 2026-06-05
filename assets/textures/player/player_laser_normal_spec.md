# Player Laser Normal Spec

Nombre: `player_laser_normal`

Tipo: proyectil del jugador

Dano: `1`

Tamano sugerido por frame:
`4x12 px`

Fogueo sugerido:
`8x8 px`, duracion `0.04` a `0.08` segundos

Uso:
Disparo basico desde el centro de la nave del jugador.

Spawn:
Centro X de la nave, ligeramente por encima de la nariz/cabina.

Paleta:
- nucleo blanco/celeste
- borde azul brillante
- fogueo amarillo/blanco con pequenos acentos naranjos

Prompt para herramienta IA de pixel art:

```text
Create a strict pixel art sprite sheet for a normal player laser projectile in a 90s vertical arcade shmup inspired by Batsugun and Raiden.

Frame 1: normal laser projectile, vertical, 4x12 px, bright cyan blue edge, white core, transparent background.

Frame 2: small muzzle flash, 8x8 px, white/yellow center, small orange pixels, transparent background.

Style: strict low resolution pixel art, hard square pixels, no blur, no antialiasing, no shadows, no text, no watermark.

Gameplay readability: must be clearly a player shot, less visually dominant than enemy bullets, readable at 240x320 logical resolution.
```

## Archivos Finales Esperados

```txt
assets/textures/player/player_laser_normal.png
assets/textures/player/player_laser_muzzle_flash.png
```

## PixelAI / Aseprite

Usar PixelAI dentro de Aseprite para generar los sprites finales.

No generar estos PNG con:

- Python
- PIL
- canvas
- sprites procedurales
- rectangulos runtime como arte final

Los rectangulos SFML actuales son solo placeholder de gameplay.

## Prompt PixelAI: Laser Normal

```text
Create a strict pixel art sprite for a normal player laser projectile for a 90s vertical arcade shmup inspired by Batsugun, Raiden, Truxton, and DoDonPachi.

Canvas: 4x12 pixels.
Background: transparent.
Direction: vertical, firing upward.
Shape: thin energy laser bolt, straight vertical silhouette.
Palette: bright cyan outer edge, white center core, tiny darker blue pixels at the bottom.
Style: strict low-resolution pixel art, hard square pixels only.
Gameplay role: basic player shot, readable at 240x320 resolution, less visually dominant than enemy bullets.

Constraints:
- no blur
- no antialiasing
- no glow outside the 4x12 canvas
- no semi-transparent pixels
- no background
- no text
- no diagonal direction
- no irregular subpixel edges
```

## Prompt PixelAI: Fogueo

```text
Create a strict pixel art muzzle flash sprite for a player ship firing a normal laser in a 90s vertical arcade shmup.

Canvas: 8x8 pixels.
Background: transparent.
Shape: compact burst centered on the ship nose, vertical diamond/star shape, not circular.
Palette: white hot center, yellow body, two or three orange pixels near the bottom.
Style: strict low-resolution pixel art, hard square pixels only.
Animation use: appears briefly for 0.04 to 0.08 seconds when firing.

Constraints:
- no blur
- no antialiasing
- no soft glow
- no semi-transparent pixels
- no background
- no text
- must align to the pixel grid
- must not move the ship sprite visually
```

## Export Desde Aseprite

Exportar:

```txt
player_laser_normal.png        4x12 px
player_laser_muzzle_flash.png  8x8 px
```

Configuracion:

```txt
Color mode: RGBA
Background: transparent
Resize: none
Scale: 1x
Filtering: nearest / none
```

## Integracion

- Mantener el laser alineado a pixeles enteros al renderizar.
- No usar antialiasing.
- Hitbox sugerida igual o menor que el sprite visual.
- Spawn en el centro X de la nave.
- El fogueo debe compartir el mismo eje X que el laser.
- El fogueo dura muy poco y no debe ocultar balas enemigas.
- Cuando existan los PNG finales, reemplazar el placeholder runtime por sprites cargados con `AssetManager`.
