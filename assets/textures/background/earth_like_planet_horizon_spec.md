# earth_like_planet_horizon

Elemento de fondo para sugerir entrada a un planeta tipo Tierra.

## Asset

| Archivo | Tamano | Uso |
| --- | ---: | --- |
| `textures/background/earth_like_planet_horizon.png` | 240x320 px | Planeta grande entrando desde arriba |

## Stage

- Tileset: `earth_like_planet`.
- Spawn: `config/stage_01_background_elements.json`.
- Tiempo: `60.0`.
- Posicion inicial: `x=0`, `y=-300`.
- Velocidad vertical: `4.5`.
- Sin colision ni dano.

## Intencion visual

- El planeta asoma lentamente desde la parte superior.
- Debe verse redondo, no ovalado ni achatado.
- El planeta fue ajustado desde una version mas grande sin deformar la circunferencia.
- Debe conservar margen inferior transparente para que la curvatura baja no quede cortada.
- La densidad de pixel art debe mantenerse consistente con la grilla logica 240x320.
