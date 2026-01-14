#include <DxLib.h>
#include <windows.h>
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
const unsigned int COLOR_HIT_FEEDBACK = 0xFF6666;
const float HIT_FEEDBACK_DURATION = 10.0f;
const int INITIAL_HP = 100;
const float COLLISION_COOLDOWN_MS = 400.0f;  // 400ms cooldown between hits
const float MS_PER_FRAME = 16.67f;  // Assuming ~60 FPS

// Text positioning constants
const int TITLE_X_OFFSET = -100;
const int TITLE_Y_POSITION = 30;
const int STATS_Y_OFFSET = 30;
const int STATS_RIGHT_X_OFFSET = -150;
const int CIRCLE_NUMBER_X_OFFSET = -10;
const int CIRCLE_NUMBER_Y_OFFSET = -8;

// Weapon type enum
enum WeaponType {
    WEAPON_BOOMERANG,
    WEAPON_SPEAR
};

// Weapon definition (attached to player)
struct WeaponDef {
    WeaponType type;
    float offsetX;      // Local offset X (in player's local space)
    float offsetY;      // Local offset Y
    float length;       // Length for collision detection
    unsigned int color;
};

// Cooldown tracking structure
struct CollisionCooldown {
    float playerPlayerCooldown;
    float weapon0ToPlayer1Cooldown;
    float weapon1ToPlayer0Cooldown;
    float weaponWeaponCooldown;
    bool playerPlayerSeparated;
    bool weapon0Player1Separated;
    bool weapon1Player0Separated;
    bool weaponWeaponSeparated;
};

