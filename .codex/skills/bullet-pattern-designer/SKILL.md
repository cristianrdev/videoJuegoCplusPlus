---

name: bullet-pattern-designer
description: Utilizar esta skill cuando se diseñen, implementen o refactoricen patrones de balas para un shooter vertical 2D en C++ con SFML, inspirado en juegos arcade como Batsugun, Truxton, Raiden, DoDonPachi o shmups tipo danmaku. Aplica para patrones como disparo recto, abanico, espiral, ráfaga circular, disparo dirigido al jugador, oleadas, patrones de jefe y fases de boss.
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# Bullet Pattern Designer para Shmup SFML

## Objetivo

Diseñar patrones de balas escalables, reutilizables y configurables para un shooter vertical arcade en C++ con SFML.

El estilo objetivo está inspirado en juegos como:

* Batsugun
* Truxton
* Raiden
* DoDonPachi
* Shmups arcade verticales de los años 90
* Bullet hell / danmaku moderado

El sistema debe permitir crear muchos patrones sin escribir una clase nueva para cada enemigo.

---

## Principio principal

Los patrones de balas NO deben estar quemados dentro de `Enemy`, `Boss` o `PlayState`.

Correcto:

```cpp
enemy.setBulletPattern("spread_5");
bulletPatternSystem.update(enemy, dt);
```

Incorrecto:

```cpp
void Enemy::update(float dt) {
    if (timer > 0.5f) {
        createBullet(x, y, 90);
        createBullet(x, y, 75);
        createBullet(x, y, 105);
    }
}
```

---

## Responsabilidades

### BulletPatternSystem

Debe encargarse de:

* Leer la configuración del patrón.
* Controlar cooldowns de disparo.
* Calcular ángulos.
* Crear proyectiles.
* Mantener estado temporal del patrón.
* Aplicar variaciones como espiral, abanico, ráfagas y disparo dirigido.
* Limpiar o reiniciar patrones cuando una entidad muere.

No debe encargarse de:

* Colisiones.
* Puntaje.
* Movimiento general de enemigos.
* Renderizado directo.
* Lógica completa del boss.

---

## Estructura recomendada

```txt
src/
├─ systems/
│  ├─ BulletPatternSystem.hpp
│  └─ BulletPatternSystem.cpp
│
├─ entities/
│  ├─ Bullet.hpp
│  ├─ Bullet.cpp
│  ├─ Enemy.hpp
│  ├─ Enemy.cpp
│  ├─ Boss.hpp
│  └─ Boss.cpp
│
├─ data/
│  ├─ BulletPatternDefinition.hpp
│  └─ BulletPatternRuntime.hpp
│
└─ utils/
   ├─ MathUtils.hpp
   └─ AngleUtils.hpp
```

Assets/configuración:

```txt
assets/
└─ data/
   └─ patterns/
      └─ bullet_patterns.json
```

---

## Modelo mental

Usar esta separación:

```txt
Enemy/Boss indica QUÉ patrón usa.
BulletPatternSystem decide CÓMO se dispara.
BulletFactory crea la bala.
BulletSystem mueve las balas.
CollisionSystem detecta impactos.
```

---

## Formato recomendado de patrón

Los patrones deben venir desde JSON siempre que sea razonable.

Ejemplo base:

```json
{
  "id": "spread_5",
  "type": "spread",
  "cooldown": 0.5,
  "bulletType": "red_small",
  "speed": 120,
  "bullets": 5,
  "centerAngle": 90,
  "spreadAngle": 40
}
```

Campos comunes:

```txt
id              Identificador único del patrón
type            Tipo de patrón
cooldown        Tiempo entre disparos
bulletType      Tipo visual/lógico de bala
speed           Velocidad base
bullets         Cantidad de balas por disparo
angle           Ángulo fijo
centerAngle     Ángulo central del patrón
spreadAngle     Apertura del abanico
angleOffset     Separación angular adicional
angleStep       Separación entre balas
duration        Duración del patrón
delay           Retardo antes de iniciar
repeat          Cantidad
```
