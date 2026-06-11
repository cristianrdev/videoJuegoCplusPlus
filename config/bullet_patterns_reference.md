# Diccionario de configuracion: bullet_patterns.json

Este documento describe los parametros disponibles en `config/bullet_patterns.json`.

El archivo define patrones de disparo para enemigos. Cada entrada dentro de `patterns` representa un patron reutilizable que luego se referencia desde `config/stage_01_enemies.json` mediante el campo `pattern`.

Ejemplo:

```json
{
  "enemy": "enemy_mecha_butterfly",
  "pattern": "enemy_mecha_butterfly_rotating_clock"
}
```

## Estructura general

```json
{
  "patterns": [
    {
      "id": "enemy_turret_spread_3",
      "type": "spread",
      "fire_interval": 0.9,
      "bullet_speed": 120.0,
      "aimed": true,
      "angle_offsets": [0.0],
      "bullet": "enemy_small_red_bullet"
    }
  ]
}
```

## Campos comunes

| Campo | Tipo | Obligatorio | Descripcion |
|---|---:|---:|---|
| `id` | string | Si | Identificador unico del patron. Es el valor que se usa desde los JSON de stage. |
| `type` | string | No | Tipo de patron. Si no se define, se asume `spread`. |
| `fire_interval` | number | Si | Tiempo en segundos entre disparos. Menor valor significa mayor cadencia. |
| `bullet` | string | No | ID del proyectil definido en `config/projectiles.json`. Si no se define, se usa proyectil rectangular por defecto. |
| `laser` | string | No | ID del laser definido en `config/projectiles.json`. Se usa en patrones `continuous_laser`. |
| `bullet_speed` | number | No | Velocidad del proyectil en pixeles logicos por segundo. Valor por defecto: `60.0`. |
| `aimed` | bool | No | Si es `true`, el angulo base apunta hacia la posicion actual del jugador al disparar. |
| `angle_offsets` | number[] | No | Lista de offsets angulares en grados respecto al angulo base. Se usa en patrones `spread`. |
| `bullet_lifetime_seconds` | number | No | Vida maxima de las balas generadas por este patron. `0` o ausente significa vida por salida de pantalla. |
| `bullet_flicker_seconds` | number | No | Tiempo previo a desaparecer en que la bala parpadea. Requiere `bullet_lifetime_seconds`. |
| `clear_bullets_on_owner_destroyed` | bool | No | Si es `true`, al morir el enemigo se eliminan las balas que ese enemigo disparo. |

## Tipos de patron

### `none`

Patron sin disparo real. Se usa para enemigos que no deben disparar.

```json
{
  "id": "none",
  "type": "none",
  "fire_interval": 9999.0
}
```

Nota: en la practica se usa un `fire_interval` muy alto para evitar disparos.

### `spread`

Dispara una o varias balas usando `angle_offsets`.

Si `aimed` es `false`, el angulo base es hacia abajo. Si `aimed` es `true`, el angulo base apunta hacia el jugador en el momento del disparo.

```json
{
  "id": "enemy_turret_spread_3",
  "type": "spread",
  "fire_interval": 0.9,
  "bullet_speed": 120.0,
  "aimed": true,
  "angle_offsets": [-12.0, 0.0, 12.0],
  "bullet": "enemy_small_red_bullet"
}
```

Campos relevantes:

| Campo | Descripcion |
|---|---|
| `bullet_speed` | Velocidad de cada bala. |
| `aimed` | Apunta el centro del disparo hacia el jugador. |
| `angle_offsets` | Desviaciones en grados. Con `[0.0]` dispara una bala directa. |

### `rotating_stream`

Dispara varios streams alrededor del enemigo. En cada disparo, todo el conjunto rota usando `angle_step`.

```json
{
  "id": "enemy_interceptor_wave_5",
  "type": "rotating_stream",
  "fire_interval": 1.90,
  "bullet_speed": 64.0,
  "streams": 5,
  "angle_step": 17.0
}
```

Campos relevantes:

| Campo | Descripcion |
|---|---|
| `streams` | Cantidad de direcciones disparadas a la vez, repartidas en 360 grados. |
| `angle_step` | Grados que rota el patron completo despues de cada disparo. |
| `bullet_speed` | Velocidad de cada bala. |

### `radial_burst`

Crea una explosion radial de balas. Puede tener varios anillos concentricos con velocidades diferentes.

```json
{
  "id": "enemy_mecha_butterfly_concentric_flower",
  "type": "radial_burst",
  "fire_interval": 2.50,
  "bullet": "enemy_orb_purple",
  "rings": 6,
  "bullets_per_ring": 15,
  "speed_start": 20.0,
  "speed_step": 10.0,
  "rotation_per_shot": 18.0,
  "angular_velocity": 20.0,
  "bullet_lifetime_seconds": 5.0,
  "bullet_flicker_seconds": 1.0,
  "clear_bullets_on_owner_destroyed": true
}
```

