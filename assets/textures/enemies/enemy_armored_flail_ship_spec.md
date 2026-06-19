# enemy_armored_flail_ship

Enemigo nave acorazada con tema medieval.

## Assets

| Archivo | Tamano | Uso |
| --- | ---: | --- |
| `textures/enemies/enemy_armored_flail_ship.png` | 56x80 px | Nave enemiga, nariz hacia abajo, propulsores arriba |
| `textures/projectiles/enemy_flail_ball.png` | 18x18 px | Bola con pinchos del mangual |
| `textures/projectiles/enemy_flail_chain_sample.png` | 8x32 px | Referencia visual de cadena |

## Gameplay

- Enemigo: `enemy_armored_flail_ship`.
- Patron: `enemy_armored_flail_tether`.
- Proyectil: `enemy_tethered_flail`.
- El mangual orbita alrededor de la nave, se lanza cada 3 segundos, extiende la cadena y vuelve a retraerse.
- La cadena se renderiza en runtime para poder estirarse dinamicamente.

## Orientacion

- La nave mira hacia abajo.
- La nariz/punta queda en la parte inferior del sprite.
- Los cohetes/propulsores quedan en la parte superior.
