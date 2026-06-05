---
name: sfml-architecture
description: Use this skill when creating or refactoring SFML game architecture, project structure, entities, systems, states, and asset management.
---

# Arquitectura Shmup SFML

## Objetivo

Crear shooters verticales escalables inspirados en Batsugun, Raiden y DoDonPachi.

## Reglas

- Utilizar StageDirector para la progresión del nivel.
- Utilizar EventQueue para desacoplar sistemas.
- Utilizar EnemySpawner para generar enemigos.
- Utilizar BulletPatternSystem para todos los disparos.