Campos relevantes:

| Campo | Descripcion |
|---|---|
| `rings` | Cantidad de anillos radiales. |
| `bullets_per_ring` | Cantidad de balas por anillo. |
| `speed_start` | Velocidad del primer anillo. |
| `speed_step` | Incremento de velocidad por cada anillo adicional. |
| `rotation_per_shot` | Grados que rota el burst completo en cada nuevo disparo. |
| `angular_velocity` | Grados por segundo que giran las balas mientras se expanden. Si es `0` o falta, viajan rectas. |

### `rotating_clock`

Patron tipo aguja de reloj. Cada disparo usa el angulo actual, luego el angulo avanza una cantidad fija de grados.

Los proyectiles ya disparados mantienen su trayectoria original. El resultado es una barrida circular continua.

```json
{
  "id": "enemy_mecha_butterfly_rotating_clock",
  "type": "rotating_clock",
  "fire_interval": 0.12,
  "bullet_speed": 72.0,
  "angle_step": 30.0,
  "rotation_direction": "clockwise",
  "bullets_per_burst": 1,
  "burst_angle_spacing": 0.0,
  "bullet": "enemy_orb_purple",
  "bullet_lifetime_seconds": 5.0,
  "bullet_flicker_seconds": 1.0,
  "clear_bullets_on_owner_destroyed": true
}
```

Campos relevantes:

| Campo | Descripcion |
|---|---|
| `angle_step` | Grados que avanza la aguja despues de cada disparo. Ejemplo: `15.0`, `30.0`, `45.0`. |
| `rotation_direction` | Sentido de rotacion. Valores utiles: `clockwise`, `counterclockwise`, `ccw`, `antihorario`. |
| `bullets_per_burst` | Cantidad de balas disparadas en cada tick del reloj. |
| `burst_angle_spacing` | Separacion angular entre balas dentro de la misma rafaga. Si `bullets_per_burst` es `1`, no tiene efecto visible. |
| `bullet_speed` | Velocidad de cada bala. |

Ejemplo con rafaga triple:

```json
{
  "bullets_per_burst": 3,
  "burst_angle_spacing": 8.0
}
```

Si la aguja apunta a `45` grados, las balas saldrian aproximadamente en `45`, `53` y `61` grados.

### `spiral_cluster`

Genera racimos de balas con forma de espiral o rueda. Cada racimo nace desde la posicion del enemigo, mantiene un centro propio que baja lentamente por la pantalla y puede expandirse o conservar un radio fijo mientras gira.

Este patron no persigue al jugador. Una vez creado el racimo, sus balas siguen la trayectoria relativa definida por la espiral.

```json
{
  "id": "enemy_mecha_butterfly_spiral_cluster",
  "type": "spiral_cluster",
  "fire_interval": 1.60,
  "bullet": "enemy_orb_purple",
  "spiral_radius": 42.0,
  "angle_step": 24.0,
  "spiral_descent_speed": 24.0,
  "bullets_per_spiral": 36,
  "bullet_speed": 0.0,
  "rotation_direction": "clockwise",
  "spiral_radius_expansion": 0.0,
  "fixed_spiral_radius": true,
  "spiral_open_seconds": 0.45,
  "cluster_duration_seconds": 6.0,
  "spiral_arms": 1,
  "angular_velocity": 32.0,
  "rotation_per_shot": 18.0,
  "bullet_flicker_seconds": 1.0,
  "clear_bullets_on_owner_destroyed": true
}
```

Campos relevantes:

| Campo | Descripcion |
|---|---|
| `spiral_radius` | Radio inicial maximo del racimo. Las balas se reparten desde el centro hasta este radio. |
| `angle_step` | Avance angular entre una bala y la siguiente dentro de cada brazo. |
| `spiral_descent_speed` | Velocidad a la que baja el centro de la espiral en pixeles logicos por segundo. |
| `bullets_per_spiral` | Cantidad total aproximada de balas del racimo. Si hay varios brazos, se reparte entre ellos. |
| `bullet_speed` | Velocidad radial individual de las balas. Usar `0.0` si se quiere una rueda de radio fijo. |
| `spiral_radius_expansion` | Expansion radial adicional en pixeles por segundo. |
| `fixed_spiral_radius` | Si es `true`, todas las balas nacen en el radio maximo y forman una rueda en vez de una espiral que nace desde el centro. |
| `spiral_open_seconds` | Tiempo en segundos que tarda el racimo en abrirse desde radio `0` hasta `spiral_radius`. Despues conserva su radio si `spiral_radius_expansion` es `0`. |
| `cluster_duration_seconds` | Tiempo de vida del racimo. Despues desaparece; si hay flicker configurado, parpadea antes. |
| `spiral_arms` | Cantidad de brazos de la espiral. Valores utiles: `1`, `2`, `3`, `4`. |
| `angular_velocity` | Velocidad de giro del racimo en grados por segundo. |
| `rotation_per_shot` | Rotacion inicial extra aplicada a cada nuevo racimo. Evita que todos los racimos nazcan iguales. |
| `rotation_direction` | Sentido de giro: `clockwise`, `counterclockwise`, `ccw` o `antihorario`. |

