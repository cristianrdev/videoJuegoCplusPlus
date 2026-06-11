# Referencia De Movement Patterns

Archivo configurado:

```txt
config/movement_patterns.json
```

Este archivo define como se mueven los enemigos dentro de la resolucion logica del juego:

```txt
240x320 px
```

Los movimientos se referencian desde `config/stage_01_enemies.json` mediante el campo:

```json
"movement": "nombre_del_movimiento"
```

Cada patron tiene un `id` unico y un `type` que determina como se interpretan sus parametros.

## Estructura General

```json
{
  "id": "slow_drop",
  "type": "linear",
  "velocity_x": 0.0,
  "velocity_y": 14.0
}
```

| Variable | Tipo | Unidad | Descripcion |
|---|---:|---:|---|
| `id` | string | identificador | Nombre unico del patron de movimiento. Se usa desde el stage. |
| `type` | string | tipo | Tipo de movimiento. Actualmente puede ser `linear`, `cosine` o `waypoints`. |

## Tipo `linear`

Movimiento recto basado en velocidad constante.

```json
{
  "id": "fast_drop",
  "type": "linear",
  "velocity_x": 0.0,
  "velocity_y": 60.0
}
```

| Variable | Tipo | Unidad | Descripcion |
|---|---:|---:|---|
| `velocity_x` | float | px/s | Velocidad horizontal. Positivo va hacia la derecha, negativo hacia la izquierda. |
| `velocity_y` | float | px/s | Velocidad vertical. Positivo baja, negativo sube. |

### Uso Recomendado

- Enemigos simples que bajan en linea recta.
- Objetos que deben moverse sincronizados con fondos.
- Torres montadas sobre elementos de fondo.

### Ejemplo

```json
{
  "id": "floating_rock_turret_drop",
  "type": "linear",
  "velocity_x": 0.0,
  "velocity_y": 18.0
}
```

Este movimiento baja a `18 px/s`, util para sincronizar una torreta con una roca flotante que tenga:

```json
"speed_y": 18.0
```

## Tipo `cosine`

Movimiento que baja mientras oscila horizontalmente de lado a lado.

```json
{
  "id": "metal_squid_cosine_medium",
  "type": "cosine",
  "velocity_y": 32.0,
  "amplitude_x": 48.0,
  "frequency": 0.38,
  "phase": 0.0
}
```

| Variable | Tipo | Unidad | Descripcion |
|---|---:|---:|---|
| `velocity_y` | float | px/s | Velocidad vertical constante. Positivo baja. |
| `amplitude_x` | float | px | Distancia maxima que se aleja horizontalmente desde su posicion inicial. |
| `frequency` | float | ciclos/s aprox. | Frecuencia del bamboleo. Mayor valor significa oscilar mas rapido. |
| `phase` | float | radianes | Desfase inicial de la onda. Permite que enemigos iguales no se muevan sincronizados. |

### Uso Recomendado

- Enemigos que bambolean mientras bajan.
- Formaciones que quieren verse organicas.
- Enemigos tipo calamar, drone o criatura mecanica.

### Como Interpretar `amplitude_x`

Si un enemigo aparece con:

```json
"x": 120
```

y el movimiento tiene:

```json
"amplitude_x": 48.0
```

el enemigo puede moverse aproximadamente entre:

```txt
x = 72  y  x = 168
```

porque `120 - 48 = 72` y `120 + 48 = 168`.

## Tipo `waypoints`

Movimiento por puntos definidos manualmente. El enemigo interpola entre posiciones segun el tiempo.

```json
{
  "id": "drop_then_diagonal_right",
  "type": "waypoints",
  "point_times": [0.0, 2.8, 4.6],
  "point_x": [80.0, 80.0, 260.0],
  "point_y": [-24.0, 170.0, 340.0]
}
```

| Variable | Tipo | Unidad | Descripcion |
|---|---:|---:|---|
| `point_times` | array float | segundos | Tiempos relativos desde que aparece el enemigo. |
| `point_x` | array float | px | Posicion horizontal en cada punto. |
| `point_y` | array float | px | Posicion vertical en cada punto. |
| `x_relative_to_spawn` | bool | true/false | Si es `true`, `point_x` se interpreta como offset horizontal respecto al `x` del spawn. |

