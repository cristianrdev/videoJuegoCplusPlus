# Player Ship Sprite Sheet Spec

Nombre: `player_ship_sheet`

Tamano final requerido: `96x32 px`

Cantidad de frames: `3`

Tamano por frame: `32x32 px`

Orden de frames:
1. `center`
2. `bank_left`
3. `bank_right`

Uso: nave del jugador para shooter vertical arcade.

Ruta fuente IA:
`assets/textures/player/player_ship_sheet_ai_source.png`

Ruta fuente con transparencia:
`assets/textures/player/player_ship_sheet_ai_transparent.png`

Ruta final para el juego:
`assets/textures/player/player_ship_sheet.png`

Paleta:
- gris metalico claro y oscuro para fuselaje
- azul brillante para cabina vertical
- rojo para acentos pequenos en nariz y alas
- naranja/amarillo para fuego de motores
- outline oscuro de 1 px en la version final

Hitbox sugerida:
`6x6 px` centrada en el fuselaje, independiente del sprite visual.

Prompt usado para IA de imagen:

```text
Create a strict pixel art sprite sheet of a player spaceship for a 90s vertical arcade shooter inspired by Batsugun, Raiden, Truxton, and DoDonPachi.

Technical layout: exactly 3 frames in one horizontal row, each frame is 32x32 pixels conceptually: frame 1 center, frame 2 bank_left, frame 3 bank_right.

Subject: compact player spaceship, always facing upward. Metallic gray body, bright blue vertical cockpit, small red accents, dark crisp outline, small orange engine flames.

Banking rules: DO NOT rotate the sprite. DO NOT make the ship point diagonally. In all 3 frames the nose and cockpit remain vertical and point straight up. For bank_left, the left wing appears slightly lower and the right wing slightly higher. For bank_right, the right wing appears slightly lower and the left wing slightly higher. The effect is lateral arcade banking, not diagonal aiming.

Style/medium: strict low resolution pixel art, 16-bit arcade style, hard square pixels, no blur, no antialiasing, clean readable silhouette at 32x32.

Scene/backdrop: perfectly flat solid #00ff00 chroma-key background for later background removal, no shadows, no gradients, no texture.

Constraints: no text, no labels, no UI, no watermark, no background scene, no perspective rotation, no diagonal nose direction, no antialiasing, no smooth edges. Leave padding around each 32x32 frame.
```

Notas para limpiar/exportar en Aseprite o PixelLab:
- Importar `player_ship_sheet_ai_transparent.png` como referencia visual, no como pixel final directo.
- Crear un canvas final de `96x32 px`.
- Dibujar cada frame en una celda exacta de `32x32 px`.
- Mantener la punta de la nave alineada verticalmente en los tres frames.
- No rotar el sprite para banking; ajustar solo alas, pods y sombras.
- Exportar PNG con fondo transparente.
- Verificar que no haya pixeles semitransparentes ni antialiasing.
