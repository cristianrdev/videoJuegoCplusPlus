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
