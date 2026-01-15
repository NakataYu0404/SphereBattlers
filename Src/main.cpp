#include <DxLib.h>

// windows.h �� min/max �}�N�����`���� std::min/std::max �ƏՓ˂���̂�h��
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <cmath>
#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <algorithm>
#include "Library/nlohmann/json.hpp"

using json = nlohmann::json;

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
const unsigned int COLOR_HIT_FEEDBACK = 0xFF2222;
const float HIT_FEEDBACK_DURATION = 10.0f;
const int MAX_HP = 100;
const int WEAPON_DAMAGE = 10;
const float HIT_STOP_DURATION = 20.0f;  // ~100ms at 60fps (frames)
const float WEAPON_COLLISION_THRESHOLD = 5.0f;  // Distance threshold for weapon-weapon collision
const float WEAPON_BOUNCE_DAMPING = 0.8f;  // Damping factor for weapon bounce
const float HIT_COOLDOWN_DURATION = 24.0f;  // ~0.4s at 60fps (frames) - cooldown between hits
const float MAP_INPUT_COOLDOWN_DURATION = 10.0f;  // ~0.167s at 60fps (frames) - cooldown after battle return

// Math constants
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
const float INITIAL_WEAPON_ANGLE = -M_PI / 2.0f;  // Initial angle facing upward

// Weapon rendering constants
const float WEAPON_REFERENCE_LENGTH = 30.0f;  // Base reference length for weapon scaling

// Aim phase constants
const float AIM_MIN_SPEED_SCALE = 0.3f;
const float AIM_MAX_SPEED_SCALE = 1.0f;
const float AIM_ARROW_LENGTH = 60.0f;
const float AIM_DEFAULT_SPEED_MAG = 4.0f;  // Default speed magnitude for aiming

// Text positioning constants
const int TITLE_X_OFFSET = -100;
const int TITLE_Y_POSITION = 30;
const int STATS_Y_OFFSET = 30;
const int STATS_RIGHT_X_OFFSET = -150;
const int CIRCLE_NUMBER_X_OFFSET = -10;
const int CIRCLE_NUMBER_Y_OFFSET = -8;
const int KO_TEXT_X_OFFSET = -20;
const int KO_TEXT_Y_OFFSET = -10;

// Map constants
const int MAP_ROWS = 8;
const int MAP_MIN_NODES_PER_ROW = 2;
const int MAP_MAX_NODES_PER_ROW = 4;
const float MAP_NODE_RADIUS = 20.0f;
const float MAP_LEFT = 100.0f;
const float MAP_RIGHT = 500.0f;
const float MAP_TOP = 100.0f;
const float MAP_BOTTOM = 650.0f;
const int NODE_PROB_NORMAL = 60;    // 60% normal nodes (unused after shop removal, now 70%)
const int NODE_PROB_ELITE = 70;     // 10% elite nodes (60-70) (unused after shop removal)
const int NODE_PROB_EVENT = 80;     // 10% event nodes (70-80) (unused after shop removal)
const int NODE_PROB_REST = 90;      // 10% rest nodes (80-90) (unused after shop removal)
const int NODE_PROB_SHOP = 90;      // 10% shop nodes (80-90) - UNUSED, shops removed
// After shop removal: 70% normal, 10% elite, 10% event, 10% rest (hardcoded in GenerateMap)
const unsigned int COLOR_RED = 0xFFAAAA;
const unsigned int COLOR_GREEN = 0x00FF00;
const unsigned int COLOR_BLUE = 0x0000FF;
const unsigned int COLOR_PURPLE = 0x800080;
const unsigned int COLOR_ORANGE = 0xFFA500;
const unsigned int COLOR_GRAY = 0x808080;
const unsigned int COLOR_WHITE = 0xFFFFFF;
const char* BOSS_SAVE_FILE = "boss_save.json";

// Player initial state constants
const float PLAYER_INITIAL_VX = 2.5f;
const float PLAYER_INITIAL_VY = -3.0f;
const float PLAYER_INITIAL_ANGULAR_VEL = 0.03f;

// Game over screen layout constants
const int GAME_OVER_OPTION_BOX_WIDTH = 200;
const int GAME_OVER_OPTION_BOX_HEIGHT = 60;
const int GAME_OVER_OPTION_SPACING = 30;
const int GAME_OVER_OPTION_START_Y = 300;
const int GAME_OVER_NUM_OPTIONS = 2;

// Name entry screen constants
const int NAME_ENTRY_KEY_WIDTH = 40;
const int NAME_ENTRY_KEY_HEIGHT = 40;
const int NAME_ENTRY_KEY_SPACING = 5;
const int NAME_ENTRY_KEYS_PER_ROW = 10;
const int NAME_ENTRY_KEYBOARD_START_Y = 320;
const int NAME_ENTRY_INPUT_BOX_Y = 250;
const int NAME_ENTRY_INPUT_BOX_WIDTH = 400;
const int NAME_ENTRY_INPUT_BOX_HEIGHT = 40;
const int NAME_ENTRY_MODE_BUTTON_WIDTH = 80;
const int NAME_ENTRY_MODE_BUTTON_HEIGHT = 35;
const int NAME_ENTRY_MODE_BUTTON_Y = 180;
const int NAME_ENTRY_CONTROL_BUTTON_WIDTH = 90;
const int NAME_ENTRY_CONTROL_BUTTON_HEIGHT = 40;
const int NAME_ENTRY_CONTROL_BUTTON_Y = 580;
const int NAME_MAX_LENGTH = 20;

// Scene enum
enum Scene {
    SCENE_TITLE,
    SCENE_MAP,
    SCENE_BATTLE,
    SCENE_REWARD,
    SCENE_GAME_OVER,
    SCENE_NAME_ENTRY
};

// Battle phase enum
enum BattlePhase {
    BATTLE_PHASE_AIM,      // Initial aiming phase
    BATTLE_PHASE_FIGHTING  // Normal fighting phase
};

// Node type enum
enum NodeType {
    NODE_START,
    NODE_NORMAL,
    NODE_ELITE,
    NODE_EVENT,
    NODE_SHOP,
    NODE_REST,
    NODE_BOSS
};

// Map node structure
struct MapNode {
    int row;
    int col;
    NodeType type;
    float x, y;
    std::vector<int> connectedNodes;  // Indices of connected nodes in next row
    bool visited;
    bool reachable;
    int parentNodeIndex;  // Index of parent node in the path (-1 if none)
};

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
    int hp;             // Hit points (current)
    int maxHP;          // Maximum hit points
    int weaponDamage;   // Weapon damage modifier
    float baseSpeed;    // Base movement speed
    bool isAlive;       // Whether player is alive
    float hitCooldown;  // Cooldown timer before can be hit again (counts down)
    bool wasHitLastFrame;  // Track if was being hit last frame (for separation detection)
    float critRate;     // Critical hit rate (0.0 to 1.0)
    std::string name;   // Boss name (for persistence)
};

// Helper function to clamp HP to valid range
int ClampHP(int hp, int maxHP) {
    return (hp > maxHP) ? maxHP : ((hp < 0) ? 0 : hp);
}

// Helper function to detect key press edge (new press only, not held)
// Note: Updates prevState as a side effect to track the current key state
bool DetectKeyPressEdge(int keyCode, bool& prevState) {
    bool currentState = CheckHitKey(keyCode) != 0;
    bool pressed = currentState && !prevState;
    prevState = currentState;
    return pressed;
}

// Helper function to reset map input state after battle
// Marks keys as "pressed" so DetectKeyPressEdge will require a key release before detecting next press
void ResetMapInputState(float& cooldown, bool& prevEnter, bool& prevSpace, int& prevMouse) {
    cooldown = MAP_INPUT_COOLDOWN_DURATION;
    prevEnter = true;   // Mark as pressed to prevent immediate re-trigger
    prevSpace = true;
    prevMouse = GetMouseInput();  // Update to current state to prevent immediate re-trigger
}

// Helper function to calculate total weapon damage for a character (with crit chance)
int GetTotalWeaponDamage(const Circle& character, bool& isCrit) {
    int baseDamage = WEAPON_DAMAGE + character.weaponDamage;
    
    // Check for critical hit
    static std::mt19937 critGen(std::random_device{}());
    std::uniform_real_distribution<float> critDist(0.0f, 1.0f);
    
    if (critDist(critGen) < character.critRate) {
        isCrit = true;
        return (int)(baseDamage * 1.5f);
    }
    
    isCrit = false;
    return baseDamage;
}

// Helper function to calculate total weapon damage for display (no crit)
int GetTotalWeaponDamage(const Circle& character) {
    return WEAPON_DAMAGE + character.weaponDamage;
}

// Boss save/load functions
void SaveBossToJSON(const Circle& player) {
    try {
        json j;
        j["maxHP"] = player.maxHP;
        j["weaponDamage"] = player.weaponDamage;
        j["baseSpeed"] = player.baseSpeed;
        j["weaponType"] = (int)player.weapon.type;
        j["weaponLength"] = player.weapon.length;
        j["name"] = player.name;
        
        std::ofstream file(BOSS_SAVE_FILE);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
        }
    } catch (...) {
        // Fail silently if save fails
    }
}

bool LoadBossFromJSON(Circle& boss) {
    try {
        std::ifstream file(BOSS_SAVE_FILE);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        file.close();
        
        // Load boss parameters with validation and defaults
        boss.maxHP = j.value("maxHP", MAX_HP);
        // Ensure maxHP is positive (fallback to default if invalid)
        if (boss.maxHP <= 0) {
            boss.maxHP = MAX_HP;
        }
        boss.hp = boss.maxHP;  // Set current HP to maxHP on load
        boss.weaponDamage = j.value("weaponDamage", 0);
        boss.baseSpeed = j.value("baseSpeed", 1.0f);
        
        // Validate weapon type enum
        int weaponTypeInt = j.value("weaponType", (int)WEAPON_SPEAR);
        if (weaponTypeInt < WEAPON_BOOMERANG || weaponTypeInt > WEAPON_SPEAR) {
            weaponTypeInt = WEAPON_SPEAR;  // Default to spear if invalid
        }
        boss.weapon.type = (WeaponType)weaponTypeInt;
        
        boss.weapon.length = j.value("weaponLength", 30.0f);
        boss.name = j.value("name", std::string("Boss"));
        
        return true;
    } catch (...) {
        return false;
    }
}

// Get node type color
unsigned int GetNodeColor(NodeType type) {
    switch (type) {
        case NODE_START: return COLOR_GREEN;
        case NODE_NORMAL: return COLOR_GRAY;
        case NODE_ELITE: return COLOR_ORANGE;
        case NODE_EVENT: return COLOR_BLUE;
        case NODE_SHOP: return COLOR_YELLOW;
        case NODE_REST: return COLOR_GREEN;
        case NODE_BOSS: return COLOR_RED;
        default: return COLOR_GRAY;
    }
}

// Get node type name
const char* GetNodeTypeName(NodeType type) {
    switch (type) {
        case NODE_START: return "Start";
        case NODE_NORMAL: return "Battle";
        case NODE_ELITE: return "Elite";
        case NODE_EVENT: return "Event";
        case NODE_SHOP: return "Shop";
        case NODE_REST: return "Rest";
        case NODE_BOSS: return "Boss";
        default: return "Unknown";
    }
}

