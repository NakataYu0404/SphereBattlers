#include <DxLib.h>
#include <cmath>

// Constants
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 720;
const int FRAME_LEFT = 120;
const int FRAME_TOP = 140;
const int FRAME_RIGHT = 480;
const int FRAME_BOTTOM = 500;
const int FRAME_WIDTH = FRAME_RIGHT - FRAME_LEFT;
const int FRAME_HEIGHT = FRAME_BOTTOM - FRAME_TOP;
const float CIRCLE_RADIUS = 28.0f;
const unsigned int COLOR_BEIGE = 0xF5F5DC;
const unsigned int COLOR_YELLOW = 0xFFFF00;
const unsigned int COLOR_CYAN = 0x00FFFF;
const unsigned int COLOR_BLACK = 0x000000;

// Circle struct
struct Circle {
    float x, y;
    float vx, vy;
    int number;
    unsigned int color;
};

// Weapon struct
struct Weapon {
    float x, y;
    float angle;
    float rotationSpeed;
};

// Function to draw a boomerang
void DrawBoomerang(float x, float y, float angle, unsigned int color) {
    // Boomerang as two connected arcs forming a V shape
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    // First arm
    float x1 = x + cos_a * 20.0f - sin_a * 5.0f;
    float y1 = y + sin_a * 20.0f + cos_a * 5.0f;
    float x2 = x + cos_a * 5.0f - sin_a * 2.0f;
    float y2 = y + sin_a * 5.0f + cos_a * 2.0f;
    
    // Second arm
    float x3 = x + cos_a * 5.0f + sin_a * 2.0f;
    float y3 = y + sin_a * 5.0f - cos_a * 2.0f;
    float x4 = x + cos_a * 20.0f + sin_a * 5.0f;
    float y4 = y + sin_a * 20.0f - cos_a * 5.0f;
    
    // Draw lines for boomerang arms
    DrawLineAA(x, y, x1, y1, color, 3.0f);
    DrawLineAA(x, y, x4, y4, color, 3.0f);
    DrawLineAA(x1, y1, x2, y2, color, 3.0f);
    DrawLineAA(x4, y4, x3, y3, color, 3.0f);
}

// Function to draw a spear
void DrawSpear(float x, float y, float angle, unsigned int color) {
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    // Spear shaft
    float shaftLen = 30.0f;
    float sx = x - cos_a * shaftLen * 0.5f;
    float sy = y - sin_a * shaftLen * 0.5f;
    float ex = x + cos_a * shaftLen * 0.5f;
    float ey = y + sin_a * shaftLen * 0.5f;
    DrawLineAA(sx, sy, ex, ey, color, 3.0f);
    
    // Spear head (triangle)
    float tipX = ex + cos_a * 10.0f;
    float tipY = ey + sin_a * 10.0f;
    float baseX1 = ex - sin_a * 4.0f;
    float baseY1 = ey + cos_a * 4.0f;
    float baseX2 = ex + sin_a * 4.0f;
    float baseY2 = ey - cos_a * 4.0f;
    
    DrawTriangleAA(tipX, tipY, baseX1, baseY1, baseX2, baseY2, color, TRUE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Window settings
    SetWindowText("Boomerang VS Spear");
    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
    ChangeWindowMode(TRUE);
    SetAlwaysRunFlag(TRUE);
    
    if (DxLib_Init() == -1) {
        return -1;
    }
    
    SetDrawScreen(DX_SCREEN_BACK);
    
    // Initialize circles
    Circle circles[2];
    
    // Yellow circle (92)
    circles[0].x = FRAME_LEFT + FRAME_WIDTH * 0.3f;
    circles[0].y = FRAME_BOTTOM - 80.0f;
    circles[0].vx = 2.5f;
    circles[0].vy = -3.0f;
    circles[0].number = 92;
    circles[0].color = COLOR_YELLOW;
    
    // Cyan circle (91)
    circles[1].x = FRAME_LEFT + FRAME_WIDTH * 0.7f;
    circles[1].y = FRAME_BOTTOM - 80.0f;
    circles[1].vx = -2.0f;
    circles[1].vy = -3.5f;
    circles[1].number = 91;
    circles[1].color = COLOR_CYAN;
    
    // Initialize weapons
    Weapon boomerang;
    boomerang.x = FRAME_LEFT + FRAME_WIDTH * 0.3f;
    boomerang.y = FRAME_TOP + 60.0f;
    boomerang.angle = 0.0f;
    boomerang.rotationSpeed = 0.05f;
    
    Weapon spear;
    spear.x = FRAME_LEFT + FRAME_WIDTH * 0.7f;
    spear.y = FRAME_TOP + 60.0f;
    spear.angle = 0.0f;
    spear.rotationSpeed = 0.04f;
    
    // Main loop
    while (ProcessMessage() == 0) {
        // Check for ESC key to exit
        if (CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }
        
        // Clear screen with beige background
        ClearDrawScreen();
        DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BEIGE, TRUE);
        
        // Draw frame box
        DrawBox(FRAME_LEFT, FRAME_TOP, FRAME_RIGHT, FRAME_BOTTOM, COLOR_BLACK, FALSE);
        
        // Update and draw circles
        for (int i = 0; i < 2; i++) {
            // Update position
            circles[i].x += circles[i].vx;
            circles[i].y += circles[i].vy;
            
            // Wall collision
            if (circles[i].x - CIRCLE_RADIUS < FRAME_LEFT) {
                circles[i].x = FRAME_LEFT + CIRCLE_RADIUS;
                circles[i].vx = -circles[i].vx;
            }
            if (circles[i].x + CIRCLE_RADIUS > FRAME_RIGHT) {
                circles[i].x = FRAME_RIGHT - CIRCLE_RADIUS;
                circles[i].vx = -circles[i].vx;
            }
            if (circles[i].y - CIRCLE_RADIUS < FRAME_TOP) {
                circles[i].y = FRAME_TOP + CIRCLE_RADIUS;
                circles[i].vy = -circles[i].vy;
            }
            if (circles[i].y + CIRCLE_RADIUS > FRAME_BOTTOM) {
                circles[i].y = FRAME_BOTTOM - CIRCLE_RADIUS;
                circles[i].vy = -circles[i].vy;
            }
            
            // Draw circle
            DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, circles[i].color, TRUE);
            DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, COLOR_BLACK, FALSE);
            
            // Draw number on circle
            DrawFormatString((int)(circles[i].x - 10), (int)(circles[i].y - 8), COLOR_BLACK, "%d", circles[i].number);
        }
        
        // Update and draw weapons
        boomerang.angle += boomerang.rotationSpeed;
        DrawBoomerang(boomerang.x, boomerang.y, boomerang.angle, COLOR_YELLOW);
        
        spear.angle += spear.rotationSpeed;
        DrawSpear(spear.x, spear.y, spear.angle, COLOR_CYAN);
        
        // Draw title text at top
        DrawFormatString(SCREEN_WIDTH / 2 - 100, 30, COLOR_BLACK, "Boomerang VS Spear");
        
        // Draw stats at bottom
        DrawFormatString(FRAME_LEFT, FRAME_BOTTOM + 30, COLOR_YELLOW, "Throw Damage: 13");
        DrawFormatString(FRAME_RIGHT - 150, FRAME_BOTTOM + 30, COLOR_CYAN, "Damage/Length: 3.5");
        
        ScreenFlip();
    }
    
    DxLib_End();
    return 0;
}
