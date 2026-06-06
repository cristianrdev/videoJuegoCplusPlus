# Enemy Rock Turret

- Asset final: `assets/textures/enemies/enemy_rock_turret.png`
- Fuente IA: `assets/textures/enemies/enemy_rock_turret_source.png`
- Tipo: torreta montada sobre roca flotante
- Vista: cenital/top-down
- Tamano final: `32x40 px`
- Paleta: gunmetal, gris metalico, acentos rojo/rust para encajar con `floating_red_rocks`
- Render:
  - no escalar textura en runtime
  - `setSmooth(false)`
  - posicion redondeada a pixel entero
- Enemy id: `enemy_rock_turret`
- Movimiento recomendado: `floating_rock_turret_drop`
- Patron recomendado: `enemy_rock_turret_fast_small`

## Configuracion

Para colocarla sobre una roca, usar en `config/stage_01_enemies.json`:

```json
{
  "time": 2.0,
  "enemy": "enemy_rock_turret",
  "x": 58,
  "y": -80,
  "pattern": "enemy_rock_turret_fast_small",
  "movement": "floating_rock_turret_drop"
}
```

La velocidad vertical de `floating_rock_turret_drop` es `18.0`, igual que una de las rocas flotantes base.