// Function to draw a boomerang
void DrawBoomerang(float x, float y, float angle, float length, unsigned int color) {
    // Boomerang as two connected arcs forming a V shape
    // Scale proportionally: arm extends 20.0f at reference length, scale maintains this ratio
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    float scale = length / WEAPON_REFERENCE_LENGTH;  // Scale based on reference length
    
    // First arm (scaled)
    float x1 = x + cos_a * 20.0f * scale - sin_a * 5.0f * scale;
    float y1 = y + sin_a * 20.0f * scale + cos_a * 5.0f * scale;
    float x2 = x + cos_a * 5.0f * scale - sin_a * 2.0f * scale;
    float y2 = y + sin_a * 5.0f * scale + cos_a * 2.0f * scale;
    
    // Second arm (scaled)
    float x3 = x + cos_a * 5.0f * scale + sin_a * 2.0f * scale;
    float y3 = y + sin_a * 5.0f * scale - cos_a * 2.0f * scale;
    float x4 = x + cos_a * 20.0f * scale + sin_a * 5.0f * scale;
    float y4 = y + sin_a * 20.0f * scale - cos_a * 5.0f * scale;
    
    // Draw lines for boomerang arms
    DrawLineAA(x, y, x1, y1, color, 3.0f);
    DrawLineAA(x, y, x4, y4, color, 3.0f);
    DrawLineAA(x1, y1, x2, y2, color, 3.0f);
    DrawLineAA(x4, y4, x3, y3, color, 3.0f);
}

