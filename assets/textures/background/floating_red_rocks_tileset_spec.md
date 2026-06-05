# Floating Red Rocks Tileset

- Asset final: `assets/textures/background/floating_red_rocks_tileset.png`
- Fuente IA: `assets/textures/background/floating_red_rocks_tileset_source.png`
- Uso: elementos de fondo orquestados para `stage_01`
- Vista: cenital/top-down para shooter vertical
- Estilo: pixel art arcade 90s, rocas rojas flotantes inspiradas en montanas flotantes fantasticas
- Tamano total: `500x100 px`
- Tiles: `5`
- Tamano por tile: `100x100 px`
- Orden:
  - tile `0`: roca flotante variante 1
  - tile `1`: roca flotante variante 2
  - tile `2`: roca flotante variante 3
  - tile `3`: roca flotante variante 4
  - tile `4`: roca flotante variante 5
- Render:
  - no escalar textura en runtime
  - usar `setSmooth(false)`
  - dibujar con posicion redondeada a pixel entero
  - acceder con `sf::IntRect({tile * 100, 0}, {100, 100})`

## Prompt usado

Create one horizontal tileset containing 5 variants of floating red rocks, each tile conceptually 100x100 pixels, inspired by floating mountains but original and suitable for a top-down vertical arcade shmup. Pure chroma-key background, strict 90s arcade pixel art, red rock palette, no blur, no antialiasing, no text.
