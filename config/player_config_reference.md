# Player Config Reference

Archivo configurado:

```txt
config/player.json
```

Este archivo centraliza los parametros principales de la nave del jugador, su disparo normal, su hitbox y algunos detalles visuales. Las unidades de gameplay usan la resolucion logica del juego:

```txt
240x320 px
```

Los valores de posicion, velocidad, tamano y offsets se expresan en pixeles logicos, no en pixeles escalados de la ventana.

## Diccionario De Variables

| Variable | Tipo | Unidad | Descripcion |
|---|---:|---:|---|
| `ship_texture` | string | ruta | Ruta relativa dentro de `assets/` para el sprite sheet de la nave. Debe apuntar a un PNG cargable por `AssetManager`. |
| `thruster_texture` | string | ruta | Ruta relativa dentro de `assets/` para la animacion de fuego de las toberas. |
| `laser_texture` | string | ruta | Ruta relativa dentro de `assets/` para el proyectil normal de la nave. |
| `muzzle_flash_texture` | string | ruta | Ruta relativa dentro de `assets/` para el fogueo que aparece al disparar. |
| `health` | int | puntos | Vida inicial de la nave. Cada impacto enemigo reduce este valor segun el dano recibido. |
| `speed` | float | px/s | Velocidad de movimiento de la nave en pixeles logicos por segundo. Usa `deltaTime`. |
| `hitbox_width` | float | px | Ancho de la hitbox de la nave. Es independiente del sprite visual. |
| `hitbox_height` | float | px | Alto de la hitbox de la nave. Es independiente del sprite visual. |
| `sprite_render_width` | float | px | Ancho final al que se renderiza la nave dentro del mundo logico. |
| `sprite_render_height` | float | px | Alto final al que se renderiza la nave dentro del mundo logico. |
| `clamp_half_size` | float | px | Margen usado para impedir que la nave salga del area jugable. Normalmente coincide con la mitad visual del sprite. |
| `laser_spawn_offset_y` | float | px | Offset vertical desde el centro de la nave hasta el punto donde aparece el laser. Valores negativos salen hacia arriba. |
| `fire_cooldown_seconds` | float | segundos | Cadencia de disparo. Tiempo minimo entre disparos normales. Menor valor significa disparar mas rapido. |
| `muzzle_flash_seconds` | float | segundos | Duracion visible del fogueo al disparar. |
| `laser_speed` | float | px/s | Velocidad vertical del laser normal de la nave. |
| `laser_damage` | float | puntos | Dano base que causa cada laser normal al impactar un enemigo. Se usa como fallback si no hay entrada en `projectile_damage_by_count`. |
| `projectile_damage_by_count` | object | puntos | Dano por proyectil segun la cantidad de lasers activos. Las claves son cantidades como `"2"`, `"4"`, `"6"` y `"8"`. |
| `thruster_animation_seconds` | float | segundos/frame | Duracion de cada frame de la animacion de las toberas. Menor valor anima mas rapido. |
| `thruster_frame_width` | int | px | Ancho de cada frame dentro del sprite sheet de toberas. |
| `thruster_frame_height` | int | px | Alto maximo de cada frame dentro del sprite sheet de toberas. |
| `thruster_idle_height` | int | px | Alto del fuego de tobera cuando la nave no empuja verticalmente. |
| `thruster_forward_height` | int | px | Alto del fuego de tobera cuando la nave avanza hacia arriba. |
| `thruster_backward_height` | int | px | Alto del fuego de tobera cuando la nave retrocede hacia abajo. |
| `thruster_left_offset_x` | float | px | Offset horizontal de la tobera izquierda respecto al centro de la nave. |
| `thruster_right_offset_x` | float | px | Offset horizontal de la tobera derecha respecto al centro de la nave. |
| `thruster_offset_y` | float | px | Offset vertical de las toberas respecto al centro de la nave. |

## Ajustes Comunes

### Aumentar La Vida De La Nave

```json
"health": 5
```

Sube la cantidad de impactos que puede recibir la nave.

### Aumentar La Velocidad De Movimiento

```json
"speed": 145.0
```

Hace que la nave se desplace mas rapido. Conviene ajustar con cuidado para no perder precision en una resolucion logica de `240x320`.

### Cambiar La Cadencia De Disparo

```json
"fire_cooldown_seconds": 0.08
```

Menor valor significa mas disparos por segundo.

Ejemplos aproximados:

| Valor | Resultado |
|---:|---|
| `0.20` | 5 disparos por segundo |
| `0.12` | 8.3 disparos por segundo |
| `0.08` | 12.5 disparos por segundo |

### Aumentar El Dano Del Laser

```json
"laser_damage": 2
```

Cada proyectil de la nave quitara mas vida a los enemigos.

### Ajustar Dano Por Cantidad De Proyectiles

```json
"projectile_damage_by_count": {
  "1": 1.0,
  "2": 1.0,
  "4": 0.65,
  "6": 0.50,
  "8": 0.42
}
```

Permite que el power-up aumente la cobertura visual sin duplicar el dano total de golpe. Si la cantidad actual de proyectiles no existe en esta tabla, se usa `laser_damage`.

### Aumentar La Velocidad Del Laser

```json
"laser_speed": 320.0
```

El laser subira mas rapido por la pantalla.

### Ajustar La Hitbox

```json
"hitbox_width": 6.0,
"hitbox_height": 6.0
```

La hitbox debe permanecer pequena y centrada para mantener una sensacion justa de shmup arcade. No tiene que coincidir con el tamano visual de la nave.

## Reglas Pixel Perfect

- Mantener valores enteros o medios simples cuando afecten posiciones visuales.
- Evitar offsets fraccionarios innecesarios.
- Las texturas deben permanecer con `setSmooth(false)`.
- El sprite puede moverse con `deltaTime`, pero se renderiza alineado a pixeles enteros.
- No escalar texturas de proyectiles dentro del gameplay; ajustar el PNG si se necesita otro tamano.

## Notas De Balance

- `speed`, `fire_cooldown_seconds`, `laser_speed`, `laser_damage` y `projectile_damage_by_count` afectan directamente la dificultad.
- Cambiar `laser_damage` puede volver inutiles enemigos con poca vida si no se ajusta `config/enemies.json`.
- Cambiar `fire_cooldown_seconds` tambien puede afectar rendimiento si se generan demasiados proyectiles.
- Para un shmup arcade vertical, es recomendable que la nave sea rapida pero precisa, y que la hitbox sea mas pequena que el sprite visual.