Los tres arrays deben tener la misma cantidad de elementos.

### Importante

En un movimiento `waypoints`, los valores `x` e `y` del spawn pueden quedar ignorados en la practica, porque el patron define posiciones absolutas en `point_x` y `point_y`.

Ejemplo:

```json
{
  "time": 10.0,
  "enemy": "enemy_turret_pod",
  "x": 200,
  "y": -24,
  "movement": "drop_then_diagonal_left"
}
```

Si `drop_then_diagonal_left` tiene:

```json
"point_x": [150.0, 150.0, 80.0]
```

el enemigo usara esos puntos, aunque el spawn diga `x: 200`.

### Waypoints Relativos Al Spawn En X

Si el patron incluye:

```json
"x_relative_to_spawn": true
```

entonces `point_x` deja de ser una coordenada absoluta y pasa a ser un offset respecto al `x` configurado en el stage.

Ejemplo:

```json
{
  "id": "mech_spider_zigzag_right_left",
  "type": "waypoints",
  "x_relative_to_spawn": true,
  "point_times": [0.0, 1.0, 2.0, 3.0, 4.0, 5.0],
  "point_x": [0.0, 0.0, 100.0, 100.0, 0.0, 0.0],
  "point_y": [-20.0, 100.0, 100.0, 200.0, 200.0, 360.0]
}
```

Si el enemigo aparece en el stage con:

```json
"x": 70
```

el movimiento usara estas posiciones horizontales reales:

```txt
70, 70, 170, 170, 70, 70
```

Esto permite reutilizar el mismo movimiento desde distintos puntos de aparicion.

## Tipo `approach_hold_retreat`

Movimiento para enemigos que se acercan hacia la nave, se detienen a una distancia de radio configurable, disparan desde ese lugar y luego se retiran.

Cuando el enemigo llega a `hold_radius`, su posicion de espera queda bloqueada. Es decir: no sigue persiguiendo ni corrigiendo su posicion si la nave se mueve despues. Durante la espera puede seguir mirando visualmente hacia la nave si el enemigo tiene rotacion habilitada desde codigo.

```json
{
  "id": "green_rotor_drone_approach_hold_retreat",
  "type": "approach_hold_retreat",
  "stop_condition": "radius",
  "hold_radius": 90.0,
  "approach_speed": 38.0,
  "hold_seconds": 6.0,
  "retreat_speed": 96.0,
  "retreat_direction_x": 0.0,
  "retreat_direction_y": -1.0
}
```

| Variable | Tipo | Unidad | Descripcion |
|---|---:|---:|---|
| `stop_condition` | string | modo | Condicion de parada. `radius` detiene por distancia radial. `same_y` detiene cuando el enemigo llega a la misma altura Y que la nave. Si se omite, usa `radius`. |
| `stop_offset_y` | float | px | Offset vertical aplicado a `same_y`. `40.0` detiene al enemigo cuando su `y` coincide con `y de la nave + 40 px`. |
| `lock_target_on_start` | bool | true/false | Si es `true`, captura la posicion de la nave al aparecer el enemigo y usa esa referencia fija durante la aproximacion. Evita que el enemigo siga a la nave mientras entra. |
| `hold_radius` | float | px | Distancia a la que el enemigo fija su punto de espera respecto a la nave. |
| `approach_speed` | float | px/s | Velocidad con la que se acerca hacia la nave. |
| `hold_seconds` | float | segundos | Tiempo que se mantiene a distancia antes de retirarse. |
| `retreat_speed` | float | px/s | Velocidad de retirada. Normalmente mayor que `approach_speed`. |
| `retreat_direction_x` | float | direccion | Componente horizontal de la direccion de retirada. |
| `retreat_direction_y` | float | direccion | Componente vertical de la direccion de retirada. `-1.0` significa hacia arriba. |
| `approach_curve_amplitude` | float | px | Desplazamiento lateral maximo de la curva durante la aproximacion. `0.0` desactiva la curva. |
| `approach_curve_direction` | float | direccion | Signo de la curva. `1.0` curva hacia un lado, `-1.0` hacia el lado contrario. |
| `approach_ease_out` | bool | true/false | Si es `true`, el enemigo desacelera suavemente antes de fijar su posicion de espera. |

