# Enemy Sprite And Pattern Specs

## Enemigos

### `enemy_drone`

Archivo final:
`assets/textures/enemies/enemy_drone.png`

Fuente:
`assets/textures/enemies/enemy_drone_source.png`

Tamano final:
`24x24 px`

Vida:
`3`

Patron:
`enemy_drone_aimed_single`

Descripcion:
Drone pequeno rojo/naranja con canon frontal. Dispara una bala dirigida al jugador.

### `enemy_turret_pod`

Archivo final:
`assets/textures/enemies/enemy_turret_pod.png`

Fuente:
`assets/textures/enemies/enemy_turret_pod_source.png`

Tamano final:
`24x24 px`

Vida:
`3`

Patron:
`enemy_turret_spread_3`

Descripcion:
Torre flotante compacta. Dispara abanico de 3 balas hacia abajo.

### `enemy_interceptor`

Archivo final:
`assets/textures/enemies/enemy_interceptor.png`

Fuente:
`assets/textures/enemies/enemy_interceptor_source.png`

Tamano final:
`24x24 px`

Vida:
`3`

Patron:
`enemy_interceptor_wave_5`

Descripcion:
Interceptor pequeno verde/teal. Dispara abanico de 5 balas hacia abajo.

## Configuracion De Patrones

Archivo:
`config/bullet_patterns.json`

Cada patron define:

```txt
id
fire_interval
bullet_speed
aimed
angle_offsets
```

Notas:
- `aimed: true` usa la direccion hacia el jugador como angulo base.
- `aimed: false` usa direccion hacia abajo como angulo base.
- `angle_offsets` permite crear single shot, spreads y abanicos sin tocar codigo C++.