### `aimed_random_cone`

Genera una nube de balas tipo shotgun apuntando hacia la posicion actual del jugador. La direccion se calcula solo al disparar; despues cada bala conserva su trayectoria original y no persigue al jugador.

```json
{
  "id": "enemy_mecha_butterfly_aimed_random_cone",
  "type": "aimed_random_cone",
  "fire_interval": 0.95,
  "bullet": "enemy_orb_purple",
  "bullet_speed": 76.0,
  "bullets_per_burst": 22,
  "spread_angle": 46.0,
  "random_spread": 12.0,
  "random_speed": 24.0,
  "spawn_jitter": 8.0,
  "bullet_lifetime_seconds": 5.0,
  "bullet_flicker_seconds": 1.0,
  "clear_bullets_on_owner_destroyed": true
}
```

Campos relevantes:

| Campo | Descripcion |
|---|---|
| `fire_interval` | Tiempo entre rafagas. Equivale al inverso de fireRate. Por ejemplo, `0.75` es una rafaga cada 0.75 segundos. |
| `bullet_speed` | Velocidad base de cada bala del cono. |
| `bullets_per_burst` | Cantidad de balas simultaneas por disparo. |
| `spread_angle` | Apertura total del cono en grados. Cada bala elige una direccion aleatoria dentro de este rango. |
| `random_spread` | Desviacion aleatoria adicional, en grados, aplicada a cada bala para romper la regularidad. |
| `random_speed` | Variacion aleatoria de velocidad respecto a `bullet_speed`. Esto hace que la nube se desordene al avanzar. |
| `spawn_jitter` | Distancia aleatoria inicial desde el origen del disparo. Ayuda a que el racimo no nazca como una linea perfecta. |

### `continuous_laser`

Dispara un laser continuo durante una duracion determinada.

```json
{
  "id": "enemy_robot_fish_mouth_laser",
  "type": "continuous_laser",
  "fire_interval": 1.0,
  "laser_duration": 0.42,
  "laser": "enemy_robot_fish_laser"
}
```

Campos relevantes:

| Campo | Descripcion |
|---|---|
| `laser` | ID del laser en `config/projectiles.json`. |
| `laser_duration` | Duracion del laser en segundos. |
| `fire_interval` | Frecuencia con la que intenta volver a disparar. |

## Convenciones de angulos

Los angulos se expresan en grados.

En este proyecto:

| Angulo | Direccion aproximada |
|---:|---|
| `0` | Hacia abajo |
| `90` | Derecha |
| `180` | Hacia arriba |
| `270` | Izquierda |

Esto calza con los disparos enemigos, que normalmente nacen desde arriba y avanzan hacia la zona del jugador.

## Relacion con projectiles.json

El campo `bullet` o `laser` referencia un ID definido en `config/projectiles.json`.

Ejemplo:

```json
{
  "bullet": "enemy_orb_purple"
}
```

Ese proyectil define textura, dano, tipo visual, rotacion visual y otros parametros propios del proyectil.

## Relacion con stage_01_enemies.json

Cada enemigo del stage elige que patron usara:

```json
{
  "time": 82.0,
  "enemy": "enemy_mecha_butterfly",
  "x": 120,
  "y": -40,
  "pattern": "enemy_mecha_butterfly_rotating_clock",
  "movement": "mecha_butterfly_center_hold"
}
```

Esto permite reutilizar el mismo enemigo con distintos patrones sin crear una clase nueva.

## Guia rapida de ajuste

| Objetivo | Parametro a tocar |
|---|---|
| Disparar mas seguido | Bajar `fire_interval`. |
| Balas mas rapidas | Subir `bullet_speed`, `speed_start` o `speed_step`. |
| Mas densidad radial | Subir `bullets_per_ring`, `rings` o `streams`. |
| Flor que gira entre disparos | Subir `rotation_per_shot`. |
| Flor que gira mientras se abre | Usar/subir `angular_velocity`. |
| Reloj mas rapido en rotacion | Subir `angle_step` o bajar `fire_interval`. |
| Cambiar sentido del reloj | Cambiar `rotation_direction`. |
| Hacer que las balas desaparezcan por tiempo | Usar `bullet_lifetime_seconds`. |
| Avisar antes de desaparecer | Usar `bullet_flicker_seconds`. |
| Limpiar balas al destruir al enemigo | Usar `clear_bullets_on_owner_destroyed: true`. |
