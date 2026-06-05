---
name: pixel-art-generation-pipeline
description: Utilizar esta skill cuando se solicite crear sprites, fondos, tiles, bosses o cualquier asset visual para el juego.
---

# Generación de Arte Pixel

## Regla principal

Codex NO debe generar sprites mediante:

- Python PIL
- generador.py
- Canvas
- Dibujos ASCII
- Algoritmos de dibujo procedural

Incorrecto:

```python
Image.new(...)
draw.rectangle(...)


Flujo obligatorio

Cuando se solicite un sprite:

Determinar tamaño exacto.
Determinar cantidad de frames.
Determinar paleta.
Generar prompt para IA de imagen.
Generar especificación técnica.
Generar ruta destino del asset.

NO generar el PNG directamente.


Herramientas preferidas

Prioridad:

GPT Image
PixelLab
Aseprite + PixelAI
Pixie.haus


## Prohibición

Si el usuario solicita sprites, fondos, enemigos, bosses o assets gráficos:

NO crear:

- generador.py
- sprite_generator.py
- PIL scripts
- OpenCV scripts
- Procedural drawing code

En su lugar:

Generar prompts para herramientas de IA especializadas en pixel art.