### Disparo

Los enemigos con este movimiento no disparan durante la aproximacion. Comienzan a disparar cuando ya estan a `hold_radius` de la nave y dejan de disparar cuando empieza la retirada.

### Ajustar La Entrada Curva Del Drone

Para una llegada mas pronunciada, sube:

```json
"approach_curve_amplitude": 24.0
```

Para invertir el lado de la curva:

```json
"approach_curve_direction": -1.0
```

Para que llegue sin desacelerar:

```json
"approach_ease_out": false
```

### Variante Por Misma Altura En Y

Usa esta variante cuando el enemigo debe entrar desde arriba y detenerse al quedar a la misma altura vertical que la nave, sin usar distancia radial:

```json
{
  "id": "green_rotor_drone_approach_y_hold_retreat",
  "type": "approach_hold_retreat",
  "stop_condition": "same_y",
  "stop_offset_y": 40.0,
  "lock_target_on_start": true,
  "approach_speed": 50.0,
  "hold_seconds": 6.0,
  "retreat_speed": 96.0,
  "retreat_direction_x": 0.0,
  "retreat_direction_y": -1.0,
  "approach_curve_amplitude": 18.0,
  "approach_curve_direction": -1.0,
  "approach_ease_out": true
}
```

Con `same_y`, el enemigo mantiene el `x` desde el que aparece en el stage y fija su punto de espera cuando su `y` coincide con la `y` de la nave mas `stop_offset_y`. Si `lock_target_on_start` es `true`, esa `y` de referencia se toma al aparecer el enemigo y ya no cambia durante la entrada. Luego mira a la nave, espera `hold_seconds` y se retira.

## Coordenadas Del Juego

La pantalla logica es:

```txt
240x320 px
```

Referencia rapida:

```txt
x = 0     borde izquierdo
x = 120   centro
x = 240   borde derecho

y = 0     borde superior
y = 320   borde inferior
```

Es comun spawnear enemigos fuera de pantalla:

```json
"y": -24
```

Esto hace que entren desde arriba.

## Ajustes Comunes

### Hacer Que Un Enemigo Baje Mas Rapido

Subir `velocity_y`:

```json
"velocity_y": 80.0
```

### Hacer Que Un Enemigo Baje Mas Lento

Bajar `velocity_y`:

```json
"velocity_y": 12.0
```

### Aumentar El Bamboleo

Subir `amplitude_x`:

```json
"amplitude_x": 64.0
```

### Hacer Que Bambolee Mas Rapido

Subir `frequency`:

```json
"frequency": 0.75
```

### Empezar Un Movimiento Diagonal Antes

En `waypoints`, reduce el tiempo del punto donde empieza la diagonal:

```json
"point_times": [0.0, 2.0, 4.0]
```

Si antes era:

```json
"point_times": [0.0, 2.8, 4.6]
```

la diagonal comienza antes.

## Reglas Pixel Perfect

- Las posiciones internas pueden ser float.
- El render debe redondearse a pixeles enteros.
- Evitar movimientos demasiado lentos si generan cambios subpixel muy largos, porque pueden sentirse como pasos irregulares visuales.
- Para elementos que deben permanecer juntos, usar exactamente la misma velocidad y el mismo tiempo de aparicion.
- Si un enemigo debe ir pegado a una roca flotante, su `velocity_y` debe coincidir con el `speed_y` de esa roca.

## Notas De Balance

- `velocity_y` afecta cuanto tiempo permanece un enemigo en pantalla.
- `amplitude_x` puede sacar enemigos fuera del area jugable si es demasiado alto.
- `frequency` alta puede hacer movimientos dificiles de leer.
- `waypoints` permite coreografias precisas, pero es mas facil desalinear el spawn si se olvidan los puntos absolutos.
