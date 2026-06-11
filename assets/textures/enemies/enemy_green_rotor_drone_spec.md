# enemy_green_rotor_drone

## Uso

Enemigo drone pequeno de color verde con una sola helice. Pensado para entrar hacia la nave, mantener distancia y disparar desde un radio configurado.

## Sprite

```txt
Archivo destino: assets/textures/enemies/enemy_green_rotor_drone.png
Tamano: 26x26 px
Frames: 1
Fondo: transparente
Estilo: pixel art arcade 90s, vista cenital
```

## Diseno

- Drone mecanico compacto.
- Color principal verde oscuro.
- Luces o brillos verde lima.
- Una sola helice visible, centrada o ligeramente superior.
- Cuerpo metalico pequeno con contorno oscuro de 1 px.
- Silueta clara y legible en resolucion logica 240x320.

## Gameplay

```txt
Enemy ID previsto: enemy_green_rotor_drone
Patron de balas: enemy_rock_turret_fast_small
Movimiento: green_rotor_drone_approach_hold_retreat
```

## Hitbox sugerida

```txt
hitbox_shape: square
hitbox_width: 18.0
hitbox_height: 18.0
hitbox_offset_x: 0.0
hitbox_offset_y: 0.0
```

## Prompt base

```txt
Create a strict pixel art sprite for a vertical arcade shmup enemy drone, top-down view, 26x26 pixels, transparent background. Single-frame sprite. Small mechanical green drone with one visible central rotor/propeller, compact metal body, dark green armor, lime highlights, small gray mechanical details, subtle black/dark outline, readable silhouette at 26x26, inspired by 90s arcade shooters like Batsugun/Raiden/Truxton. No blur, no antialiasing, no background, no text, no shadow outside sprite. The sprite should face downward/neutral top-down and have a clear center point for shooting.
```

