# Boomerang vs Spear - 2D DxLib Demo

A simple 2D autoplay demonstration using DxLib showing bouncing circles with rotating weapon graphics.

## Features

- **2D Animation**: Two bouncing circles (yellow #92 and cyan #91) with physics-based wall collision
- **Rotating Weapons**: Animated boomerang and spear graphics that continuously rotate
- **Autoplay**: No player input required - everything animates automatically
- **Simple Graphics**: All visuals rendered using DxLib primitive drawing functions (DrawCircleAA, DrawLineAA, DrawTriangleAA, etc.)

## Technical Details

- **Window Size**: 600x720 pixels, windowed mode
- **Background**: Beige color (#F5F5DC)
- **Frame Box**: 360x360 pixel play area centered on screen
- **Libraries**: DxLib only (no Effekseer, no external assets)
- **Controls**: ESC key to exit

## Building

Requires:
- Visual Studio 2022 or compatible
- DxLib installed and DXLIB_DIR environment variable set
- Windows platform

Open `BaseProject.sln` in Visual Studio and build the project.

## Code Structure

The entire demo is contained in a single file: `Src/main.cpp`

Key components:
- Circle physics with wall collision detection
- Rotation-based weapon rendering (boomerang and spear)
- Simple text rendering for stats and title
 
