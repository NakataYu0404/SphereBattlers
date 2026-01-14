# Expected Demo Output

When the application runs, you should see:

## Window Layout (600x720)

```
┌─────────────────────────────────────────────────────┐
│                                                     │
│          Boomerang VS Spear                         │ <- Title text (centered, ~20px from top)
│                                                     │
│                                                     │
│      ┌─────────────────────────────────┐           │
│      │                                 │           │
│      │  🎯 (rotating boomerang)       │           │ <- Yellow boomerang (left)
│      │              ⚔ (rotating spear)│           │ <- Cyan spear (right)
│      │                                 │           │
│      │                                 │           │
│      │                                 │           │
│      │      ⚪ 92     ⚪ 91            │           │ <- Bouncing circles with numbers
│      │    (yellow)  (cyan)             │           │    (moving and bouncing)
│      │                                 │           │
│      │                                 │           │
│      │                                 │           │
│      └─────────────────────────────────┘           │ <- 360x360 black frame box
│                                                     │
│      Throw Damage: 13    Damage/Length: 3.5        │ <- Stats text (yellow left, cyan right)
│                                                     │
└─────────────────────────────────────────────────────┘
```

## Visual Elements

### Background
- Entire window filled with beige color (#F5F5DC)

### Frame Box
- Position: (120, 140) to (480, 500)
- Size: 360x360 pixels
- Color: Black outline (2px thick), transparent fill

### Bouncing Circles
1. **Yellow Circle (#92)**
   - Radius: 28px
   - Color: Yellow (#FFFF00)
   - Number "92" in black, centered on circle
   - Initial position: left side, near bottom
   - Velocity: (2.5, -3.0) px/frame
   
2. **Cyan Circle (#91)**
   - Radius: 28px
   - Color: Cyan (#00FFFF)
   - Number "91" in black, centered on circle
   - Initial position: right side, near bottom
   - Velocity: (-2.0, -2.5) px/frame

Both circles bounce off the walls of the frame box with perfect reflection.

### Rotating Weapons

1. **Boomerang (Yellow)**
   - Position: Near top-left of frame (~80px from left edge, ~40px from top)
   - Rendered as two curved arms forming a V shape
   - Continuous rotation at 0.05 radians/frame
   - Line thickness: 3px
   - Color: Yellow

2. **Spear (Cyan)**
   - Position: Near top-right of frame (~80px from right edge, ~40px from top)
   - Rendered as shaft (line) with triangular tip
   - Continuous rotation at 0.05 radians/frame
   - Line thickness: 3px
   - Color: Cyan

### Text Elements

1. **Title**: "Boomerang VS Spear"
   - Position: Centered horizontally, 20px from top
   - Color: Black

2. **Left Stat**: "Throw Damage: 13"
   - Position: Aligned with left edge of frame, ~20px below frame
   - Color: Yellow

3. **Right Stat**: "Damage/Length: 3.5"
   - Position: Aligned with right edge of frame, ~20px below frame
   - Color: Cyan

## Animation

- Everything runs continuously without user input
- Circles bounce smoothly with velocity reversal on wall collision
- Weapons rotate continuously and smoothly
- Frame rate managed by DxLib's default timing
- Press ESC to exit

## Technical Notes

- No external assets required (all primitives)
- No 3D rendering
- No Effekseer effects
- No ResourceManager or complex scene system
- Single main.cpp file (~200 lines)
- Uses only DxLib drawing primitives:
  - `DrawBox()` - frame and background
  - `DrawCircleAA()` - circles
  - `DrawLineAA()` - weapon components
  - `DrawTriangleAA()` - spear tip
  - `DrawString()` - text rendering