// Circle/Player struct
struct Circle {
    float x, y;         // Position
    float vx, vy;       // Velocity
    float angle;        // Player rotation angle
    float angularVel;   // Angular velocity
    int number;
    unsigned int color;
    WeaponDef weapon;
    float hitTimer;     // Timer for hit feedback (counts down)
    int hp;             // Current HP
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

// Helper function: Calculate weapon world position from player position and rotation
void GetWeaponWorldPosition(const Circle& player, float& outX, float& outY) {
    float cos_player = cosf(player.angle);
    float sin_player = sinf(player.angle);
    
    outX = player.x + (cos_player * player.weapon.offsetX - sin_player * player.weapon.offsetY);
    outY = player.y + (sin_player * player.weapon.offsetX + cos_player * player.weapon.offsetY);
}

// Helper function: Calculate distance from point to line segment
float DistancePointToSegment(float px, float py, float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float lengthSq = dx * dx + dy * dy;
    
    if (lengthSq < 0.001f) {
        // Degenerate segment
        dx = px - x1;
        dy = py - y1;
        return sqrtf(dx * dx + dy * dy);
    }
    
    // Project point onto line
    float t = ((px - x1) * dx + (py - y1) * dy) / lengthSq;
    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
    
    float closestX = x1 + t * dx;
    float closestY = y1 + t * dy;
    
    dx = px - closestX;
    dy = py - closestY;
    return sqrtf(dx * dx + dy * dy);
}

// Check if weapon hits a player (line segment vs circle)
bool CheckWeaponHit(const Circle& attacker, const Circle& target) {
    // Get weapon world position
    float weaponWorldX, weaponWorldY;
    GetWeaponWorldPosition(attacker, weaponWorldX, weaponWorldY);
    
    // Weapon line segment endpoints (using player's angle)
    float weaponAngle = attacker.angle;
    float cos_w = cosf(weaponAngle);
    float sin_w = sinf(weaponAngle);
    float halfLen = attacker.weapon.length * 0.5f;
    
    float x1 = weaponWorldX - cos_w * halfLen;
    float y1 = weaponWorldY - sin_w * halfLen;
    float x2 = weaponWorldX + cos_w * halfLen;
    float y2 = weaponWorldY + sin_w * halfLen;
    
    // Check distance from target center to weapon segment
    float dist = DistancePointToSegment(target.x, target.y, x1, y1, x2, y2);
    return dist < CIRCLE_RADIUS;
}

// Check if two weapons collide (line segment vs line segment)
bool CheckWeaponWeaponCollision(const Circle& player1, const Circle& player2) {
    // Get both weapon world positions
    float weapon1X, weapon1Y, weapon2X, weapon2Y;
    GetWeaponWorldPosition(player1, weapon1X, weapon1Y);
    GetWeaponWorldPosition(player2, weapon2X, weapon2Y);
    
    // Weapon 1 line segment
    float cos_w1 = cosf(player1.angle);
    float sin_w1 = sinf(player1.angle);
    float halfLen1 = player1.weapon.length * 0.5f;
    float x1a = weapon1X - cos_w1 * halfLen1;
    float y1a = weapon1Y - sin_w1 * halfLen1;
    float x1b = weapon1X + cos_w1 * halfLen1;
    float y1b = weapon1Y + sin_w1 * halfLen1;
    
    // Weapon 2 line segment
    float cos_w2 = cosf(player2.angle);
    float sin_w2 = sinf(player2.angle);
    float halfLen2 = player2.weapon.length * 0.5f;
    float x2a = weapon2X - cos_w2 * halfLen2;
    float y2a = weapon2Y - sin_w2 * halfLen2;
    float x2b = weapon2X + cos_w2 * halfLen2;
    float y2b = weapon2Y + sin_w2 * halfLen2;
    
    // Check distance between line segments (approximate with endpoints)
    float dist1 = DistancePointToSegment(x1a, y1a, x2a, y2a, x2b, y2b);
    float dist2 = DistancePointToSegment(x1b, y1b, x2a, y2a, x2b, y2b);
    float dist3 = DistancePointToSegment(x2a, y2a, x1a, y1a, x1b, y1b);
    float dist4 = DistancePointToSegment(x2b, y2b, x1a, y1a, x1b, y1b);
    
    float minDist = (dist1 < dist2) ? dist1 : dist2;
    minDist = (minDist < dist3) ? minDist : dist3;
    minDist = (minDist < dist4) ? minDist : dist4;
    
    // Weapons collide if segments are very close
    return minDist < 5.0f;
}

// Check and handle circle-circle collision
void HandlePlayerCollision(Circle& c1, Circle& c2, CollisionCooldown& cooldown) {
    float dx = c2.x - c1.x;
    float dy = c2.y - c1.y;
    float distSq = dx * dx + dy * dy;
    float minDist = CIRCLE_RADIUS * 2.0f;
    
    bool colliding = distSq < minDist * minDist && distSq > 0.001f;
    
    if (!colliding) {
        // Not colliding - mark as separated
        cooldown.playerPlayerSeparated = true;
    }
    
    if (colliding) {
        // Collision detected - elastic bounce
        float dist = sqrtf(distSq);
        
        // Normalize
        dx /= dist;
        dy /= dist;
        
        // Separate circles
        float overlap = minDist - dist;
        c1.x -= dx * overlap * 0.5f;
        c1.y -= dy * overlap * 0.5f;
        c2.x += dx * overlap * 0.5f;
        c2.y += dy * overlap * 0.5f;
        
        // Elastic bounce (swap velocity components along collision normal)
        float v1n = c1.vx * dx + c1.vy * dy;
        float v2n = c2.vx * dx + c2.vy * dy;
        
        c1.vx += (v2n - v1n) * dx;
        c1.vy += (v2n - v1n) * dy;
        c2.vx += (v1n - v2n) * dx;
        c2.vy += (v1n - v2n) * dy;
        
        // Apply damage if cooldown expired and separated since last hit
        if (cooldown.playerPlayerCooldown <= 0.0f && cooldown.playerPlayerSeparated) {
            // Both players take damage from collision
            if (c1.hp > 0) {
                c1.hp -= 5;  // Small collision damage
                if (c1.hp < 0) c1.hp = 0;
                c1.hitTimer = HIT_FEEDBACK_DURATION;
            }
            if (c2.hp > 0) {
                c2.hp -= 5;  // Small collision damage
                if (c2.hp < 0) c2.hp = 0;
                c2.hitTimer = HIT_FEEDBACK_DURATION;
            }
            cooldown.playerPlayerCooldown = COLLISION_COOLDOWN_MS;
            cooldown.playerPlayerSeparated = false;
        }
    }
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
    
    // Yellow circle (92) with boomerang
    circles[0].x = FRAME_LEFT + FRAME_WIDTH * 0.3f;
    circles[0].y = FRAME_BOTTOM - 80.0f;
    circles[0].vx = 2.5f;
    circles[0].vy = -3.0f;
    circles[0].angle = 0.0f;
    circles[0].angularVel = 0.03f;  // Rotation speed
    circles[0].number = 92;
    circles[0].color = COLOR_YELLOW;
    circles[0].hitTimer = 0.0f;
    circles[0].hp = INITIAL_HP;
    circles[0].weapon.type = WEAPON_BOOMERANG;
    circles[0].weapon.offsetX = 40.0f;  // Offset in local space
    circles[0].weapon.offsetY = 0.0f;
    circles[0].weapon.length = 25.0f;   // For collision
    circles[0].weapon.color = COLOR_YELLOW;
    
    // Cyan circle (91) with spear
    circles[1].x = FRAME_LEFT + FRAME_WIDTH * 0.7f;
    circles[1].y = FRAME_BOTTOM - 80.0f;
    circles[1].vx = -2.0f;
    circles[1].vy = -3.5f;
    circles[1].angle = 0.0f;
    circles[1].angularVel = -0.025f;  // Rotation speed (opposite direction)
    circles[1].number = 91;
    circles[1].color = COLOR_CYAN;
    circles[1].hitTimer = 0.0f;
    circles[1].hp = INITIAL_HP;
    circles[1].weapon.type = WEAPON_SPEAR;
    circles[1].weapon.offsetX = 45.0f;  // Offset in local space
    circles[1].weapon.offsetY = 0.0f;
    circles[1].weapon.length = 30.0f;   // For collision
    circles[1].weapon.color = COLOR_CYAN;
    
    // Initialize cooldown tracking
    CollisionCooldown cooldown;
    cooldown.playerPlayerCooldown = 0.0f;
    cooldown.weapon0ToPlayer1Cooldown = 0.0f;
    cooldown.weapon1ToPlayer0Cooldown = 0.0f;
    cooldown.weaponWeaponCooldown = 0.0f;
    cooldown.playerPlayerSeparated = true;
    cooldown.weapon0Player1Separated = true;
    cooldown.weapon1Player0Separated = true;
    cooldown.weaponWeaponSeparated = true;
    
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
            // Skip update if K.O. (HP = 0)
            if (circles[i].hp <= 0) {
                continue;
            }
            
            // Update position
            circles[i].x += circles[i].vx;
            circles[i].y += circles[i].vy;
            
            // Update rotation
            circles[i].angle += circles[i].angularVel;
            
            // Update hit timer
            if (circles[i].hitTimer > 0.0f) {
                circles[i].hitTimer -= 1.0f;
            }
            
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
        }
        
