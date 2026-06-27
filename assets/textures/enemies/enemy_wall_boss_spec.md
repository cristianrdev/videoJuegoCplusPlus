# enemy_wall_boss

Sprite temporal generado con flujo de IA para un enemigo tipo pared.

- Ruta: `assets/textures/enemies/enemy_wall_boss.png`
- Concept aprobado pendiente: `assets/textures/enemies/concepts/enemy_wall_boss_concept.png`
- Tamaño final: `240x32 px`
- Estilo: pixel art arcade 90s, pared metálica futurista.
- Paleta: grises de acero, detalles celestes y cristales azules brillantes.
- Uso en juego: entidad `WallBoss`.

## Puntos débiles

La textura incluye tres cristales visuales. La posición jugable se define en `config/wall_bosses.json` con offsets respecto al centro de la pared:

- Izquierda: `offset_x = -84`
- Centro: `offset_x = 0`
- Derecha: `offset_x = 84`

Cada cristal tiene 30 HP y al destruirse abre un hueco permanente de 40 px.

## Pixel Perfect

La textura final ya está a resolución lógica nativa. No se debe escalar dentro del juego.
