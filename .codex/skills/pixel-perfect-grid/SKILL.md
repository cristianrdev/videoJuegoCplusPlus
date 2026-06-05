---
name: pixel-perfect-grid
description: Usar esta skill cuando se creen, integren o rendericen sprites pixel art en el juego y sea importante evitar subpixeles, bamboleo visual, jitter, blur o desalineacion entre frames y la rejilla logica de pixeles.
---

# Pixel Perfect Grid

## Objetivo

Garantizar que todos los sprites pixel art coincidan con la rejilla de pixeles del juego y no generen efectos de bamboleo al moverse, cambiar de frame o escalarse.

El juego usa resolucion logica:

```txt
240x320 px
```

Todo sprite debe verse estable dentro de esa resolucion logica antes de escalarse a la ventana.

---

## Regla principal

Los sprites deben dibujarse siempre alineados a pixeles enteros dentro del render target logico.

No renderizar sprites pixel art en coordenadas fraccionarias.

Correcto:

```cpp
sprite.setPosition({
    std::round(position.x),
    std::round(position.y)
});
```

Incorrecto:

```cpp
sprite.setPosition(position); // si position contiene valores como 120.37f
```

El movimiento interno puede usar `float` y `deltaTime`, pero la posicion de render debe ajustarse a la rejilla de pixeles.

---

## Movimiento

Usar `deltaTime` para calcular movimiento:

```cpp
position += velocity * deltaTime.asSeconds();
```

Pero antes de dibujar:

```cpp
auto drawPosition = sf::Vector2f{
    std::round(position.x),
    std::round(position.y)
};
```

Nunca sacrificar `deltaTime`; solo separar posicion logica de posicion de render.

---

## Frames De Animacion

Todos los frames de un mismo sprite sheet deben compartir:

- mismo tamano exacto por frame
- mismo origen/pivote
- mismo punto visual central
- misma ubicacion de la hitbox
- misma direccion de la nariz/cabina si es una nave
- mismo margen relativo dentro de cada frame

Si un frame tiene el centro visual desplazado, el sprite parecera tiritar aunque la posicion de render sea entera.

---

## Sprite Sheets

Para sprite sheets:

```txt
frame_width  = texture_width / frame_count
frame_height = texture_height
```

Cada frame debe ocupar una celda exacta.

Ejemplo:

```txt
player_ship_sheet.png
Tamano total: 96x32
Frames: 3
Tamano por frame: 32x32
```

Los rectangulos de textura deben comenzar en coordenadas enteras:

```cpp
sprite.setTextureRect({
    {frameIndex * frameWidth, 0},
    {frameWidth, frameHeight}
});
```

---

## Escalado

Escalar solo por factores enteros cuando se renderice pixel art:

```txt
1x: 240x320
2x: 480x640
3x: 720x960
4x: 960x1280
```

Para una ventana 1080p, usar escala entera maxima que mantenga aspect ratio.

No usar escalado fraccional si se quiere evitar blur o vibracion visual.

---

## Texturas

Para SFML:

```cpp
texture.setSmooth(false);
renderTexture.setSmooth(false);
```

No activar smoothing en sprites pixel art.

---

## Origen Y Hitbox

El origen visual del sprite debe ser estable.

Para una nave de `32x32`:

```cpp
sprite.setOrigin({16.f, 16.f});
```

La hitbox debe estar separada del sprite visual y mantenerse centrada:

```txt
hitbox sugerida jugador: 6x6 px
```

Cambiar de frame no debe mover la hitbox.

---

## Banking Sin Bamboleo

Para naves con frames:

```txt
center
bank_left
bank_right
```

No rotar el sprite completo.

Mantener:

- punta alineada al mismo pixel X en los tres frames
- cabina vertical
- fuselaje central estable
- origen identico
- hitbox en el mismo punto

El banking debe lograrse moviendo alas, pods, brillos y sombras dentro de la celda, no desplazando el sprite completo.

---

## Checklist

Antes de aceptar un sprite o animacion:

```txt
Los frames tienen tamano exacto?
El sprite sheet divide en celdas enteras?
El origen/pivote es igual en todos los frames?
La punta/cabina se mantiene alineada si corresponde?
La hitbox no cambia al cambiar frame?
La posicion de render se redondea a pixeles enteros?
Las texturas tienen smooth=false?
El escalado final es entero?
No hay antialiasing ni pixeles semitransparentes indeseados?
```

---

## Resultado Esperado

Cuando Codex use esta skill, debe evitar jitter, bamboleo, blur y desalineacion visual en sprites pixel art.

La regla mental es:

```txt
movimiento logico suave, render en rejilla entera
```
