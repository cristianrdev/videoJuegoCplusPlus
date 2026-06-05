# Enemy Robot Fish

- Asset final: `assets/textures/enemies/enemy_robot_fish_sheet.png`
- Fuente IA: `assets/textures/enemies/enemy_robot_fish_sheet_source.png`
- Tipo: enemigo grande / miniboss
- Vista: cenital/top-down
- Tamano total: `240x300 px`
- Frames: `2`
- Tamano por frame: `120x300 px`
- Orden:
  - frame `0`: idle
  - frame `1`: mouth laser firing
- Pivote en juego: centro del frame, `60x150`
- Boca / origen del laser: `position.y + 132`
- Hitbox de gameplay: separada del sprite visual, `82x252`
- Paleta: metal gris, gunmetal, brillos plata, detalles azul/cian en frame de disparo
- Render:
  - no escalar textura en runtime
  - `setSmooth(false)`
  - posicion redondeada a pixel entero
  - frames sin desplazar el eje central

## Patron asociado

- Pattern: `enemy_robot_fish_mouth_laser`
- Tipo: `continuous_laser`
- Intervalo: `1.0s`
- Duracion visual/hitbox: `0.42s`
- Laser: `enemy_robot_fish_laser`
