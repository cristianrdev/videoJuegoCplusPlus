# videoJuegoCplusPlus

Shooter vertical arcade en C++17 inspirado en Batsugun/Darius.

## Dependencias

- Windows 10/11
- CMake `3.20` o superior
- MSYS2 instalado en `C:\msys64`
- Toolchain UCRT64 de MinGW:
  - `gcc/g++`
  - `ninja`
  - `sfml`

El proyecto usa:

- C++17
- SFML 3
- CMake
- Ninja

## Instalar MSYS2 y paquetes

Instala MSYS2 desde:

```txt
https://www.msys2.org/
```

Abre una terminal de MSYS2 UCRT64 o PowerShell con `pacman` disponible y ejecuta:

```powershell
pacman -Syu
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-sfml
```

Si no tienes CMake instalado, puedes instalarlo desde:

```txt
https://cmake.org/download/
```

Descarga el instalador Windows x64 `.msi` y marca la opcion para agregar CMake al `PATH`.

## Compilar en PowerShell

Desde la carpeta del proyecto:

```powershell
cd C:\Users\luxo2\git\videoJuegoCplusPlus
```

Agrega MSYS2 UCRT64 al `PATH` de la sesion:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;$env:Path"
```

Configura CMake:

```powershell
cmake -S . -B build -G "Ninja" `
  -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64 `
  -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe
```

Compila:

```powershell
cmake --build build
```

Ejecuta:

```powershell
.\build\videoJuegoCplusPlus.exe
```

## Recompilar despues de cambios

Para cambios en C++, assets o JSON:

```powershell
cmake --build build
```

El `CMakeLists.txt` copia automaticamente `assets/` y `config/` al directorio del ejecutable despues de compilar.

## Controles

- Flechas / movimiento configurado: mover nave
- `Space` o `Z`: disparar
- `P`: pausar
- `Esc`: salir
- `1`: resolucion logica real `240x320`
- `2`: escala entera pixel-perfect

Tambien hay soporte basico para joystick estilo Xbox/PlayStation:

- Stick/D-pad: movimiento
- Botones principales: disparo
- Start/Options/Menu: pausa

## Problemas comunes

Si PowerShell dice que `cmake` no existe:

```txt
cmake : El termino 'cmake' no se reconoce...
```

Instala CMake y asegúrate de agregarlo al `PATH`.

Si CMake no encuentra SFML:

```txt
Could not find SFML
```

Verifica que SFML este instalado en MSYS2 UCRT64:

```powershell
pacman -S --needed mingw-w64-ucrt-x86_64-sfml
```

Y configura usando:

```powershell
-DCMAKE_PREFIX_PATH=C:/msys64/ucrt64
```

Si Ninja no existe:

```powershell
pacman -S --needed mingw-w64-ucrt-x86_64-ninja
```