        // Update cooldown timers
        if (cooldown.playerPlayerCooldown > 0.0f) {
            cooldown.playerPlayerCooldown -= MS_PER_FRAME;
        }
        if (cooldown.weapon0ToPlayer1Cooldown > 0.0f) {
            cooldown.weapon0ToPlayer1Cooldown -= MS_PER_FRAME;
        }
        if (cooldown.weapon1ToPlayer0Cooldown > 0.0f) {
            cooldown.weapon1ToPlayer0Cooldown -= MS_PER_FRAME;
        }
        if (cooldown.weaponWeaponCooldown > 0.0f) {
            cooldown.weaponWeaponCooldown -= MS_PER_FRAME;
        }
        
        // Handle player-player collision (only if both alive)
        if (circles[0].hp > 0 && circles[1].hp > 0) {
            HandlePlayerCollision(circles[0], circles[1], cooldown);
        }
        
        // Check weapon hits
        if (circles[0].hp > 0 && circles[1].hp > 0) {
            // Check if weapon 0 hits player 1
            bool weapon0HitsPlayer1 = CheckWeaponHit(circles[0], circles[1]);
            
            if (!weapon0HitsPlayer1) {
                cooldown.weapon0Player1Separated = true;
            }
            
            if (weapon0HitsPlayer1 && cooldown.weapon0ToPlayer1Cooldown <= 0.0f && cooldown.weapon0Player1Separated) {
                circles[1].hp -= 13;  // Weapon damage
                if (circles[1].hp < 0) circles[1].hp = 0;
                circles[1].hitTimer = HIT_FEEDBACK_DURATION;
                cooldown.weapon0ToPlayer1Cooldown = COLLISION_COOLDOWN_MS;
                cooldown.weapon0Player1Separated = false;
            }
            
            // Check if weapon 1 hits player 0
            bool weapon1HitsPlayer0 = CheckWeaponHit(circles[1], circles[0]);
            
            if (!weapon1HitsPlayer0) {
                cooldown.weapon1Player0Separated = true;
            }
            
            if (weapon1HitsPlayer0 && cooldown.weapon1ToPlayer0Cooldown <= 0.0f && cooldown.weapon1Player0Separated) {
                circles[0].hp -= 13;  // Weapon damage
                if (circles[0].hp < 0) circles[0].hp = 0;
                circles[0].hitTimer = HIT_FEEDBACK_DURATION;
                cooldown.weapon1ToPlayer0Cooldown = COLLISION_COOLDOWN_MS;
                cooldown.weapon1Player0Separated = false;
            }
            
            // Check weapon-weapon collision
            bool weaponsCollide = CheckWeaponWeaponCollision(circles[0], circles[1]);
            
            if (!weaponsCollide) {
                cooldown.weaponWeaponSeparated = true;
            }
            
            if (weaponsCollide && cooldown.weaponWeaponCooldown <= 0.0f && cooldown.weaponWeaponSeparated) {
                // Both weapons take "damage" (visual feedback)
                circles[0].hitTimer = HIT_FEEDBACK_DURATION;
                circles[1].hitTimer = HIT_FEEDBACK_DURATION;
                cooldown.weaponWeaponCooldown = COLLISION_COOLDOWN_MS;
                cooldown.weaponWeaponSeparated = false;
            }
        }
        
