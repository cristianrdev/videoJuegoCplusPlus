---
name: sfml-shmup-architecture
description: Utilizar esta skill cuando se cree, implemente o refactorice la arquitectura de un shooter vertical 2D en C++ con SFML, inspirado en juegos arcade como Batsugun, Truxton, Raiden o DoDonPachi. Aplica para estructura de carpetas, PlayState, StageDirector, EventQueue, EnemySpawner, BulletPatternSystem, BossSystem, oleadas, niveles, entidades, sistemas y configuración mediante JSON.
---

# SFML Shmup Architecture

## Objetivo

Construir una arquitectura escalable para un shooter vertical 2D en C++ con SFML.

El juego debe estar inspirado en shmups arcade como:

- Batsugun
- Truxton
- Raiden
- DoDonPachi
- Shooters verticales arcade de los años 90

La arquitectura debe permitir crecer desde un prototipo simple hasta un juego completo con:

- varios niveles
- muchas oleadas
- enemigos configurables
- jefes por fases
- patrones de balas reutilizables
- eventos desacoplados
- assets administrados correctamente

---

## Estructura recomendada

```txt
src/
├─ main.cpp
├─ core/
├─ states/
├─ entities/
├─ systems/
├─ ui/
├─ utils/
└─ data/

Responsabilidades principales
core/

Contiene la infraestructura base del juego.

Ejemplos:

core/
├─ Game.hpp
├─ Game.cpp
├─ AssetManager.hpp
├─ AssetManager.cpp
├─ AudioManager.hpp
├─ AudioManager.cpp
├─ Config.hpp
└─ Config.cpp

Responsabilidades:

Crear ventana SFML.
Ejecutar el game loop.
Controlar deltaTime.
Administrar estados.
Cargar configuración global.
Administrar recursos compartidos.

core no debe contener lógica específica de gameplay.

states/

Contiene las pantallas o modos del juego.

Ejemplos:

states/
├─ State.hpp
├─ MenuState.hpp/.cpp
├─ PlayState.hpp/.cpp
├─ PauseState.hpp/.cpp
└─ GameOverState.hpp/.cpp

Estados típicos:

MenuState
PlayState
PauseState
GameOverState
StageClearState

PlayState coordina el gameplay, pero no debe contener todos los detalles internos de oleadas, disparos o colisiones.

Correcto:

void PlayState::update(float dt) {
    stageDirector.update(dt);
    enemySpawner.update(dt);
    bulletPatternSystem.update(dt);
    bulletSystem.update(dt);
    collisionSystem.update(dt);
    eventQueue.dispatch();
}

Incorrecto:

void PlayState::update(float dt) {
    if (timer > 10.0f) {
        spawnBoss();
    }

    if (timer > 15.0f) {
        createSpiralBullets();
    }
}
entities/

Representa objetos del mundo del juego.

Ejemplos:

entities/
├─ Entity.hpp
├─ Player.hpp/.cpp
├─ Enemy.hpp/.cpp
├─ Bullet.hpp/.cpp
├─ Boss.hpp/.cpp
├─ PowerUp.hpp/.cpp
└─ Explosion.hpp/.cpp

Una entidad puede tener:

posición
velocidad
sprite
vida
estado
hitbox
identificador de patrón
identificador de movimiento

Las entidades deben ser livianas.

No deben:

administrar puntaje global
reproducir audio global directamente
cargar texturas
controlar todo el stage
crear oleadas completas
resolver todas las colisiones
systems/

Contiene lógica que procesa entidades.

Ejemplos:

systems/
├─ StageDirector.hpp/.cpp
├─ EventQueue.hpp/.cpp
├─ EnemySpawner.hpp/.cpp
├─ BulletPatternSystem.hpp/.cpp
├─ BulletSystem.hpp/.cpp
├─ MovementSystem.hpp/.cpp
├─ CollisionSystem.hpp/.cpp
├─ ScoreSystem.hpp/.cpp
├─ EffectSystem.hpp/.cpp
├─ PowerUpSystem.hpp/.cpp
└─ BossSystem.hpp/.cpp

Cada sistema debe tener una responsabilidad clara.

StageDirector

StageDirector orquesta la progresión del nivel.

Responsabilidades:

Mantener el tiempo transcurrido del nivel.
Leer eventos desde JSON.
Gatillar eventos en el momento correcto.
Pedir al EnemySpawner que cree enemigos.
Activar oleadas.
Activar jefes.
Publicar eventos importantes en EventQueue.

No debe crear balas directamente.

Ejemplo de stage:

{
  "stage": "stage_01",
  "events": [
    {
      "time": 2.0,
      "type": "spawn_enemy",
      "enemyType": "small_fighter",
      "x": 80,
      "y": -20,
      "movement": "straight_down",
      "bulletPattern": "aimed_shot"
    },
    {
      "time": 6.0,
      "type": "spawn_wave",
      "wave": "left_curve_wave"
    },
    {
      "time": 35.0,
      "type": "spawn_boss",
      "bossType": "boss_01",
      "x": 120,
      "y": -80
    }
  ]
}
EventQueue

EventQueue desacopla sistemas.

Ejemplos de eventos:

StageStarted
EnemySpawned
EnemyDestroyed
PlayerHit
PlayerDead
BossSpawned
BossPhaseChanged
BossDefeated
PowerUpCollected
StageCleared

Correcto:

eventQueue.publish(EnemyDestroyedEvent{
    enemy.getPosition(),
    enemy.getScoreValue()
});

Luego otros sistemas reaccionan:

EnemyDestroyed
├─ ScoreSystem suma puntos
├─ AudioSystem reproduce explosión
├─ EffectSystem crea partículas
└─ PowerUpSystem evalúa drop

Incorrecto:

enemy.destroy();
score += 100;
sound.play();
createExplosion();
dropPowerUp();
EnemySpawner

EnemySpawner crea enemigos usando datos.

Ejemplo:

{
  "id": "small_fighter",
  "texture": "enemy_small.png",
  "hp": 3,
  "speed": 80,
  "score": 100,
  "hitbox": {
    "width": 16,
    "height": 16
  },
  "defaultMovement": "straight_down",
  "defaultBulletPattern": "aimed_shot"
}

Responsabilidades:

Crear instancias de Enemy.
Asignar textura mediante AssetManager.
Asignar movimiento.
Asignar patrón de balas.
Registrar enemigo en la colección activa.
Publicar evento EnemySpawned si corresponde.
BulletPatternSystem

BulletPatternSystem controla cómo disparan enemigos y jefes.

Debe trabajar con IDs de patrón:

enemy.setBulletPattern("spread_5");

Y leer configuración desde:

assets/data/patterns/bullet_patterns.json

No debe haber patrones de balas quemados dentro de Enemy o Boss.

BulletSystem

BulletSystem procesa balas existentes.

Responsabilidades:

Mover balas.
Aplicar velocidad.
Aplicar trayectoria si corresponde.
Eliminar balas fuera de pantalla.
Marcar balas destruidas.

No debe decidir cuándo un enemigo dispara. Eso pertenece a BulletPatternSystem.

CollisionSystem

CollisionSystem detecta y resuelve colisiones.

Casos típicos:

PlayerBullet vs Enemy
PlayerBullet vs Boss
EnemyBullet vs Player
Enemy vs Player
PowerUp vs Player

Debe publicar eventos cuando corresponda:

EnemyDestroyed
PlayerHit
BossDefeated
PowerUpCollected
BossSystem

BossSystem controla lógica de jefes.

Responsabilidades:

Gestionar fases por vida.
Activar patrones según fase.
Publicar BossPhaseChanged.
Detectar BossDefeated.
Coordinar entrada/salida del boss.

Ejemplo:

{
  "id": "boss_01",
  "texture": "boss_01.png",
  "hp": 1000,
  "score": 10000,
  "phases": [
    {
      "hpBelow": 1000,
      "movement": "boss_intro",
      "bulletPattern": "spread_5"
    },
    {
      "hpBelow": 700,
      "movement": "boss_hover",
      "bulletPattern": "circle_burst"
    },
    {
      "hpBelow": 300,
      "movement": "boss_rage",
      "bulletPattern": "spiral_rage"
    }
  ]
}
WaveSystem o configuración de oleadas

Las oleadas deben ser configurables.

Ejemplo:

{
  "id": "left_curve_wave",
  "enemies": [
    {
      "delay": 0.0,
      "enemyType": "small_fighter",
      "x": 20,
      "y": -20,
      "movement": "curve_right",
      "bulletPattern": "straight_down"
    },
    {
      "delay": 0.4,
      "enemyType": "small_fighter",
      "x": 40,
      "y": -20,
      "movement": "curve_right",
      "bulletPattern": "straight_down"
    },
    {
      "delay": 0.8,
      "enemyType": "small_fighter",
      "x": 60,
      "y": -20,
      "movement": "curve_right",
      "bulletPattern": "straight_down"
    }
  ]
}
Movimiento de enemigos

El movimiento debe ser configurable cuando sea posible.

Tipos recomendados:

straight_down
straight_left
straight_right
curve_left
curve_right
sine_wave
stop_and_shoot
boss_intro
boss_hover
boss_rage

Ejemplo:

{
  "id": "sine_wave",
  "type": "sine",
  "speedY": 70,
  "amplitude": 40,
  "frequency": 2.0
}
Orden recomendado de update

Usar un orden determinista:

void PlayState::update(float dt) {
    stageDirector.update(dt);

    enemySpawner.update(dt);

    inputSystem.update(dt);
    player.update(dt);

    movementSystem.update(dt);

    bossSystem.update(dt);

    bulletPatternSystem.update(dt);

    bulletSystem.update(dt);

    collisionSystem.update(dt);

    eventQueue.dispatch();

    effectSystem.update(dt);

    powerUpSystem.update(dt);

    cleanupDestroyedEntities();
}
Renderizado recomendado

Separar update de render.

void PlayState::render(sf::RenderWindow& window) {
    background.render(window);

    for (auto& enemy : enemies) {
        enemy.render(window);
    }

    for (auto& bullet : bullets) {
        bullet.render(window);
    }

    player.render(window);

    effectSystem.render(window);

    hud.render(window);
}

No modificar gameplay dentro de render.

Resolución lógica recomendada

Para un shooter vertical arcade inspirado en Batsugun:

Resolución lógica: 240x320
Orientación: vertical
Sprite nave: aprox. 32x32 px

El juego puede escalar a ventana moderna manteniendo aspect ratio.

Reglas de diseño

Hacer:

Usar deltaTime.
Mantener entidades simples.
Separar update y render.
Usar AssetManager.
Usar StageDirector para progresión.
Usar EventQueue para comunicación.
Usar JSON para stages, enemies, waves, bosses y patterns.
Usar IDs para referenciar configuraciones.
Mantener PlayState como coordinador.
Mantener sistemas desacoplados.

Evitar:

Cargar texturas dentro de update.
Poner lógica de stage dentro de PlayState.
Poner patrones de balas dentro de Enemy.
Crear una clase nueva por cada patrón simple.
Mezclar render con lógica.
Usar variables globales para estado del juego.
Hacer que Boss conozca directamente ScoreSystem o AudioSystem.
Duplicar patrones similares en vez de parametrizarlos.
Modelo mental de la arquitectura
StageDirector decide CUÁNDO pasa algo.
EnemySpawner decide QUÉ aparece.
MovementSystem decide CÓMO se mueve.
BulletPatternSystem decide CÓMO dispara.
CollisionSystem decide QUÉ impactó.
EventQueue comunica QUÉ ocurrió.
ScoreSystem, AudioSystem y EffectSystem reaccionan.