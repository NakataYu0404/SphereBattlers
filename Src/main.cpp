#include <DxLib.h>
#include <math.h>

// Constants
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 720;
const int FRAME_LEFT = 120;
const int FRAME_TOP = 140;
const int FRAME_RIGHT = 480;
const int FRAME_BOTTOM = 500;
const int FRAME_WIDTH = FRAME_RIGHT - FRAME_LEFT;
const int FRAME_HEIGHT = FRAME_BOTTOM - FRAME_TOP;
const int CIRCLE_RADIUS = 28;
const unsigned int BEIGE_COLOR = 0xF5F5DC;
const unsigned int BLACK_COLOR = 0x000000;
const unsigned int YELLOW_COLOR = 0xFFFF00;
const unsigned int CYAN_COLOR = 0x00FFFF;
const float PI = 3.14159265f;

// Circle structure for bouncing circles
struct Circle {
    float x, y;
    float vx, vy;
    unsigned int color;
    int number;
};

// Draw a simple boomerang shape
void DrawBoomerang(int cx, int cy, float angle, unsigned int color) {
    // Boomerang as two connected curved lines forming a V shape
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    // First arm
    float x1 = cx + (-20.0f * cos_a - (-5.0f) * sin_a);
    float y1 = cy + (-20.0f * sin_a + (-5.0f) * cos_a);
    float x2 = cx + (0.0f * cos_a - 0.0f * sin_a);
    float y2 = cy + (0.0f * sin_a + 0.0f * cos_a);
    float x3 = cx + (15.0f * cos_a - 10.0f * sin_a);
    float y3 = cy + (15.0f * sin_a + 10.0f * cos_a);
    
    // Second arm
    float x4 = cx + (-20.0f * cos_a - 5.0f * sin_a);
    float y4 = cy + (-20.0f * sin_a + 5.0f * cos_a);
    float x5 = cx + (15.0f * cos_a - (-10.0f) * sin_a);
    float y5 = cy + (15.0f * sin_a + (-10.0f) * cos_a);
    
    DrawLineAA(x1, y1, x2, y2, color, 3.0f);
    DrawLineAA(x2, y2, x3, y3, color, 3.0f);
    DrawLineAA(x4, y4, x2, y2, color, 3.0f);
    DrawLineAA(x2, y2, x5, y5, color, 3.0f);
}

// Draw a simple spear shape
void DrawSpear(int cx, int cy, float angle, unsigned int color) {
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    // Spear shaft
    float x1 = cx + (-25.0f * cos_a);
    float y1 = cy + (-25.0f * sin_a);
    float x2 = cx + (15.0f * cos_a);
    float y2 = cy + (15.0f * sin_a);
    DrawLineAA(x1, y1, x2, y2, color, 3.0f);
    
    // Spear tip (triangle)
    float tipX = cx + (25.0f * cos_a);
    float tipY = cy + (25.0f * sin_a);
    float baseX = cx + (15.0f * cos_a);
    float baseY = cy + (15.0f * sin_a);
    
    float leftX = baseX + (-8.0f * sin_a);
    float leftY = baseY + (8.0f * cos_a);
    float rightX = baseX + (8.0f * sin_a);
    float rightY = baseY + (-8.0f * cos_a);
    
    DrawTriangleAA(tipX, tipY, leftX, leftY, rightX, rightY, color, TRUE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Set window mode and size
    ChangeWindowMode(TRUE);
    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
    SetWindowText("Boomerang VS Spear");
    
    // Run even when window is inactive
    SetAlwaysRunFlag(TRUE);
    
    // Initialize DxLib
    if (DxLib_Init() == -1) {
        return -1;
    }
    
    // Set drawing screen
    SetDrawScreen(DX_SCREEN_BACK);
    
    // Initialize two circles
    Circle circles[2];
    
    // Yellow circle with number 92
    circles[0].x = FRAME_LEFT + FRAME_WIDTH * 0.3f;
    circles[0].y = FRAME_TOP + FRAME_HEIGHT * 0.7f;
    circles[0].vx = 2.5f;
    circles[0].vy = -3.0f;
    circles[0].color = YELLOW_COLOR;
    circles[0].number = 92;
    
    // Cyan circle with number 91
    circles[1].x = FRAME_LEFT + FRAME_WIDTH * 0.7f;
    circles[1].y = FRAME_TOP + FRAME_HEIGHT * 0.7f;
    circles[1].vx = -2.0f;
    circles[1].vy = -2.5f;
    circles[1].color = CYAN_COLOR;
    circles[1].number = 91;
    
    float boomerangAngle = 0.0f;
    float spearAngle = 0.0f;
    
    // Pre-calculate text widths (done once, not every frame)
    const char* titleText = "Boomerang VS Spear";
    int titleWidth = GetDrawStringWidth(titleText, strlen(titleText));
    const char* damageText = "Throw Damage: 13";
    const char* damagePerLengthText = "Damage/Length: 3.5";
    int damagePerLengthWidth = GetDrawStringWidth(damagePerLengthText, strlen(damagePerLengthText));
    
    // Main loop
    while (ProcessMessage() == 0) {
        // Check ESC key to quit
        if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
            break;
        }
        
        // Clear screen with beige color
        ClearDrawScreen();
        DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BEIGE_COLOR, TRUE);
        
        // Draw frame box
        DrawBox(FRAME_LEFT, FRAME_TOP, FRAME_RIGHT, FRAME_BOTTOM, BLACK_COLOR, FALSE);
        DrawBox(FRAME_LEFT + 1, FRAME_TOP + 1, FRAME_RIGHT - 1, FRAME_BOTTOM - 1, BLACK_COLOR, FALSE);
        
        // Update and draw circles
        for (int i = 0; i < 2; i++) {
            // Update position
            circles[i].x += circles[i].vx;
            circles[i].y += circles[i].vy;
            
            // Wall collision detection
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
            DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, BLACK_COLOR, FALSE);
            
            // Draw number on circle
            char numStr[16];  // Increased buffer size for safety
            int numLen = sprintf_s(numStr, "%d", circles[i].number);
            int textWidth = GetDrawStringWidth(numStr, numLen);
            DrawString((int)circles[i].x - textWidth / 2, (int)circles[i].y - 8, numStr, BLACK_COLOR);
        }
        
        // Draw rotating boomerang (yellow) near top left
        boomerangAngle += 0.05f;
        DrawBoomerang(FRAME_LEFT + 80, FRAME_TOP + 40, boomerangAngle, YELLOW_COLOR);
        
        // Draw rotating spear (cyan) near top right
        spearAngle += 0.05f;
        DrawSpear(FRAME_RIGHT - 80, FRAME_TOP + 40, spearAngle, CYAN_COLOR);
        
        // Draw title text at top
        DrawString((SCREEN_WIDTH - titleWidth) / 2, 20, titleText, BLACK_COLOR);
        
        // Draw stats text at bottom
        DrawString(FRAME_LEFT, FRAME_BOTTOM + 20, damageText, YELLOW_COLOR);
        DrawString(FRAME_RIGHT - damagePerLengthWidth, FRAME_BOTTOM + 20, damagePerLengthText, CYAN_COLOR);
        
        // Flip screen
        ScreenFlip();
    }
    
    // End DxLib
    DxLib_End();
    
    return 0;
}