        // Draw circles with hit feedback
        for (int i = 0; i < 2; i++) {
            // Skip drawing if K.O. (HP = 0)
            if (circles[i].hp <= 0) {
                continue;
            }
            
            unsigned int drawColor = circles[i].color;
            
            // Apply red flash if hit
            if (circles[i].hitTimer > 0.0f) {
                drawColor = COLOR_HIT_FEEDBACK;
            }
            
            // Draw circle with outline
            DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, drawColor, TRUE);
            DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, COLOR_BLACK, FALSE);
            
            // Draw HP as the number on circle
            DrawFormatString((int)(circles[i].x + CIRCLE_NUMBER_X_OFFSET), (int)(circles[i].y + CIRCLE_NUMBER_Y_OFFSET), COLOR_BLACK, "%d", circles[i].hp);
        }
        
        // Draw weapons at player positions with rotated offsets
        for (int i = 0; i < 2; i++) {
            // Skip drawing weapon if player is K.O.
            if (circles[i].hp <= 0) {
                continue;
            }
            
            // Calculate weapon world position
            float weaponWorldX, weaponWorldY;
            GetWeaponWorldPosition(circles[i], weaponWorldX, weaponWorldY);
            
            // Draw weapon using player's angle
            if (circles[i].weapon.type == WEAPON_BOOMERANG) {
                DrawBoomerang(weaponWorldX, weaponWorldY, circles[i].angle, circles[i].weapon.color);
            } else {
                DrawSpear(weaponWorldX, weaponWorldY, circles[i].angle, circles[i].weapon.color);
            }
        }
        
        // Draw title text at top
        DrawFormatString(SCREEN_WIDTH / 2 + TITLE_X_OFFSET, TITLE_Y_POSITION, COLOR_BLACK, "Boomerang VS Spear");
        
        // Draw stats at bottom
        DrawFormatString(FRAME_LEFT, FRAME_BOTTOM + STATS_Y_OFFSET, COLOR_YELLOW, "Throw Damage: 13");
        DrawFormatString(FRAME_RIGHT + STATS_RIGHT_X_OFFSET, FRAME_BOTTOM + STATS_Y_OFFSET, COLOR_CYAN, "Damage/Length: 3.5");
        
        ScreenFlip();
    }
    
    DxLib_End();
    return 0;
}
