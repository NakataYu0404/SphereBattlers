# Build Instructions

## 2D Boomerang vs Spear Demo

This is a simple 2D DxLib demo showing an autoplay animation with bouncing circles and rotating weapons.

### Requirements

- Windows OS
- Visual Studio 2019 or later
- DxLib library installed and `DXLIB_DIR` environment variable set to the DxLib directory path

### Building

1. Open `BaseProject.sln` in Visual Studio
2. Select your desired configuration (Debug/Release) and platform (Win32/x64)
3. Build the solution (F7 or Build > Build Solution)

### Running

- The executable will be created in the Debug or Release folder
- Run the executable to see the demo
- Press ESC to exit the application

### Features

- **Window**: 600x720 pixels, windowed mode
- **Background**: Beige color
- **Frame**: Black square frame in the center (360x360 pixels)
- **Yellow Circle**: Number 92, bounces inside the frame with rotating boomerang
- **Cyan Circle**: Number 91, bounces inside the frame with rotating spear
- **Weapon Possession**: Each player holds its weapon at a defined offset that rotates with the player
  - Weapons orbit around players as they spin
  - Yellow boomerang orbits at 40 pixels offset, following player's rotation
  - Cyan spear orbits at 45 pixels offset, following player's rotation
  - Weapons rotate solely with their owning player's angle (no independent spin)
- **Collision Detection**:
  - Player-player: Circle-circle collision with elastic bounce
  - Weapon-player: Line segment vs circle collision detection
  - Hit feedback: Players flash red when hit by opponent's weapon
- **Stats Display**: 
  - "Throw Damage: 13" (Yellow, bottom left)
  - "Damage/Length: 3.5" (Cyan, bottom right)
- **Auto-play**: Animation runs continuously without user input
- **ESC to Exit**: Press ESC key to close the application

### Notes

- No external assets are required; all graphics are drawn using DxLib primitive drawing functions
- The application uses `SetAlwaysRunFlag(TRUE)` to ensure smooth animation even when the window is not in focus
