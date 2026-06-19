# enemy_mechanical_spiked_shell

Enemigo circular mecanico con pinchos, derivado del concepto de tortuga mecanica pero sin cabeza, patas ni cola.

## Asset

| Archivo | Tamano | Uso |
| --- | ---: | --- |
| `textures/enemies/enemy_mechanical_spiked_shell.png` | 128x32 px | Spritesheet de 4 frames |

## Animacion

- 4 frames horizontales.
- Frame: 32x32 px.
- Animacion de giro por cambio de frame, sin rotar el sprite por transform.
- Centro rojo debe permanecer alineado entre frames.

## Gameplay

- Enemy id: `enemy_mechanical_spiked_shell`.
- Patron de disparo: `enemy_mecha_butterfly_aimed_random_cone`.
- Movimiento: `mecha_butterfly_center_hold`.
- Dispara desde el centro del sprite.