// Function to draw a spear
void DrawSpear(float x, float y, float angle, float length, unsigned int color) {
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    // Spear shaft (scaled by length parameter)
    float shaftLen = length;
    float sx = x - cos_a * shaftLen * 0.5f;
    float sy = y - sin_a * shaftLen * 0.5f;
    float ex = x + cos_a * shaftLen * 0.5f;
    float ey = y + sin_a * shaftLen * 0.5f;
    DrawLineAA(sx, sy, ex, ey, color, 3.0f);
    
    // Spear head (triangle) - extends from shaft end
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

// Check and handle circle-circle collision
bool HandlePlayerCollision(Circle& c1, Circle& c2) {
    float dx = c2.x - c1.x;
    float dy = c2.y - c1.y;
    float distSq = dx * dx + dy * dy;
    float minDist = CIRCLE_RADIUS * 2.0f;
    
    if (distSq < minDist * minDist && distSq > 0.001f) {
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
        
        return true;  // Collision occurred
    }
    return false;
}

// Helper function: Calculate distance between two line segments (approximation)
float SegmentDistance(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    // Simple approximation: check endpoints to opposite segments
    float d1 = DistancePointToSegment(x1, y1, x3, y3, x4, y4);
    float d2 = DistancePointToSegment(x2, y2, x3, y3, x4, y4);
    float d3 = DistancePointToSegment(x3, y3, x1, y1, x2, y2);
    float d4 = DistancePointToSegment(x4, y4, x1, y1, x2, y2);
    
    float minDist = d1;
    if (d2 < minDist) minDist = d2;
    if (d3 < minDist) minDist = d3;
    if (d4 < minDist) minDist = d4;
    
    return minDist;
}

// Check and handle weapon-weapon collision
bool HandleWeaponCollision(Circle& c1, Circle& c2) {
    // Get weapon world positions
    float w1x, w1y, w2x, w2y;
    GetWeaponWorldPosition(c1, w1x, w1y);
    GetWeaponWorldPosition(c2, w2x, w2y);
    
    // Calculate weapon segment endpoints
    float cos1 = cosf(c1.angle);
    float sin1 = sinf(c1.angle);
    float halfLen1 = c1.weapon.length * 0.5f;
    float w1x1 = w1x - cos1 * halfLen1;
    float w1y1 = w1y - sin1 * halfLen1;
    float w1x2 = w1x + cos1 * halfLen1;
    float w1y2 = w1y + sin1 * halfLen1;
    
    float cos2 = cosf(c2.angle);
    float sin2 = sinf(c2.angle);
    float halfLen2 = c2.weapon.length * 0.5f;
    float w2x1 = w2x - cos2 * halfLen2;
    float w2y1 = w2y - sin2 * halfLen2;
    float w2x2 = w2x + cos2 * halfLen2;
    float w2y2 = w2y + sin2 * halfLen2;
    
    // Check if weapons are close enough to collide
    float dist = SegmentDistance(w1x1, w1y1, w1x2, w1y2, w2x1, w2y1, w2x2, w2y2);
    
    if (dist < WEAPON_COLLISION_THRESHOLD) {
        // Simple bounce: reverse angular velocities to make weapons separate
        // No damping to prevent rotation slowdown during collisions
        c1.angularVel = -c1.angularVel;
        c2.angularVel = -c2.angularVel;
        
        return true;  // Collision occurred
    }
    return false;
}

// Generate branching map
std::vector<MapNode> GenerateMap() {
    std::vector<MapNode> nodes;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Track node counts per row
    std::vector<std::vector<int>> rowNodes(MAP_ROWS);
    
    int nodeIndex = 0;
    
    // Row 0: Start node
    MapNode startNode;
    startNode.row = 0;
    startNode.col = 0;
    startNode.type = NODE_START;
    startNode.visited = false;
    startNode.reachable = true;
    startNode.parentNodeIndex = -1;  // Start node has no parent
    rowNodes[0].push_back(nodeIndex++);
    nodes.push_back(startNode);
    
    // Rows 1-5: Mixed nodes (normal 70%, elite 10%, event 10%, rest 10%)
    for (int row = 1; row < MAP_ROWS - 2; row++) {
        // Random nodes per row based on constants
        std::uniform_int_distribution<> nodeDist(MAP_MIN_NODES_PER_ROW, MAP_MAX_NODES_PER_ROW);
        int numNodes = nodeDist(gen);
        
        for (int col = 0; col < numNodes; col++) {
            MapNode node;
            node.row = row;
            node.col = col;
            node.visited = false;
            node.reachable = false;
            node.parentNodeIndex = -1;  // Will be set when node is selected
            
            // Node type distribution: 70% normal, 10% elite, 10% event, 10% rest
            std::uniform_int_distribution<> typeDist(0, 99);
            int typeRoll = typeDist(gen);
            if (typeRoll < 70) {
                node.type = NODE_NORMAL;
            } else if (typeRoll < 80) {
                node.type = NODE_ELITE;
            } else if (typeRoll < 90) {
                node.type = NODE_EVENT;
            } else {
                node.type = NODE_REST;
            }
            
            rowNodes[row].push_back(nodeIndex++);
            nodes.push_back(node);
        }
    }
    
    // Row 6 (MAP_ROWS - 2): Mandatory boss-ante rest node (single, must pass through)
    MapNode bossAnteRestNode;
    bossAnteRestNode.row = MAP_ROWS - 2;
    bossAnteRestNode.col = 0;
    bossAnteRestNode.type = NODE_REST;
    bossAnteRestNode.visited = false;
    bossAnteRestNode.reachable = false;
    bossAnteRestNode.parentNodeIndex = -1;  // Will be set when node is selected
    rowNodes[MAP_ROWS - 2].push_back(nodeIndex++);
    nodes.push_back(bossAnteRestNode);
    
    // Row 7 (top): Boss node (single)
    MapNode bossNode;
    bossNode.row = MAP_ROWS - 1;
    bossNode.col = 0;
    bossNode.type = NODE_BOSS;
    bossNode.visited = false;
    bossNode.reachable = false;
    bossNode.parentNodeIndex = -1;  // Will be set when node is selected
    rowNodes[MAP_ROWS - 1].push_back(nodeIndex++);
    nodes.push_back(bossNode);
    
    // Connect nodes (roughly 2 edges upward per node on average)
    // First pass: random connections
    for (int row = 0; row < MAP_ROWS - 1; row++) {
        for (int idx : rowNodes[row]) {
            int nextRowSize = (int)rowNodes[row + 1].size();
            if (nextRowSize == 0) continue;
            
            // Each node connects to 1-3 nodes in next row (average 2)
            int maxConnections = (nextRowSize < 3) ? nextRowSize : 3;
            std::uniform_int_distribution<int> connDist(1, maxConnections);
            int numConnections = connDist(gen);
            
            // Randomly select nodes to connect to
            std::vector<int> nextIndices = rowNodes[row + 1];
            std::shuffle(nextIndices.begin(), nextIndices.end(), gen);
            
            for (int i = 0; i < numConnections && i < (int)nextIndices.size(); i++) {
                nodes[idx].connectedNodes.push_back(nextIndices[i]);
            }
        }
    }
    
    // Second pass: ensure all nodes in next row are reachable
    for (int row = 1; row < MAP_ROWS; row++) {
        for (int nextIdx : rowNodes[row]) {
            // Check if this node is reachable from any node in previous row
            bool isReachable = false;
            for (int prevIdx : rowNodes[row - 1]) {
                for (int connectedIdx : nodes[prevIdx].connectedNodes) {
                    if (connectedIdx == nextIdx) {
                        isReachable = true;
                        break;
                    }
                }
                if (isReachable) break;
            }
            
            // If not reachable, connect from a random node in previous row
            if (!isReachable && !rowNodes[row - 1].empty()) {
                std::uniform_int_distribution<> prevDist(0, (int)rowNodes[row - 1].size() - 1);
                int prevIdx = rowNodes[row - 1][prevDist(gen)];
                nodes[prevIdx].connectedNodes.push_back(nextIdx);
            }
        }
    }
    
    // Calculate node positions for display
    const float rowSpacing = (MAP_BOTTOM - MAP_TOP) / (MAP_ROWS - 1);
    
    for (size_t i = 0; i < nodes.size(); i++) {
        int row = nodes[i].row;
        int numNodesInRow = (int)rowNodes[row].size();
        int colIndex = 0;
        for (int j = 0; j < numNodesInRow; j++) {
            if (rowNodes[row][j] == (int)i) {
                colIndex = j;
                break;
            }
        }
        
        float rowWidth = MAP_RIGHT - MAP_LEFT;
        float nodeSpacing = (numNodesInRow > 1) ? rowWidth / (numNodesInRow - 1) : 0.0f;
        
        nodes[i].x = MAP_LEFT + (numNodesInRow > 1 ? colIndex * nodeSpacing : rowWidth * 0.5f);
        nodes[i].y = MAP_BOTTOM - row * rowSpacing;  // Invert so row 0 is at bottom
    }
    
    return nodes;
}

// Helper function to draw an arrow from one point to another
void DrawArrow(float x1, float y1, float x2, float y2, unsigned int color, float thickness) {
    // Draw line from start to end
    DrawLineAA(x1, y1, x2, y2, color, thickness);
    
    // Calculate arrowhead
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrtf(dx * dx + dy * dy);
    
    if (length > 0.1f) {
        float dirX = dx / length;
        float dirY = dy / length;
        
        // Arrowhead parameters
        float arrowHeadLen = 8.0f;
        float arrowHeadAngle = 0.4f;  // radians
        
        // Calculate arrowhead points
        float headX1 = x2 - dirX * arrowHeadLen * cosf(arrowHeadAngle) + dirY * arrowHeadLen * sinf(arrowHeadAngle);
        float headY1 = y2 - dirY * arrowHeadLen * cosf(arrowHeadAngle) - dirX * arrowHeadLen * sinf(arrowHeadAngle);
        float headX2 = x2 - dirX * arrowHeadLen * cosf(arrowHeadAngle) - dirY * arrowHeadLen * sinf(arrowHeadAngle);
        float headY2 = y2 - dirY * arrowHeadLen * cosf(arrowHeadAngle) + dirX * arrowHeadLen * sinf(arrowHeadAngle);
        
        // Draw arrowhead
        DrawLineAA(x2, y2, headX1, headY1, color, thickness);
        DrawLineAA(x2, y2, headX2, headY2, color, thickness);
    }
}

// Draw map scene
void DrawMap(const std::vector<MapNode>& nodes, int currentNodeIndex, int highlightedNodeIndex) {
    // Draw title
    DrawFormatString(SCREEN_WIDTH / 2 - 50, 20, COLOR_BLACK, "Map - Select Path");
    
    // Get current row for comparison
    int currentRow = nodes[currentNodeIndex].row;
    
    // Build a list of traversed edges (for exclusion from gray rendering)
    std::vector<std::pair<int, int>> traversedEdges;
    {
        int nodeIdx = currentNodeIndex;
        while (nodeIdx != -1 && nodes[nodeIdx].parentNodeIndex != -1) {
            int parentIdx = nodes[nodeIdx].parentNodeIndex;
            traversedEdges.push_back({parentIdx, nodeIdx});
            nodeIdx = parentIdx;
        }
    }
    
    // Draw gray arrows for all reachable edges in current row and future rows
    // (rows >= currentRow)
    for (size_t i = 0; i < nodes.size(); i++) {
        // Only process nodes in current row or future rows (rows >= currentRow)
        if (nodes[i].row < currentRow) continue;
        
        // Draw edges from this node to connected nodes
        for (int connectedIdx : nodes[i].connectedNodes) {
            // Check if this edge is part of traversed path (skip if it is, will draw in blue)
            bool isTraversed = false;
            for (const auto& edge : traversedEdges) {
                if (edge.first == (int)i && edge.second == connectedIdx) {
                    isTraversed = true;
                    break;
                }
            }
            
            // Check if this edge is the highlighted edge (skip if it is, will draw in red)
            bool isHighlighted = ((int)i == currentNodeIndex && connectedIdx == highlightedNodeIndex &&
                                 highlightedNodeIndex >= 0 && nodes[highlightedNodeIndex].reachable && 
                                 !nodes[highlightedNodeIndex].visited);
            
            // Draw gray arrow only if not traversed and not highlighted
            if (!isTraversed && !isHighlighted) {
                DrawArrow(nodes[i].x, nodes[i].y,
                         nodes[connectedIdx].x, nodes[connectedIdx].y,
                         COLOR_GRAY, 2.0f);
            }
        }
    }
    
    // Draw blue arrows for passed route (from start to current node)
    int nodeIdx = currentNodeIndex;
    while (nodeIdx != -1 && nodes[nodeIdx].parentNodeIndex != -1) {
        int parentIdx = nodes[nodeIdx].parentNodeIndex;
        DrawArrow(nodes[parentIdx].x, nodes[parentIdx].y,
                 nodes[nodeIdx].x, nodes[nodeIdx].y,
                 COLOR_BLUE, 3.0f);
        nodeIdx = parentIdx;
    }
    
    // Draw red arrows for path from current node to highlighted node
    if (highlightedNodeIndex >= 0 && highlightedNodeIndex != currentNodeIndex && 
        nodes[highlightedNodeIndex].reachable && !nodes[highlightedNodeIndex].visited) {
        DrawArrow(nodes[currentNodeIndex].x, nodes[currentNodeIndex].y,
                 nodes[highlightedNodeIndex].x, nodes[highlightedNodeIndex].y,
                 COLOR_RED, 3.0f);
    }
    
    // Draw nodes
    for (size_t i = 0; i < nodes.size(); i++) {
        unsigned int nodeColor = GetNodeColor(nodes[i].type);
        
        // Darken visited nodes
        if (nodes[i].visited) {
            nodeColor = COLOR_GRAY;
        }
        
        // Draw node circle
        DrawCircleAA(nodes[i].x, nodes[i].y, MAP_NODE_RADIUS, 32, nodeColor, TRUE);
        
        // Highlight current node
        if ((int)i == currentNodeIndex) {
            DrawCircleAA(nodes[i].x, nodes[i].y, MAP_NODE_RADIUS + 3, 32, COLOR_WHITE, FALSE);
            DrawCircleAA(nodes[i].x, nodes[i].y, MAP_NODE_RADIUS + 5, 32, COLOR_CYAN, FALSE);
        }
        
        // Highlight hovered/selected node
        if ((int)i == highlightedNodeIndex && nodes[i].reachable) {
            DrawCircleAA(nodes[i].x, nodes[i].y, MAP_NODE_RADIUS + 2, 32, COLOR_YELLOW, FALSE);
        }
        
        // Draw black outline
        DrawCircleAA(nodes[i].x, nodes[i].y, MAP_NODE_RADIUS, 32, COLOR_BLACK, FALSE);
    }
    
    // Draw node type info at bottom
    if (highlightedNodeIndex >= 0 && highlightedNodeIndex < (int)nodes.size()) {
        const char* typeName = GetNodeTypeName(nodes[highlightedNodeIndex].type);
        DrawFormatString(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 40, COLOR_BLACK, 
                        "Node: %s", typeName);
    }
    
    // Draw instructions
    DrawFormatString(10, SCREEN_HEIGHT - 60, COLOR_BLACK, "Mouse: Click node to select");
    DrawFormatString(10, SCREEN_HEIGHT - 40, COLOR_BLACK, "ESC: Quit");
}

// Update reachability after selecting a node
void UpdateReachability(std::vector<MapNode>& mapNodes, int currentNodeIndex) {
    // Clear all reachability
    for (size_t i = 0; i < mapNodes.size(); i++) {
        mapNodes[i].reachable = false;
    }
    // Set connected nodes as reachable and set parent
    for (int connectedIdx : mapNodes[currentNodeIndex].connectedNodes) {
        mapNodes[connectedIdx].reachable = true;
        mapNodes[connectedIdx].parentNodeIndex = currentNodeIndex;
    }
}

// Helper function to reset player character to initial state
void ResetPlayerCharacter(Circle& playerChar) {
    playerChar.hp = MAX_HP;
    playerChar.maxHP = MAX_HP;
    playerChar.weaponDamage = 0;
    playerChar.baseSpeed = 1.0f;
    playerChar.vx = PLAYER_INITIAL_VX;
    playerChar.vy = PLAYER_INITIAL_VY;
    playerChar.angularVel = PLAYER_INITIAL_ANGULAR_VEL;
    playerChar.critRate = 0.0f;
}

// Character set mode enum
enum CharSetMode {
    CHARSET_HIRAGANA,
    CHARSET_KATAKANA,
    CHARSET_ENGLISH,
    CHARSET_SYMBOLS
};

// Get character set based on mode
std::vector<std::string> GetCharacterSet(CharSetMode mode) {
    std::vector<std::string> chars;
    
    if (mode == CHARSET_HIRAGANA) {
        // Hiragana characters
        const char* hiragana[] = {
            "あ", "い", "う", "え", "お", "か", "き", "く", "け", "こ",
            "さ", "し", "す", "せ", "そ", "た", "ち", "つ", "て", "と",
            "な", "に", "ぬ", "ね", "の", "は", "ひ", "ふ", "へ", "ほ",
            "ま", "み", "む", "め", "も", "や", "ゆ", "よ", "ら", "り",
            "る", "れ", "ろ", "わ", "を", "ん", "が", "ぎ", "ぐ", "げ"
        };
        for (int i = 0; i < 50; i++) {
            chars.push_back(hiragana[i]);
        }
    } else if (mode == CHARSET_KATAKANA) {
        // Katakana characters
        const char* katakana[] = {
            "ア", "イ", "ウ", "エ", "オ", "カ", "キ", "ク", "ケ", "コ",
            "サ", "シ", "ス", "セ", "ソ", "タ", "チ", "ツ", "テ", "ト",
            "ナ", "ニ", "ヌ", "ネ", "ノ", "ハ", "ヒ", "フ", "ヘ", "ホ",
            "マ", "ミ", "ム", "メ", "モ", "ヤ", "ユ", "ヨ", "ラ", "リ",
            "ル", "レ", "ロ", "ワ", "ヲ", "ン", "ガ", "ギ", "グ", "ゲ"
        };
        for (int i = 0; i < 50; i++) {
            chars.push_back(katakana[i]);
        }
    } else if (mode == CHARSET_ENGLISH) {
        // English alphabet (uppercase + lowercase) and numbers
        const char* english[] = {
            "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
            "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
            "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3",
            "4", "5", "6", "7", "8", "9", " ", "a", "b", "c",
            "d", "e", "f", "g", "h", "i", "j", "k", "l", "m"
        };
        for (int i = 0; i < 50; i++) {
            chars.push_back(english[i]);
        }
    } else if (mode == CHARSET_SYMBOLS) {
        // Lowercase letters and symbols
        const char* symbols[] = {
            "n", "o", "p", "q", "r", "s", "t", "u", "v", "w",
            "x", "y", "z", "!", "?", ".", ",", "-", "_", "+",
            "=", "*", "/", "@", "#", "$", "%", "&", "(", ")",
            "[", "]", "{", "}", "<", ">", ":", ";", "\"", "'",
            "`", "~", "^", "|", "\\", "。", "、", "！", "？", "～"
        };
        for (int i = 0; i < 50; i++) {
            chars.push_back(symbols[i]);
        }
    }
    
    return chars;
}

// Helper function to remove last character from UTF-8 string
void RemoveLastCharacterUTF8(std::string& str) {
    if (str.length() > 0) {
        // Remove continuation bytes (10xxxxxx)
        while (str.length() > 0 && (str.back() & 0xC0) == 0x80) {
            str.pop_back();
        }
        // Remove the leading byte
        if (str.length() > 0) {
            str.pop_back();
        }
    }
}

// Draw title screen
void DrawTitleScreen() {
    // Draw title
    DrawFormatString(SCREEN_WIDTH / 2 - 100, 200, COLOR_BLACK, "SPHERE BATTLERS");
    
    // Draw start instruction
    DrawFormatString(SCREEN_WIDTH / 2 - 80, 300, COLOR_BLACK, "Click Anywhere to Start");
    
    // Draw controls info
    DrawFormatString(SCREEN_WIDTH / 2 - 80, 400, COLOR_BLACK, "ESC: Quit");
}

// Draw game over screen
void DrawGameOverScreen(int selectedOption) {
    // Draw game over text
    DrawFormatString(SCREEN_WIDTH / 2 - 60, 200, COLOR_BLACK, "GAME OVER");
    
    // Calculate option positions
    const int OPTION_X = (SCREEN_WIDTH - GAME_OVER_OPTION_BOX_WIDTH) / 2;
    
    // Retry option
    int retryY = GAME_OVER_OPTION_START_Y;
    unsigned int retryColor = (selectedOption == 0) ? COLOR_YELLOW : COLOR_WHITE;
    DrawBox(OPTION_X, retryY, OPTION_X + GAME_OVER_OPTION_BOX_WIDTH, retryY + GAME_OVER_OPTION_BOX_HEIGHT, retryColor, TRUE);
    DrawBox(OPTION_X, retryY, OPTION_X + GAME_OVER_OPTION_BOX_WIDTH, retryY + GAME_OVER_OPTION_BOX_HEIGHT, COLOR_BLACK, FALSE);
    DrawFormatString(OPTION_X + 40, retryY + 20, COLOR_BLACK, "Retry (Restart Run)");
    
    // Return to title option
    int titleY = GAME_OVER_OPTION_START_Y + GAME_OVER_OPTION_BOX_HEIGHT + GAME_OVER_OPTION_SPACING;
    unsigned int titleColor = (selectedOption == 1) ? COLOR_YELLOW : COLOR_WHITE;
    DrawBox(OPTION_X, titleY, OPTION_X + GAME_OVER_OPTION_BOX_WIDTH, titleY + GAME_OVER_OPTION_BOX_HEIGHT, titleColor, TRUE);
    DrawBox(OPTION_X, titleY, OPTION_X + GAME_OVER_OPTION_BOX_WIDTH, titleY + GAME_OVER_OPTION_BOX_HEIGHT, COLOR_BLACK, FALSE);
    DrawFormatString(OPTION_X + 45, titleY + 20, COLOR_BLACK, "Return to Title");
    
    // Draw instructions
    DrawFormatString(SCREEN_WIDTH / 2 - 80, 500, COLOR_BLACK, "Mouse: Click on option");
}

// Initialize battle characters
void InitializeBattle(Circle& player, Circle& enemy, const Circle& playerChar, 
                      NodeType nodeType, const std::vector<MapNode>& mapNodes, int currentNodeIndex) {
    // Initialize battle characters
    player = playerChar;
    player.x = FRAME_LEFT + FRAME_WIDTH * 0.3f;
    player.y = FRAME_BOTTOM - 80.0f;
    player.hitTimer = 0.0f;
    player.isAlive = true;
    player.hitCooldown = 0.0f;
    player.wasHitLastFrame = false;
    
    // Initialize enemy
    enemy.x = FRAME_LEFT + FRAME_WIDTH * 0.7f;
    enemy.y = FRAME_BOTTOM - 80.0f;
    enemy.angle = INITIAL_WEAPON_ANGLE;
    enemy.number = 91;
    enemy.hitTimer = 0.0f;
    enemy.isAlive = true;
    enemy.hitCooldown = 0.0f;
    enemy.wasHitLastFrame = false;
    enemy.maxHP = MAX_HP;
    enemy.weaponDamage = 0;
    enemy.baseSpeed = 1.0f;
    enemy.critRate = 0.0f;
    
    // Set enemy parameters based on node type
    if (nodeType == NODE_BOSS) {
        // Try to load boss from JSON
        if (!LoadBossFromJSON(enemy)) {
            // Default boss (if load failed or no file)
            enemy.hp = enemy.maxHP;
            enemy.weapon.type = WEAPON_SPEAR;
            enemy.weapon.length = 30.0f;
            enemy.name = "Boss";
        }
        // Always set these defaults for boss (not persisted in save)
        enemy.vx = -2.0f;
        enemy.vy = -3.5f;
        enemy.angularVel = -0.025f;
        enemy.color = COLOR_RED;
        enemy.weapon.offsetX = 45.0f;
        enemy.weapon.offsetY = 0.0f;
        enemy.weapon.color = COLOR_RED;
    } else if (nodeType == NODE_ELITE) {
        // Elite enemy
        enemy.vx = -2.0f;
        enemy.vy = -3.5f;
        enemy.angularVel = -0.025f;
        enemy.color = COLOR_RED;
        enemy.maxHP = 30;
        enemy.hp = enemy.maxHP;
        enemy.weapon.type = WEAPON_SPEAR;
        enemy.weapon.offsetX = 45.0f;
        enemy.weapon.offsetY = 0.0f;
        enemy.weapon.length = 30.0f;
        enemy.weapon.color = COLOR_RED;
    } else {
        // Normal enemy
        enemy.vx = -2.0f;
        enemy.vy = -3.5f;
        enemy.angularVel = -0.025f;
        enemy.color = COLOR_RED;
        enemy.maxHP = 10;
        enemy.hp = enemy.maxHP;
        enemy.weapon.type = WEAPON_SPEAR;
        enemy.weapon.offsetX = 45.0f;
        enemy.weapon.offsetY = 0.0f;
        enemy.weapon.length = 30.0f;
        enemy.weapon.color = COLOR_RED;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Window settings
    SetWindowText("Sphere Battlers - Map & Battle");
    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
    ChangeWindowMode(TRUE);
    SetAlwaysRunFlag(TRUE);
    
    if (DxLib_Init() == -1) {
        return -1;
    }
    
    SetDrawScreen(DX_SCREEN_BACK);
    
    // Initialize scene state
    Scene currentScene = SCENE_TITLE;
    
    // Generate map
    std::vector<MapNode> mapNodes = GenerateMap();
    int currentNodeIndex = 0;  // Start at first node
    int highlightedNodeIndex = 0;
    mapNodes[0].visited = true;
    
    // Update reachability from start
    for (int connectedIdx : mapNodes[0].connectedNodes) {
        mapNodes[connectedIdx].reachable = true;
        mapNodes[connectedIdx].parentNodeIndex = 0;  // Set parent to start node
    }
    
    // Battle state
    Circle circles[2];
    float hitStopTimer = 0.0f;
    bool battleEnded = false;
    bool playerWon = false;
    BattlePhase battlePhase = BATTLE_PHASE_AIM;
    
    // Aim phase state
    bool aimPhaseActive = false;
    bool aimMouseDragging = false;
    int aimMouseStartX = 0, aimMouseStartY = 0;
    float aimKeyboardDirX = 0.0f, aimKeyboardDirY = 0.0f;
    bool aimKeyboardActive = false;
    
    // Player character (persistent across battles)
    Circle playerChar;
    playerChar.x = FRAME_LEFT + FRAME_WIDTH * 0.3f;
    playerChar.y = FRAME_BOTTOM - 80.0f;
    playerChar.vx = PLAYER_INITIAL_VX;
    playerChar.vy = PLAYER_INITIAL_VY;
    playerChar.angle = INITIAL_WEAPON_ANGLE;
    playerChar.angularVel = PLAYER_INITIAL_ANGULAR_VEL;
    playerChar.number = 92;
    playerChar.color = COLOR_CYAN;
    playerChar.hp = MAX_HP;
    playerChar.weapon.type = WEAPON_SPEAR;
    playerChar.weapon.offsetX = 40.0f;
    playerChar.weapon.offsetY = 0.0f;
    playerChar.weapon.length = 25.0f;
    playerChar.weapon.color = COLOR_CYAN;
    playerChar.maxHP = MAX_HP;
    playerChar.weaponDamage = 0;
    playerChar.baseSpeed = 1.0f;
    playerChar.critRate = 0.0f;
    
    // Mouse state tracking
    int prevMouseState = 0;
    
    // Keyboard navigation state
    int keyPressDelay = 0;
    const int KEY_PRESS_COOLDOWN = 10;
    
    // Input cooldown for map scene after battle return
    float mapInputCooldown = 0.0f;
    
    // Keyboard state tracking for edge detection
    bool prevEnterKeyState = false;
    bool prevSpaceKeyState = false;
    
    // Game over screen state
    int gameOverSelectedOption = 0;  // 0 = Retry, 1 = Return to Title
    
    // Main loop
    while (ProcessMessage() == 0) {
        // Check for ESC key to exit
        if (CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }
        
        // Clear screen with beige background
        ClearDrawScreen();
        DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BEIGE, TRUE);
        
        if (currentScene == SCENE_TITLE) {
            // ===== TITLE SCENE =====
            
            DrawTitleScreen();
            
            // Mouse click only to start
            int mouseState = GetMouseInput();
            bool mouseClicked = (mouseState & MOUSE_INPUT_LEFT) && !(prevMouseState & MOUSE_INPUT_LEFT);
            prevMouseState = mouseState;
            
            if (mouseClicked) {
                // Start game - transition to map
                currentScene = SCENE_MAP;
            }
            
        } else if (currentScene == SCENE_GAME_OVER) {
            // ===== GAME OVER SCENE =====
            
            // Handle mouse hover
            int mouseX, mouseY;
            GetMousePoint(&mouseX, &mouseY);
            
            const int OPTION_X = (SCREEN_WIDTH - GAME_OVER_OPTION_BOX_WIDTH) / 2;
            
            // Check hover for Retry option
            int retryY = GAME_OVER_OPTION_START_Y;
            if (mouseX >= OPTION_X && mouseX <= OPTION_X + GAME_OVER_OPTION_BOX_WIDTH &&
                mouseY >= retryY && mouseY <= retryY + GAME_OVER_OPTION_BOX_HEIGHT) {
                gameOverSelectedOption = 0;
            }
            
            // Check hover for Return to Title option
            int titleY = GAME_OVER_OPTION_START_Y + GAME_OVER_OPTION_BOX_HEIGHT + GAME_OVER_OPTION_SPACING;
            if (mouseX >= OPTION_X && mouseX <= OPTION_X + GAME_OVER_OPTION_BOX_WIDTH &&
                mouseY >= titleY && mouseY <= titleY + GAME_OVER_OPTION_BOX_HEIGHT) {
                gameOverSelectedOption = 1;
            }
            
            // Handle mouse click selection only
            int mouseState = GetMouseInput();
            bool mouseClicked = (mouseState & MOUSE_INPUT_LEFT) && !(prevMouseState & MOUSE_INPUT_LEFT);
            prevMouseState = mouseState;
            
            if (mouseClicked) {
                if (gameOverSelectedOption == 0) {
                    // Retry - restart from beginning
                    mapNodes = GenerateMap();
                    currentNodeIndex = 0;
                    highlightedNodeIndex = 0;
                    mapNodes[0].visited = true;
                    for (int connectedIdx : mapNodes[0].connectedNodes) {
                        mapNodes[connectedIdx].reachable = true;
                        mapNodes[connectedIdx].parentNodeIndex = 0;
                    }
                    
                    // Reset player
                    ResetPlayerCharacter(playerChar);
                    
                    // Set input cooldown and clear input state before returning to map
                    ResetMapInputState(mapInputCooldown, prevEnterKeyState, prevSpaceKeyState, prevMouseState);
                    currentScene = SCENE_MAP;
                } else {
                    // Return to title
                    ResetPlayerCharacter(playerChar);
                    currentScene = SCENE_TITLE;
                }
            }
            
            DrawGameOverScreen(gameOverSelectedOption);
            
        } else if (currentScene == SCENE_MAP) {
            // ===== MAP SCENE =====
            
            // Update input cooldown timer
            if (mapInputCooldown > 0.0f) {
                mapInputCooldown -= 1.0f;
            }
            
            // Update highlighted node based on mouse position
            int mouseX, mouseY;
            GetMousePoint(&mouseX, &mouseY);
            highlightedNodeIndex = -1;
            
            for (size_t i = 0; i < mapNodes.size(); i++) {
                if (!mapNodes[i].reachable || mapNodes[i].visited) continue;
                
                float dx = mouseX - mapNodes[i].x;
                float dy = mouseY - mapNodes[i].y;
                float distSq = dx * dx + dy * dy;
                
                if (distSq < MAP_NODE_RADIUS * MAP_NODE_RADIUS) {
                    highlightedNodeIndex = (int)i;
                    break;
                }
            }
            
            // Mouse click to select node (with cooldown)
            int mouseState = GetMouseInput();
            if ((mouseState & MOUSE_INPUT_LEFT) && !(prevMouseState & MOUSE_INPUT_LEFT) && mapInputCooldown <= 0.0f) {
                // Mouse clicked
                if (highlightedNodeIndex >= 0 && mapNodes[highlightedNodeIndex].reachable && !mapNodes[highlightedNodeIndex].visited) {
                    currentNodeIndex = highlightedNodeIndex;
                    mapNodes[currentNodeIndex].visited = true;
                    
                    // Update reachability
                    UpdateReachability(mapNodes, currentNodeIndex);
                    
                    // Enter battle scene
                    if (mapNodes[currentNodeIndex].type == NODE_NORMAL || 
                        mapNodes[currentNodeIndex].type == NODE_ELITE || 
                        mapNodes[currentNodeIndex].type == NODE_BOSS) {
                        currentScene = SCENE_BATTLE;
                        
                        // Initialize battle
                        InitializeBattle(circles[0], circles[1], playerChar, 
                                       mapNodes[currentNodeIndex].type, mapNodes, currentNodeIndex);
                        
                        hitStopTimer = 0.0f;
                        battleEnded = false;
                        playerWon = false;
                        battlePhase = BATTLE_PHASE_AIM;
                        aimPhaseActive = true;
                        aimMouseDragging = false;
                        aimKeyboardDirX = 0.0f;
                        aimKeyboardDirY = 0.0f;
                        aimKeyboardActive = false;
                    } else if (mapNodes[currentNodeIndex].type == NODE_REST) {
                        // Rest node healing
                        // Boss-ante rest (row before boss) heals fully, general rest heals 50%
                        int healAmount;
                        if (mapNodes[currentNodeIndex].row == MAP_ROWS - 2) {
                            // Boss-ante rest: heal to full maxHP
                            healAmount = playerChar.maxHP - playerChar.hp;
                        } else {
                            // General rest: heal 50% of maxHP
                            healAmount = playerChar.maxHP / 2;
                        }
                        playerChar.hp = ClampHP(playerChar.hp + healAmount, playerChar.maxHP);
                    }
                }
            }
            prevMouseState = mouseState;
            
            // Draw map
            DrawMap(mapNodes, currentNodeIndex, highlightedNodeIndex);
            
        } else if (currentScene == SCENE_BATTLE) {
            // ===== BATTLE SCENE =====
            
            // Draw frame box
            DrawBox(FRAME_LEFT, FRAME_TOP, FRAME_RIGHT, FRAME_BOTTOM, COLOR_BLACK, FALSE);
            
            if (aimPhaseActive && battlePhase == BATTLE_PHASE_AIM) {
                // ===== AIM PHASE =====
                
                // Get mouse position
                int mouseX, mouseY;
                GetMousePoint(&mouseX, &mouseY);
                int mouseState = GetMouseInput();
                
                // Calculate player center for mouse drag detection
                float playerCenterX = circles[0].x;
                float playerCenterY = circles[0].y;
                float distToPlayerSq = (mouseX - playerCenterX) * (mouseX - playerCenterX) + 
                                       (mouseY - playerCenterY) * (mouseY - playerCenterY);
                
                // Mouse drag mechanics
                if ((mouseState & MOUSE_INPUT_LEFT) && !aimMouseDragging) {
                    // Check if mouse is near player (within circle radius * 2 for easier interaction)
                    if (distToPlayerSq < (CIRCLE_RADIUS * 2.0f) * (CIRCLE_RADIUS * 2.0f)) {
                        aimMouseDragging = true;
                        aimMouseStartX = mouseX;
                        aimMouseStartY = mouseY;
                    }
                }
                
                if (aimMouseDragging) {
                    if (!(mouseState & MOUSE_INPUT_LEFT)) {
                        // Mouse released - confirm aim and set velocity
                        float dx = mouseX - playerCenterX;
                        float dy = mouseY - playerCenterY;
                        float mag = sqrtf(dx * dx + dy * dy);
                        
                        if (mag > 1.0f) {
                            // Calculate base speed magnitude from initial velocity
                            float baseSpeedMag = sqrtf(circles[0].vx * circles[0].vx + circles[0].vy * circles[0].vy);
                            if (baseSpeedMag < 0.1f) baseSpeedMag = AIM_DEFAULT_SPEED_MAG;
                            
                            // Scale based on drag distance (normalize and clamp)
                            float scale = mag / AIM_ARROW_LENGTH;
                            if (scale < AIM_MIN_SPEED_SCALE) scale = AIM_MIN_SPEED_SCALE;
                            if (scale > AIM_MAX_SPEED_SCALE) scale = AIM_MAX_SPEED_SCALE;
                            
                            // Set player velocity
                            circles[0].vx = (dx / mag) * baseSpeedMag * scale;
                            circles[0].vy = (dy / mag) * baseSpeedMag * scale;
                        }
                        
                        // End aim phase
                        aimPhaseActive = false;
                        battlePhase = BATTLE_PHASE_FIGHTING;
                        aimMouseDragging = false;
                    }
                }
                
                // Draw characters in aim phase (static, no movement)
                for (int i = 0; i < 2; i++) {
                    // Draw circle with outline
                    DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, circles[i].color, TRUE);
                    DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, COLOR_BLACK, FALSE);
                    
                    // Draw HP number inside circle
                    DrawFormatString((int)(circles[i].x + CIRCLE_NUMBER_X_OFFSET), 
                                   (int)(circles[i].y + CIRCLE_NUMBER_Y_OFFSET), COLOR_BLACK, "%d", circles[i].hp);
                }
                
                // Draw player aim arrow (cyan)
                if (aimMouseDragging) {
                    // Mouse drag arrow
                    float dx = mouseX - playerCenterX;
                    float dy = mouseY - playerCenterY;
                    float mag = sqrtf(dx * dx + dy * dy);
                    
                    if (mag > 1.0f) {
                        float arrowLen = (mag < AIM_ARROW_LENGTH) ? mag : AIM_ARROW_LENGTH;
                        float dirX = dx / mag;
                        float dirY = dy / mag;
                        
                        float endX = playerCenterX + dirX * arrowLen;
                        float endY = playerCenterY + dirY * arrowLen;
                        
                        // Draw arrow line
                        DrawLineAA(playerCenterX, playerCenterY, endX, endY, COLOR_CYAN, 3.0f);
                        
                        // Draw arrowhead
                        float arrowHeadLen = 10.0f;
                        float arrowHeadAngle = 0.5f;  // radians
                        float headX1 = endX - dirX * arrowHeadLen * cosf(arrowHeadAngle) + dirY * arrowHeadLen * sinf(arrowHeadAngle);
                        float headY1 = endY - dirY * arrowHeadLen * cosf(arrowHeadAngle) - dirX * arrowHeadLen * sinf(arrowHeadAngle);
                        float headX2 = endX - dirX * arrowHeadLen * cosf(arrowHeadAngle) - dirY * arrowHeadLen * sinf(arrowHeadAngle);
                        float headY2 = endY - dirY * arrowHeadLen * cosf(arrowHeadAngle) + dirX * arrowHeadLen * sinf(arrowHeadAngle);
                        
                        DrawLineAA(endX, endY, headX1, headY1, COLOR_CYAN, 3.0f);
                        DrawLineAA(endX, endY, headX2, headY2, COLOR_CYAN, 3.0f);
                    }
                }
                
                // Draw enemy aim arrow (red, fixed initial direction)
                float enemyCenterX = circles[1].x;
                float enemyCenterY = circles[1].y;
                float enemyVx = circles[1].vx;
                float enemyVy = circles[1].vy;
                float enemySpeedMag = sqrtf(enemyVx * enemyVx + enemyVy * enemyVy);
                
                if (enemySpeedMag > 0.1f) {
                    float enemyDirX = enemyVx / enemySpeedMag;
                    float enemyDirY = enemyVy / enemySpeedMag;
                    float enemyEndX = enemyCenterX + enemyDirX * AIM_ARROW_LENGTH;
                    float enemyEndY = enemyCenterY + enemyDirY * AIM_ARROW_LENGTH;
                    
                    // Draw arrow line
                    DrawLineAA(enemyCenterX, enemyCenterY, enemyEndX, enemyEndY, COLOR_RED, 3.0f);
                    
                    // Draw arrowhead
                    float arrowHeadLen = 10.0f;
                    float arrowHeadAngle = 0.5f;
                    float headX1 = enemyEndX - enemyDirX * arrowHeadLen * cosf(arrowHeadAngle) + enemyDirY * arrowHeadLen * sinf(arrowHeadAngle);
                    float headY1 = enemyEndY - enemyDirY * arrowHeadLen * cosf(arrowHeadAngle) - enemyDirX * arrowHeadLen * sinf(arrowHeadAngle);
                    float headX2 = enemyEndX - enemyDirX * arrowHeadLen * cosf(arrowHeadAngle) - enemyDirY * arrowHeadLen * sinf(arrowHeadAngle);
                    float headY2 = enemyEndY - enemyDirY * arrowHeadLen * cosf(arrowHeadAngle) + enemyDirX * arrowHeadLen * sinf(arrowHeadAngle);
                    
                    DrawLineAA(enemyEndX, enemyEndY, headX1, headY1, COLOR_RED, 3.0f);
                    DrawLineAA(enemyEndX, enemyEndY, headX2, headY2, COLOR_RED, 3.0f);
                }
                
                // Draw aim phase instructions
                DrawFormatString(SCREEN_WIDTH / 2 - 100, TITLE_Y_POSITION, COLOR_BLACK, "Aim Phase");
                DrawFormatString(SCREEN_WIDTH / 2 - 100, TITLE_Y_POSITION + 20, COLOR_BLACK, "Drag from player to aim");
                DrawFormatString(SCREEN_WIDTH / 2 - 100, TITLE_Y_POSITION + 40, COLOR_BLACK, "Release to start battle");
                
            } else if (!battleEnded) {
                // Update hit-stop timer
                bool inHitStop = (hitStopTimer > 0.0f);
                if (inHitStop) {
                    hitStopTimer -= 1.0f;
                }
                
                // Update and move circles (only if not in hit-stop)
                if (!inHitStop) {
                    for (int i = 0; i < 2; i++) {
                        if (!circles[i].isAlive) continue;
                        
                        // Update position
                        circles[i].x += circles[i].vx;
                        circles[i].y += circles[i].vy;
                        
                        // Update rotation
                        circles[i].angle += circles[i].angularVel;
                        
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
                    
                    // Handle player-player collision (only if both alive)
                    if (circles[0].isAlive && circles[1].isAlive) {
                        HandlePlayerCollision(circles[0], circles[1]);
                    }
                    
                    // Check weapon hits (only if both alive)
                    if (circles[0].isAlive && circles[1].isAlive) {
                        // Check if yellow's weapon hits cyan
                        bool yellowHitsCyan = CheckWeaponHit(circles[0], circles[1]);
                        if (yellowHitsCyan) {
                            // Apply damage only if cooldown expired and we had separation
                            if (circles[1].hitCooldown <= 0.0f && !circles[1].wasHitLastFrame) {
                                circles[1].hitTimer = HIT_FEEDBACK_DURATION;
                                bool isCrit = false;
                                int damage = GetTotalWeaponDamage(circles[0], isCrit);
                                circles[1].hp -= damage;
                                if (circles[1].hp <= 0) {
                                    circles[1].isAlive = false;
                                    battleEnded = true;
                                    playerWon = true;
                                }
                                circles[1].hitCooldown = HIT_COOLDOWN_DURATION;
                                hitStopTimer = HIT_STOP_DURATION;
                            }
                            circles[1].wasHitLastFrame = true;
                        } else {
                            circles[1].wasHitLastFrame = false;
                        }
                        
                        // Check if cyan's weapon hits yellow
                        bool cyanHitsYellow = CheckWeaponHit(circles[1], circles[0]);
                        if (cyanHitsYellow) {
                            // Apply damage only if cooldown expired and we had separation
                            if (circles[0].hitCooldown <= 0.0f && !circles[0].wasHitLastFrame) {
                                circles[0].hitTimer = HIT_FEEDBACK_DURATION;
                                bool isCrit = false;
                                int damage = GetTotalWeaponDamage(circles[1], isCrit);
                                circles[0].hp -= damage;
                                if (circles[0].hp <= 0) {
                                    circles[0].isAlive = false;
                                    battleEnded = true;
                                    playerWon = false;
                                }
                                circles[0].hitCooldown = HIT_COOLDOWN_DURATION;
                                hitStopTimer = HIT_STOP_DURATION;
                            }
                            circles[0].wasHitLastFrame = true;
                        } else {
                            circles[0].wasHitLastFrame = false;
                        }
                    }
                    
                    // Handle weapon-weapon collision (only if both alive)
                    if (circles[0].isAlive && circles[1].isAlive) {
                        if (HandleWeaponCollision(circles[0], circles[1])) {
                            hitStopTimer = HIT_STOP_DURATION;
                        }
                    }
                }
                
                // Update hit timer (always, even during hit-stop)
                for (int i = 0; i < 2; i++) {
                    if (circles[i].hitTimer > 0.0f) {
                        circles[i].hitTimer -= 1.0f;
                    }
                    if (circles[i].hitCooldown > 0.0f) {
                        circles[i].hitCooldown -= 1.0f;
                    }
                }
            } else {
                // Battle ended - wait for mouse click to continue
                // Check for mouse click (edge trigger)
                int mouseState = GetMouseInput();
                bool mouseClicked = (mouseState & MOUSE_INPUT_LEFT) && !(prevMouseState & MOUSE_INPUT_LEFT);
                prevMouseState = mouseState;
                
                if (mouseClicked) {
                    if (playerWon) {
                        // Update player character state
                        playerChar.hp = circles[0].hp;
                        playerChar.vx = circles[0].vx;
                        playerChar.vy = circles[0].vy;
                        playerChar.angularVel = circles[0].angularVel;
                        
                        // Check if reached boss (end of run)
                        if (mapNodes[currentNodeIndex].type == NODE_BOSS) {
                            // Boss defeated - transition to name entry screen
                            ResetMapInputState(mapInputCooldown, prevEnterKeyState, prevSpaceKeyState, prevMouseState);
                            currentScene = SCENE_NAME_ENTRY;
                        } else {
                            // Transition to reward selection scene
                            ResetMapInputState(mapInputCooldown, prevEnterKeyState, prevSpaceKeyState, prevMouseState);
                            currentScene = SCENE_REWARD;
                        }
                    } else {
                        // Player lost - go to game over screen
                        gameOverSelectedOption = 0;  // Reset to default (Retry)
                        ResetMapInputState(mapInputCooldown, prevEnterKeyState, prevSpaceKeyState, prevMouseState);
                        currentScene = SCENE_GAME_OVER;
                    }
                }
            }
            
            // Draw circles with hit feedback (only if alive)
            for (int i = 0; i < 2; i++) {
                if (!circles[i].isAlive) continue;
                
                unsigned int drawColor = circles[i].color;
                
                // Apply red flash if hit
                if (circles[i].hitTimer > 0.0f) {
                    drawColor = COLOR_HIT_FEEDBACK;
                }
                
                // Draw circle with outline
                DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, drawColor, TRUE);
                DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, COLOR_BLACK, FALSE);
                
                // Draw HP number inside circle (centered)
                DrawFormatString((int)(circles[i].x + CIRCLE_NUMBER_X_OFFSET), (int)(circles[i].y + CIRCLE_NUMBER_Y_OFFSET), COLOR_BLACK, "%d", circles[i].hp);
            }
            
            // Draw weapons at player positions with rotated offsets (only if alive)
            for (int i = 0; i < 2; i++) {
                if (!circles[i].isAlive) continue;
                
                // Calculate weapon world position
                float weaponWorldX, weaponWorldY;
                GetWeaponWorldPosition(circles[i], weaponWorldX, weaponWorldY);
                
                // Draw weapon using player's angle
                if (circles[i].weapon.type == WEAPON_BOOMERANG) {
                    DrawBoomerang(weaponWorldX, weaponWorldY, circles[i].angle, circles[i].weapon.length, circles[i].weapon.color);
                } else {
                    DrawSpear(weaponWorldX, weaponWorldY, circles[i].angle, circles[i].weapon.length, circles[i].weapon.color);
                }
            }
            
            // Draw dead characters as outlines and K.O. text
            for (int i = 0; i < 2; i++) {
                if (!circles[i].isAlive) {
                    // Draw only outline (transparent interior) for dead characters
                    DrawCircleAA(circles[i].x, circles[i].y, CIRCLE_RADIUS, 32, COLOR_BLACK, FALSE);
                    
                    // Draw K.O. text
                    DrawFormatString((int)(circles[i].x + KO_TEXT_X_OFFSET), (int)(circles[i].y + KO_TEXT_Y_OFFSET), COLOR_BLACK, "K.O.");
                }
            }
            
            // Draw battle result
            if (battleEnded) {
                if (playerWon) {
                    DrawFormatString(SCREEN_WIDTH / 2 - 50, TITLE_Y_POSITION, COLOR_BLACK, "Victory!");
                    DrawFormatString(SCREEN_WIDTH / 2 - 80, TITLE_Y_POSITION + 20, COLOR_BLACK, "Click to continue");
                } else {
                    DrawFormatString(SCREEN_WIDTH / 2 - 50, TITLE_Y_POSITION, COLOR_BLACK, "Defeat!");
                    DrawFormatString(SCREEN_WIDTH / 2 - 80, TITLE_Y_POSITION + 20, COLOR_BLACK, "Click to continue");
                }
            } else {
                // Draw title text at top
                const char* nodeTypeName = GetNodeTypeName(mapNodes[currentNodeIndex].type);
                DrawFormatString(SCREEN_WIDTH / 2 - 50, TITLE_Y_POSITION, COLOR_BLACK, "Battle: %s", nodeTypeName);
            }
        } else if (currentScene == SCENE_REWARD) {
            // ===== REWARD SELECTION SCENE =====
            
            // Reward selection state (persistent across frames in this scene)
            static int selectedReward = 0;  // 0 = HP, 1 = Attack, 2 = Speed, 3 = Crit, 4 = Weapon Length
            static bool rewardSceneInitialized = false;
            static NodeType lastBattleNodeType = NODE_NORMAL;
            
            // Initialize reward scene on first entry
            if (!rewardSceneInitialized) {
                selectedReward = 0;
                lastBattleNodeType = mapNodes[currentNodeIndex].type;
                rewardSceneInitialized = true;
            }
            
            // Calculate reward multiplier (3x for Elite nodes)
            int rewardMultiplier = (lastBattleNodeType == NODE_ELITE) ? 3 : 1;
            
            // Update input cooldown timer
            if (mapInputCooldown > 0.0f) {
                mapInputCooldown -= 1.0f;
            }
            
            // Define reward option rectangles (5 rewards, 2 rows)
            const int REWARD_BOX_WIDTH = 140;
            const int REWARD_BOX_HEIGHT = 90;
            const int REWARD_SPACING = 15;
            const int REWARD_TOP_ROW_Y = 220;
            const int REWARD_BOTTOM_ROW_Y = 330;
            const int REWARD_TOP_START_X = (SCREEN_WIDTH - (REWARD_BOX_WIDTH * 3 + REWARD_SPACING * 2)) / 2;
            const int REWARD_BOTTOM_START_X = (SCREEN_WIDTH - (REWARD_BOX_WIDTH * 2 + REWARD_SPACING)) / 2;
            
            // Handle mouse hover
            int mouseX, mouseY;
            GetMousePoint(&mouseX, &mouseY);
            
            // Check top row (3 rewards)
            for (int i = 0; i < 3; i++) {
                int boxX = REWARD_TOP_START_X + i * (REWARD_BOX_WIDTH + REWARD_SPACING);
                int boxY = REWARD_TOP_ROW_Y;
                
                if (mouseX >= boxX && mouseX <= boxX + REWARD_BOX_WIDTH &&
                    mouseY >= boxY && mouseY <= boxY + REWARD_BOX_HEIGHT) {
                    selectedReward = i;
                }
            }
            
            // Check bottom row (2 rewards)
            for (int i = 0; i < 2; i++) {
                int boxX = REWARD_BOTTOM_START_X + i * (REWARD_BOX_WIDTH + REWARD_SPACING);
                int boxY = REWARD_BOTTOM_ROW_Y;
                
                if (mouseX >= boxX && mouseX <= boxX + REWARD_BOX_WIDTH &&
                    mouseY >= boxY && mouseY <= boxY + REWARD_BOX_HEIGHT) {
                    selectedReward = i + 3;
                }
            }
            
            // Handle mouse click selection only
            int mouseState = GetMouseInput();
            bool mouseClicked = (mouseState & MOUSE_INPUT_LEFT) && !(prevMouseState & MOUSE_INPUT_LEFT);
            
            if (mouseClicked && mapInputCooldown <= 0.0f) {
                // Apply selected reward with multiplier
                if (selectedReward == 0) {
                    // Max HP +10 * multiplier, also heal by the same amount (clamped to new maxHP)
                    int hpIncrease = 10 * rewardMultiplier;
                    playerChar.maxHP += hpIncrease;
                    playerChar.hp = ClampHP(playerChar.hp + hpIncrease, playerChar.maxHP);
                } else if (selectedReward == 1) {
                    // Attack +1 (weapon damage +1) * multiplier
                    playerChar.weaponDamage += 1 * rewardMultiplier;
                } else if (selectedReward == 2) {
                    // Speed +0.1 * multiplier
                    float oldSpeed = playerChar.baseSpeed;
                    playerChar.baseSpeed += 0.1f * rewardMultiplier;
                    // Apply proportionally to current velocity (avoid division by zero)
                    if (oldSpeed > 0.0f) {
                        float speedMult = playerChar.baseSpeed / oldSpeed;
                        playerChar.vx *= speedMult;
                        playerChar.vy *= speedMult;
                    }
                } else if (selectedReward == 3) {
                    // Crit rate +5% * multiplier
                    playerChar.critRate += 0.05f * rewardMultiplier;
                    // Clamp to max 100%
                    if (playerChar.critRate > 1.0f) playerChar.critRate = 1.0f;
                } else if (selectedReward == 4) {
                    // Weapon length +5 * multiplier
                    playerChar.weapon.length += 15.0f * rewardMultiplier;
                }
                
                // Reset reward scene for next time
                rewardSceneInitialized = false;
                
                // Set input cooldown and clear input state before returning to map
                ResetMapInputState(mapInputCooldown, prevEnterKeyState, prevSpaceKeyState, prevMouseState);
                currentScene = SCENE_MAP;
            }
            
            prevMouseState = mouseState;
            
            // Draw reward selection UI
            DrawFormatString(SCREEN_WIDTH / 2 - 80, 150, COLOR_BLACK, "Select Your Reward!");
            if (rewardMultiplier > 1) {
                DrawFormatString(SCREEN_WIDTH / 2 - 90, 180, COLOR_BLACK, "Elite Victory: %dx rewards!", rewardMultiplier);
            }
            
            // Draw top row rewards (0, 1, 2)
            for (int i = 0; i < 3; i++) {
                int boxX = REWARD_TOP_START_X + i * (REWARD_BOX_WIDTH + REWARD_SPACING);
                int boxY = REWARD_TOP_ROW_Y;
                
                // Draw box background
                unsigned int boxColor = (i == selectedReward) ? COLOR_YELLOW : COLOR_WHITE;
                DrawBox(boxX, boxY, boxX + REWARD_BOX_WIDTH, boxY + REWARD_BOX_HEIGHT, boxColor, TRUE);
                DrawBox(boxX, boxY, boxX + REWARD_BOX_WIDTH, boxY + REWARD_BOX_HEIGHT, COLOR_BLACK, FALSE);
                
                // Draw reward text
                if (i == 0) {
                    DrawFormatString(boxX + 10, boxY + 15, COLOR_BLACK, "Max HP +%d", 10 * rewardMultiplier);
                    DrawFormatString(boxX + 5, boxY + 35, COLOR_BLACK, "(+HP healing)");
                    DrawFormatString(boxX + 5, boxY + 55, COLOR_BLACK, "Now: %d", playerChar.maxHP);
                } else if (i == 1) {
                    DrawFormatString(boxX + 15, boxY + 15, COLOR_BLACK, "Attack +%d", 1 * rewardMultiplier);
                    DrawFormatString(boxX + 5, boxY + 35, COLOR_BLACK, "(Weapon dmg)");
                    DrawFormatString(boxX + 5, boxY + 55, COLOR_BLACK, "Now: %d", GetTotalWeaponDamage(playerChar));
                } else if (i == 2) {
                    DrawFormatString(boxX + 15, boxY + 15, COLOR_BLACK, "Speed +%.1f", 0.1f * rewardMultiplier);
                    DrawFormatString(boxX + 5, boxY + 35, COLOR_BLACK, "(Movement)");
                    DrawFormatString(boxX + 5, boxY + 55, COLOR_BLACK, "Now: %.1f", playerChar.baseSpeed);
                }
            }
            
            // Draw bottom row rewards (3, 4)
            for (int i = 0; i < 2; i++) {
                int boxX = REWARD_BOTTOM_START_X + i * (REWARD_BOX_WIDTH + REWARD_SPACING);
                int boxY = REWARD_BOTTOM_ROW_Y;
                
                // Draw box background
                unsigned int boxColor = (i + 3 == selectedReward) ? COLOR_YELLOW : COLOR_WHITE;
                DrawBox(boxX, boxY, boxX + REWARD_BOX_WIDTH, boxY + REWARD_BOX_HEIGHT, boxColor, TRUE);
                DrawBox(boxX, boxY, boxX + REWARD_BOX_WIDTH, boxY + REWARD_BOX_HEIGHT, COLOR_BLACK, FALSE);
                
                // Draw reward text
                if (i == 0) {
                    // Crit rate reward
                    DrawFormatString(boxX + 10, boxY + 15, COLOR_BLACK, "Crit +%d%%", (int)(5 * rewardMultiplier));
                    DrawFormatString(boxX + 5, boxY + 35, COLOR_BLACK, "(1.5x dmg)");
                    DrawFormatString(boxX + 5, boxY + 55, COLOR_BLACK, "Now: %d%%", (int)(playerChar.critRate * 100));
                } else if (i == 1) {
                    // Weapon length reward
                    DrawFormatString(boxX + 5, boxY + 15, COLOR_BLACK, "Weapon +%d", (int)(5 * rewardMultiplier));
                    DrawFormatString(boxX + 5, boxY + 35, COLOR_BLACK, "(Length)");
                    DrawFormatString(boxX + 5, boxY + 55, COLOR_BLACK, "Now: %.0f", playerChar.weapon.length);
                }
            }
            
            // Draw instructions
            DrawFormatString(SCREEN_WIDTH / 2 - 80, 450, COLOR_BLACK, "Mouse: Click on reward");
        } else if (currentScene == SCENE_NAME_ENTRY) {
            // ===== NAME ENTRY SCENE =====
            
            // Name entry state (persistent across frames in this scene)
            static std::string enteredName = "";
            static CharSetMode charSetMode = CHARSET_HIRAGANA;
            static bool nameEntryInitialized = false;
            
            // Initialize name entry scene on first entry
            if (!nameEntryInitialized) {
                enteredName = "";
                charSetMode = CHARSET_HIRAGANA;
                nameEntryInitialized = true;
            }
            
            // Update input cooldown timer
            if (mapInputCooldown > 0.0f) {
                mapInputCooldown -= 1.0f;
            }
            
            // Get current character set
            std::vector<std::string> charSet = GetCharacterSet(charSetMode);
            
            // Handle mouse input
            int mouseX, mouseY;
            GetMousePoint(&mouseX, &mouseY);
            int mouseState = GetMouseInput();
            bool mouseClicked = (mouseState & MOUSE_INPUT_LEFT) && !(prevMouseState & MOUSE_INPUT_LEFT);
            
            // Calculate keyboard layout
            const int KEYBOARD_START_X = (SCREEN_WIDTH - (NAME_ENTRY_KEYS_PER_ROW * (NAME_ENTRY_KEY_WIDTH + NAME_ENTRY_KEY_SPACING) - NAME_ENTRY_KEY_SPACING)) / 2;
            
            // Handle character key clicks
            if (mouseClicked && mapInputCooldown <= 0.0f) {
                // Check keyboard keys
                for (size_t i = 0; i < charSet.size(); i++) {
                    int row = i / NAME_ENTRY_KEYS_PER_ROW;
                    int col = i % NAME_ENTRY_KEYS_PER_ROW;
                    int keyX = KEYBOARD_START_X + col * (NAME_ENTRY_KEY_WIDTH + NAME_ENTRY_KEY_SPACING);
                    int keyY = NAME_ENTRY_KEYBOARD_START_Y + row * (NAME_ENTRY_KEY_HEIGHT + NAME_ENTRY_KEY_SPACING);
                    
                    if (mouseX >= keyX && mouseX <= keyX + NAME_ENTRY_KEY_WIDTH &&
                        mouseY >= keyY && mouseY <= keyY + NAME_ENTRY_KEY_HEIGHT) {
                        // Add character if not at max length
                        if ((int)enteredName.length() < NAME_MAX_LENGTH) {
                            enteredName += charSet[i];
                        }
                    }
                }
                
                // Check mode switch buttons
                const int MODE_BUTTON_START_X = (SCREEN_WIDTH - (4 * NAME_ENTRY_MODE_BUTTON_WIDTH + 3 * NAME_ENTRY_KEY_SPACING)) / 2;
                
                // Hiragana button
                int hiraX = MODE_BUTTON_START_X;
                if (mouseX >= hiraX && mouseX <= hiraX + NAME_ENTRY_MODE_BUTTON_WIDTH &&
                    mouseY >= NAME_ENTRY_MODE_BUTTON_Y && mouseY <= NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT) {
                    charSetMode = CHARSET_HIRAGANA;
                }
                
                // Katakana button
                int kataX = hiraX + NAME_ENTRY_MODE_BUTTON_WIDTH + NAME_ENTRY_KEY_SPACING;
                if (mouseX >= kataX && mouseX <= kataX + NAME_ENTRY_MODE_BUTTON_WIDTH &&
                    mouseY >= NAME_ENTRY_MODE_BUTTON_Y && mouseY <= NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT) {
                    charSetMode = CHARSET_KATAKANA;
                }
                
                // English button
                int engX = kataX + NAME_ENTRY_MODE_BUTTON_WIDTH + NAME_ENTRY_KEY_SPACING;
                if (mouseX >= engX && mouseX <= engX + NAME_ENTRY_MODE_BUTTON_WIDTH &&
                    mouseY >= NAME_ENTRY_MODE_BUTTON_Y && mouseY <= NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT) {
                    charSetMode = CHARSET_ENGLISH;
                }
                
                // Symbols button
                int symX = engX + NAME_ENTRY_MODE_BUTTON_WIDTH + NAME_ENTRY_KEY_SPACING;
                if (mouseX >= symX && mouseX <= symX + NAME_ENTRY_MODE_BUTTON_WIDTH &&
                    mouseY >= NAME_ENTRY_MODE_BUTTON_Y && mouseY <= NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT) {
                    charSetMode = CHARSET_SYMBOLS;
                }
                
                // Check control buttons
                const int CONTROL_BUTTON_START_X = (SCREEN_WIDTH - (3 * NAME_ENTRY_CONTROL_BUTTON_WIDTH + 2 * 15)) / 2;
                
                // Backspace button
                int backX = CONTROL_BUTTON_START_X;
                if (mouseX >= backX && mouseX <= backX + NAME_ENTRY_CONTROL_BUTTON_WIDTH &&
                    mouseY >= NAME_ENTRY_CONTROL_BUTTON_Y && mouseY <= NAME_ENTRY_CONTROL_BUTTON_Y + NAME_ENTRY_CONTROL_BUTTON_HEIGHT) {
                    RemoveLastCharacterUTF8(enteredName);
                }
                
                // Clear button
                int clearX = backX + NAME_ENTRY_CONTROL_BUTTON_WIDTH + 15;
                if (mouseX >= clearX && mouseX <= clearX + NAME_ENTRY_CONTROL_BUTTON_WIDTH &&
                    mouseY >= NAME_ENTRY_CONTROL_BUTTON_Y && mouseY <= NAME_ENTRY_CONTROL_BUTTON_Y + NAME_ENTRY_CONTROL_BUTTON_HEIGHT) {
                    enteredName = "";
                }
                
                // OK button
                int okX = clearX + NAME_ENTRY_CONTROL_BUTTON_WIDTH + 15;
                if (mouseX >= okX && mouseX <= okX + NAME_ENTRY_CONTROL_BUTTON_WIDTH &&
                    mouseY >= NAME_ENTRY_CONTROL_BUTTON_Y && mouseY <= NAME_ENTRY_CONTROL_BUTTON_Y + NAME_ENTRY_CONTROL_BUTTON_HEIGHT) {
                    // Save boss with name and return to title
                    if (enteredName.empty()) {
                        playerChar.name = "Boss";
                    } else {
                        playerChar.name = enteredName;
                    }
                    SaveBossToJSON(playerChar);
                    
                    // Reset for next run
                    mapNodes = GenerateMap();
                    currentNodeIndex = 0;
                    highlightedNodeIndex = 0;
                    mapNodes[0].visited = true;
                    for (int connectedIdx : mapNodes[0].connectedNodes) {
                        mapNodes[connectedIdx].reachable = true;
                        mapNodes[connectedIdx].parentNodeIndex = 0;
                    }
                    
                    // Reset player
                    playerChar.hp = MAX_HP;
                    playerChar.maxHP = MAX_HP;
                    playerChar.weaponDamage = 0;
                    playerChar.baseSpeed = 1.0f;
                    playerChar.critRate = 0.0f;
                    
                    // Reset name entry state for next time
                    nameEntryInitialized = false;
                    
                    // Return to title
                    currentScene = SCENE_TITLE;
                }
            }
            
            // Handle physical keyboard input as fallback (for English characters)
            static bool prevBackspaceKey = false;
            bool backspacePressed = DetectKeyPressEdge(KEY_INPUT_BACK, prevBackspaceKey);
            if (backspacePressed) {
                RemoveLastCharacterUTF8(enteredName);
            }
            
            // Check for alphanumeric keys (A-Z, 0-9)
            if ((int)enteredName.length() < NAME_MAX_LENGTH) {
                for (int key = KEY_INPUT_A; key <= KEY_INPUT_Z; key++) {
                    static bool prevKeyStates[26] = {false};
                    int idx = key - KEY_INPUT_A;
                    if (DetectKeyPressEdge(key, prevKeyStates[idx])) {
                        char c = 'A' + idx;
                        enteredName += c;
                        break;
                    }
                }
                for (int key = KEY_INPUT_0; key <= KEY_INPUT_9; key++) {
                    static bool prevNumStates[10] = {false};
                    int idx = key - KEY_INPUT_0;
                    if (DetectKeyPressEdge(key, prevNumStates[idx])) {
                        char c = '0' + idx;
                        enteredName += c;
                        break;
                    }
                }
            }
            
            prevMouseState = mouseState;
            
            // Draw UI
            DrawFormatString(SCREEN_WIDTH / 2 - 100, 100, COLOR_BLACK, "Congratulations!");
            DrawFormatString(SCREEN_WIDTH / 2 - 140, 140, COLOR_BLACK, "名前を入力してください！");
            
            // Draw input box
            const int INPUT_BOX_X = (SCREEN_WIDTH - NAME_ENTRY_INPUT_BOX_WIDTH) / 2;
            DrawBox(INPUT_BOX_X, NAME_ENTRY_INPUT_BOX_Y, 
                    INPUT_BOX_X + NAME_ENTRY_INPUT_BOX_WIDTH, 
                    NAME_ENTRY_INPUT_BOX_Y + NAME_ENTRY_INPUT_BOX_HEIGHT, 
                    COLOR_WHITE, TRUE);
            DrawBox(INPUT_BOX_X, NAME_ENTRY_INPUT_BOX_Y, 
                    INPUT_BOX_X + NAME_ENTRY_INPUT_BOX_WIDTH, 
                    NAME_ENTRY_INPUT_BOX_Y + NAME_ENTRY_INPUT_BOX_HEIGHT, 
                    COLOR_BLACK, FALSE);
            DrawFormatString(INPUT_BOX_X + 10, NAME_ENTRY_INPUT_BOX_Y + 10, COLOR_BLACK, "%s", enteredName.c_str());
            
            // Draw mode buttons
            const int MODE_BUTTON_START_X = (SCREEN_WIDTH - (4 * NAME_ENTRY_MODE_BUTTON_WIDTH + 3 * NAME_ENTRY_KEY_SPACING)) / 2;
            
            // Hiragana
            int hiraX = MODE_BUTTON_START_X;
            unsigned int hiraColor = (charSetMode == CHARSET_HIRAGANA) ? COLOR_YELLOW : COLOR_WHITE;
            DrawBox(hiraX, NAME_ENTRY_MODE_BUTTON_Y, 
                    hiraX + NAME_ENTRY_MODE_BUTTON_WIDTH, 
                    NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT, 
                    hiraColor, TRUE);
            DrawBox(hiraX, NAME_ENTRY_MODE_BUTTON_Y, 
                    hiraX + NAME_ENTRY_MODE_BUTTON_WIDTH, 
                    NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT, 
                    COLOR_BLACK, FALSE);
            DrawFormatString(hiraX + 8, NAME_ENTRY_MODE_BUTTON_Y + 10, COLOR_BLACK, "ひらがな");
            
            // Katakana
            int kataX = hiraX + NAME_ENTRY_MODE_BUTTON_WIDTH + NAME_ENTRY_KEY_SPACING;
            unsigned int kataColor = (charSetMode == CHARSET_KATAKANA) ? COLOR_YELLOW : COLOR_WHITE;
            DrawBox(kataX, NAME_ENTRY_MODE_BUTTON_Y, 
                    kataX + NAME_ENTRY_MODE_BUTTON_WIDTH, 
                    NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT, 
                    kataColor, TRUE);
            DrawBox(kataX, NAME_ENTRY_MODE_BUTTON_Y, 
                    kataX + NAME_ENTRY_MODE_BUTTON_WIDTH, 
                    NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT, 
                    COLOR_BLACK, FALSE);
            DrawFormatString(kataX + 8, NAME_ENTRY_MODE_BUTTON_Y + 10, COLOR_BLACK, "カタカナ");
            
            // English
            int engX = kataX + NAME_ENTRY_MODE_BUTTON_WIDTH + NAME_ENTRY_KEY_SPACING;
            unsigned int engColor = (charSetMode == CHARSET_ENGLISH) ? COLOR_YELLOW : COLOR_WHITE;
            DrawBox(engX, NAME_ENTRY_MODE_BUTTON_Y, 
                    engX + NAME_ENTRY_MODE_BUTTON_WIDTH, 
                    NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT, 
                    engColor, TRUE);
            DrawBox(engX, NAME_ENTRY_MODE_BUTTON_Y, 
                    engX + NAME_ENTRY_MODE_BUTTON_WIDTH, 
                    NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT, 
                    COLOR_BLACK, FALSE);
            DrawFormatString(engX + 20, NAME_ENTRY_MODE_BUTTON_Y + 10, COLOR_BLACK, "英語");
            
            // Symbols
            int symX = engX + NAME_ENTRY_MODE_BUTTON_WIDTH + NAME_ENTRY_KEY_SPACING;
            unsigned int symColor = (charSetMode == CHARSET_SYMBOLS) ? COLOR_YELLOW : COLOR_WHITE;
            DrawBox(symX, NAME_ENTRY_MODE_BUTTON_Y, 
                    symX + NAME_ENTRY_MODE_BUTTON_WIDTH, 
                    NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT, 
                    symColor, TRUE);
            DrawBox(symX, NAME_ENTRY_MODE_BUTTON_Y, 
                    symX + NAME_ENTRY_MODE_BUTTON_WIDTH, 
                    NAME_ENTRY_MODE_BUTTON_Y + NAME_ENTRY_MODE_BUTTON_HEIGHT, 
                    COLOR_BLACK, FALSE);
            DrawFormatString(symX + 20, NAME_ENTRY_MODE_BUTTON_Y + 10, COLOR_BLACK, "記号");
            
            // Draw keyboard
            for (size_t i = 0; i < charSet.size(); i++) {
                int row = i / NAME_ENTRY_KEYS_PER_ROW;
                int col = i % NAME_ENTRY_KEYS_PER_ROW;
                int keyX = KEYBOARD_START_X + col * (NAME_ENTRY_KEY_WIDTH + NAME_ENTRY_KEY_SPACING);
                int keyY = NAME_ENTRY_KEYBOARD_START_Y + row * (NAME_ENTRY_KEY_HEIGHT + NAME_ENTRY_KEY_SPACING);
                
                // Draw key
                DrawBox(keyX, keyY, keyX + NAME_ENTRY_KEY_WIDTH, keyY + NAME_ENTRY_KEY_HEIGHT, COLOR_WHITE, TRUE);
                DrawBox(keyX, keyY, keyX + NAME_ENTRY_KEY_WIDTH, keyY + NAME_ENTRY_KEY_HEIGHT, COLOR_BLACK, FALSE);
                DrawFormatString(keyX + 12, keyY + 12, COLOR_BLACK, "%s", charSet[i].c_str());
            }
            
            // Draw control buttons
            const int CONTROL_BUTTON_START_X = (SCREEN_WIDTH - (3 * NAME_ENTRY_CONTROL_BUTTON_WIDTH + 2 * 15)) / 2;
            
            // Backspace
            int backX = CONTROL_BUTTON_START_X;
            DrawBox(backX, NAME_ENTRY_CONTROL_BUTTON_Y, 
                    backX + NAME_ENTRY_CONTROL_BUTTON_WIDTH, 
                    NAME_ENTRY_CONTROL_BUTTON_Y + NAME_ENTRY_CONTROL_BUTTON_HEIGHT, 
                    COLOR_WHITE, TRUE);
            DrawBox(backX, NAME_ENTRY_CONTROL_BUTTON_Y, 
                    backX + NAME_ENTRY_CONTROL_BUTTON_WIDTH, 
                    NAME_ENTRY_CONTROL_BUTTON_Y + NAME_ENTRY_CONTROL_BUTTON_HEIGHT, 
                    COLOR_BLACK, FALSE);
            DrawFormatString(backX + 15, NAME_ENTRY_CONTROL_BUTTON_Y + 12, COLOR_BLACK, "削除");
            
            // Clear
            int clearX = backX + NAME_ENTRY_CONTROL_BUTTON_WIDTH + 15;
            DrawBox(clearX, NAME_ENTRY_CONTROL_BUTTON_Y, 
                    clearX + NAME_ENTRY_CONTROL_BUTTON_WIDTH, 
                    NAME_ENTRY_CONTROL_BUTTON_Y + NAME_ENTRY_CONTROL_BUTTON_HEIGHT, 
                    COLOR_WHITE, TRUE);
            DrawBox(clearX, NAME_ENTRY_CONTROL_BUTTON_Y, 
                    clearX + NAME_ENTRY_CONTROL_BUTTON_WIDTH, 
                    NAME_ENTRY_CONTROL_BUTTON_Y + NAME_ENTRY_CONTROL_BUTTON_HEIGHT, 
                    COLOR_BLACK, FALSE);
            DrawFormatString(clearX + 10, NAME_ENTRY_CONTROL_BUTTON_Y + 12, COLOR_BLACK, "クリア");
            
            // OK
            int okX = clearX + NAME_ENTRY_CONTROL_BUTTON_WIDTH + 15;
            DrawBox(okX, NAME_ENTRY_CONTROL_BUTTON_Y, 
                    okX + NAME_ENTRY_CONTROL_BUTTON_WIDTH, 
                    NAME_ENTRY_CONTROL_BUTTON_Y + NAME_ENTRY_CONTROL_BUTTON_HEIGHT, 
                    COLOR_GREEN, TRUE);
            DrawBox(okX, NAME_ENTRY_CONTROL_BUTTON_Y, 
                    okX + NAME_ENTRY_CONTROL_BUTTON_WIDTH, 
                    NAME_ENTRY_CONTROL_BUTTON_Y + NAME_ENTRY_CONTROL_BUTTON_HEIGHT, 
                    COLOR_BLACK, FALSE);
            DrawFormatString(okX + 30, NAME_ENTRY_CONTROL_BUTTON_Y + 12, COLOR_BLACK, "OK");
        }
        
        ScreenFlip();
    }
    
    DxLib_End();
    return 0;
}
