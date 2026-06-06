# Enemy Small Red Bullet

- Asset final: `assets/textures/projectiles/enemy_small_red_bullet.png`
- Fuente IA: `assets/textures/projectiles/enemy_small_red_bullet_source.png`
- Tamano: `6x6 px`
- Uso: proyectil enemigo pequeno de cadencia media-alta
- Projectile id: `enemy_small_red_bullet`
- Patron actual: `enemy_turret_spread_3`
- Paleta: rojo oscuro, rojo/naranja, nucleo amarillo/blanco
- Render:
  - no escalar textura en runtime
  - `setSmooth(false)`
  - posicion redondeada por `EnemyBullet`

## Prompt usado

Strict pixel art enemy bullet for a vertical arcade shmup, conceptually 6x6 pixels. Tiny red bullet, dark red outer pixels, bright red/orange mid pixels, small yellow-white core highlight, chroma-key background, no blur, no antialiasing.
