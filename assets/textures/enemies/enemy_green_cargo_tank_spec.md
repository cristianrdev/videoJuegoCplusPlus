# Enemy Green Cargo Tank

Asset aprobado para enemigo pesado tipo tanque/nave cargo.

## Sprites

| Asset | Tamaño | Uso |
|---|---:|---|
| `textures/enemies/enemy_green_cargo_tank.png` | 64x96 px | Cuerpo principal, vida 15 |
| `textures/enemies/enemy_cargo_sphere_turret.png` | 16x16 px | Torreta/esfera montada como enemigo separado |
| `textures/projectiles/enemy_cargo_missile.png` | 8x26 px | Laser enemigo naranja alargado con nucleo amarillo |

## Integración

- El tanque usa `enemy_green_cargo_parallel_missiles`.
- `enemy_cargo_missile` conserva su comportamiento; solo cambia su visual a laser vertical naranja.
- La esfera usa `enemy_mecha_butterfly_rotating_clock`.
- Ambos usan `cargo_tank_slow_drop` para permanecer alineados.
- El stage spawnea la esfera `0.01` segundos después para asegurar que se dibuje encima del tanque.

## Pixel Perfect

- Los sprites están en resolución final, sin escalado dentro del juego.
- Origen esperado: centro del sprite.
- Render en coordenadas enteras mediante `Enemy::render`.
