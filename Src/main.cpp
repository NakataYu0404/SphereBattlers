#include <DxLib.h>

// windows.h �� min/max �}�N�����`���� std::min/std::max �ƏՓ˂���̂�h��
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <cmath>
#include <vector>
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
const unsigned int COLOR_HIT_FEEDBACK = 0xFF6666;
const float HIT_FEEDBACK_DURATION = 10.0f;
const int MAX_HP = 100;
const int WEAPON_DAMAGE = 10;
const float HIT_STOP_DURATION = 20.0f;  // ~100ms at 60fps (frames)
const float WEAPON_COLLISION_THRESHOLD = 5.0f;  // Distance threshold for weapon-weapon collision
const float WEAPON_BOUNCE_DAMPING = 0.8f;  // Damping factor for weapon bounce
const float HIT_COOLDOWN_DURATION = 24.0f;  // ~0.4s at 60fps (frames) - cooldown between hits
const float MAP_INPUT_COOLDOWN_DURATION = 10.0f;  // ~0.167s at 60fps (frames) - cooldown after battle return

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
const int MAP_ROWS = 7;
const int MAP_MIN_NODES_PER_ROW = 2;
const int MAP_MAX_NODES_PER_ROW = 4;
const float MAP_NODE_RADIUS = 20.0f;
const float MAP_LEFT = 100.0f;
const float MAP_RIGHT = 500.0f;
const float MAP_TOP = 100.0f;
const float MAP_BOTTOM = 650.0f;
const int NODE_PROB_NORMAL = 60;    // 60% normal nodes
const int NODE_PROB_ELITE = 70;     // 10% elite nodes (60-70)
const int NODE_PROB_EVENT = 80;     // 10% event nodes (70-80)
const int NODE_PROB_SHOP = 90;      // 10% shop nodes (80-90)
// Remaining 10% are rest nodes (90-100)
const unsigned int COLOR_RED = 0xFF0000;
const unsigned int COLOR_GREEN = 0x00FF00;
const unsigned int COLOR_BLUE = 0x0000FF;
const unsigned int COLOR_PURPLE = 0x800080;
const unsigned int COLOR_ORANGE = 0xFFA500;
const unsigned int COLOR_GRAY = 0x808080;
const unsigned int COLOR_WHITE = 0xFFFFFF;
const char* BOSS_SAVE_FILE = "boss_save.json";

// Scene enum
enum Scene {
    SCENE_MAP,
    SCENE_BATTLE
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
    int hp;             // Hit points (100 max)
    bool isAlive;       // Whether player is alive
    float hitCooldown;  // Cooldown timer before can be hit again (counts down)
    bool wasHitLastFrame;  // Track if was being hit last frame (for separation detection)
};

// Helper function to clamp HP to valid range
int ClampHP(int hp) {
    return (hp > MAX_HP) ? MAX_HP : ((hp < 0) ? 0 : hp);
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
void ResetMapInputState(float& cooldown, bool& prevEnter, bool& prevSpace) {
    cooldown = MAP_INPUT_COOLDOWN_DURATION;
    prevEnter = true;   // Mark as pressed to prevent immediate re-trigger
    prevSpace = true;
}

// Boss save/load functions
void SaveBossToJSON(const Circle& player) {
    try {
        json j;
        j["hp"] = ClampHP(player.hp);
        j["maxHP"] = MAX_HP;
        j["vx"] = player.vx;
        j["vy"] = player.vy;
        j["angularVel"] = player.angularVel;
        j["color"] = player.color;
        j["radius"] = CIRCLE_RADIUS;
        j["weaponType"] = (int)player.weapon.type;
        j["weaponOffsetX"] = player.weapon.offsetX;
        j["weaponOffsetY"] = player.weapon.offsetY;
        j["weaponLength"] = player.weapon.length;
        j["weaponColor"] = player.weapon.color;
        
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
        
        // Load boss parameters with clamping and validation
        int loadedHP = j.value("hp", MAX_HP);
        boss.hp = ClampHP(loadedHP);
        boss.vx = j.value("vx", -2.0f);
        boss.vy = j.value("vy", -3.5f);
        boss.angularVel = j.value("angularVel", -0.025f);
        boss.color = j.value("color", COLOR_CYAN);
        
        // Validate weapon type enum
        int weaponTypeInt = j.value("weaponType", (int)WEAPON_SPEAR);
        if (weaponTypeInt < WEAPON_BOOMERANG || weaponTypeInt > WEAPON_SPEAR) {
            weaponTypeInt = WEAPON_SPEAR;  // Default to spear if invalid
        }
        boss.weapon.type = (WeaponType)weaponTypeInt;
        
        boss.weapon.offsetX = j.value("weaponOffsetX", 45.0f);
        boss.weapon.offsetY = j.value("weaponOffsetY", 0.0f);
        boss.weapon.length = j.value("weaponLength", 30.0f);
        boss.weapon.color = j.value("weaponColor", COLOR_CYAN);
        
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
        case NODE_REST: return COLOR_PURPLE;
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
        c1.angularVel = -c1.angularVel * WEAPON_BOUNCE_DAMPING;
        c2.angularVel = -c2.angularVel * WEAPON_BOUNCE_DAMPING;
        
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
    rowNodes[0].push_back(nodeIndex++);
    nodes.push_back(startNode);
    
    // Rows 1-5: Mixed nodes (normal, elite, event, shop, rest)
    for (int row = 1; row < MAP_ROWS - 1; row++) {
        // Random nodes per row based on constants
        std::uniform_int_distribution<> nodeDist(MAP_MIN_NODES_PER_ROW, MAP_MAX_NODES_PER_ROW);
        int numNodes = nodeDist(gen);
        
        for (int col = 0; col < numNodes; col++) {
            MapNode node;
            node.row = row;
            node.col = col;
            node.visited = false;
            node.reachable = false;
            
            // Node type distribution using probability constants
            std::uniform_int_distribution<> typeDist(0, 99);
            int typeRoll = typeDist(gen);
            if (typeRoll < NODE_PROB_NORMAL) {
                node.type = NODE_NORMAL;
            } else if (typeRoll < NODE_PROB_ELITE) {
                node.type = NODE_ELITE;
            } else if (typeRoll < NODE_PROB_EVENT) {
                node.type = NODE_EVENT;
            } else if (typeRoll < NODE_PROB_SHOP) {
                node.type = NODE_SHOP;
            } else {
                node.type = NODE_REST;
            }
            
            rowNodes[row].push_back(nodeIndex++);
            nodes.push_back(node);
        }
    }
    
    // Row 6 (top): Boss node (single)
    MapNode bossNode;
    bossNode.row = MAP_ROWS - 1;
    bossNode.col = 0;
    bossNode.type = NODE_BOSS;
    bossNode.visited = false;
    bossNode.reachable = false;
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

// Draw map scene
void DrawMap(const std::vector<MapNode>& nodes, int currentNodeIndex, int highlightedNodeIndex) {
    // Draw title
    DrawFormatString(SCREEN_WIDTH / 2 - 50, 20, COLOR_BLACK, "Map - Select Path");
    
    // Draw edges first (behind nodes)
    for (size_t i = 0; i < nodes.size(); i++) {
        for (int connectedIdx : nodes[i].connectedNodes) {
            unsigned int edgeColor = COLOR_GRAY;
            
            // Highlight edges from current node
            if ((int)i == currentNodeIndex) {
                edgeColor = COLOR_BLACK;
            }
            
            DrawLineAA(nodes[i].x, nodes[i].y, 
                      nodes[connectedIdx].x, nodes[connectedIdx].y, 
                      edgeColor, 2.0f);
        }
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
            DrawCircleAA(nodes[i].x, nodes[i].y, MAP_NODE_RADIUS + 5, 32, COLOR_BLACK, FALSE);
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
    DrawFormatString(10, SCREEN_HEIGHT - 60, COLOR_BLACK, "Mouse: Click | Keys: Arrow/WASD + Enter/Space");
    DrawFormatString(10, SCREEN_HEIGHT - 40, COLOR_BLACK, "ESC: Quit");
}

// Update reachability after selecting a node
void UpdateReachability(std::vector<MapNode>& mapNodes, int currentNodeIndex) {
    // Clear all reachability
    for (size_t i = 0; i < mapNodes.size(); i++) {
        mapNodes[i].reachable = false;
    }
    // Set connected nodes as reachable
    for (int connectedIdx : mapNodes[currentNodeIndex].connectedNodes) {
        mapNodes[connectedIdx].reachable = true;
    }
}

// Initialize battle characters
void InitializeBattle(Circle& player, Circle& enemy, const Circle& playerChar, 
                      NodeType nodeType, const std::vector<MapNode>& mapNodes, int currentNodeIndex) {
    // Initialize player
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
    enemy.angle = 0.0f;
    enemy.number = 91;
    enemy.hitTimer = 0.0f;
    enemy.isAlive = true;
    enemy.hitCooldown = 0.0f;
    enemy.wasHitLastFrame = false;
    
    // Set enemy parameters based on node type
    if (nodeType == NODE_BOSS) {
        // Try to load boss from JSON
        if (!LoadBossFromJSON(enemy)) {
            // Default boss
            enemy.vx = -2.0f;
            enemy.vy = -3.5f;
            enemy.angularVel = -0.025f;
            enemy.color = COLOR_CYAN;
            enemy.hp = MAX_HP;
            enemy.weapon.type = WEAPON_SPEAR;
            enemy.weapon.offsetX = 45.0f;
            enemy.weapon.offsetY = 0.0f;
            enemy.weapon.length = 30.0f;
            enemy.weapon.color = COLOR_CYAN;
        }
    } else {
        // Regular/elite enemy (for now, same as default)
        enemy.vx = -2.0f;
        enemy.vy = -3.5f;
        enemy.angularVel = -0.025f;
        enemy.color = COLOR_CYAN;
        enemy.hp = 10;
        enemy.weapon.type = WEAPON_SPEAR;
        enemy.weapon.offsetX = 45.0f;
        enemy.weapon.offsetY = 0.0f;
        enemy.weapon.length = 30.0f;
        enemy.weapon.color = COLOR_CYAN;
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
    Scene currentScene = SCENE_MAP;
    
    // Generate map
    std::vector<MapNode> mapNodes = GenerateMap();
    int currentNodeIndex = 0;  // Start at first node
    int highlightedNodeIndex = 0;
    mapNodes[0].visited = true;
    
    // Update reachability from start
    for (int connectedIdx : mapNodes[0].connectedNodes) {
        mapNodes[connectedIdx].reachable = true;
    }
    
    // Battle state
    Circle circles[2];
    float hitStopTimer = 0.0f;
    bool battleEnded = false;
    bool playerWon = false;
    
    // Player character (persistent across battles)
    Circle playerChar;
    playerChar.x = FRAME_LEFT + FRAME_WIDTH * 0.3f;
    playerChar.y = FRAME_BOTTOM - 80.0f;
    playerChar.vx = 2.5f;
    playerChar.vy = -3.0f;
    playerChar.angle = 0.0f;
    playerChar.angularVel = 0.03f;
    playerChar.number = 92;
    playerChar.color = COLOR_YELLOW;
    playerChar.hp = MAX_HP;
    playerChar.weapon.type = WEAPON_SPEAR;
    playerChar.weapon.offsetX = 40.0f;
    playerChar.weapon.offsetY = 0.0f;
    playerChar.weapon.length = 25.0f;
    playerChar.weapon.color = COLOR_YELLOW;
    
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
    
    // Main loop
    while (ProcessMessage() == 0) {
        // Check for ESC key to exit
        if (CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }
        
        // Clear screen with beige background
        ClearDrawScreen();
        DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BEIGE, TRUE);
        
        if (currentScene == SCENE_MAP) {
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
            
            // Handle keyboard navigation
            if (keyPressDelay > 0) {
                keyPressDelay--;
            }
            
            if (keyPressDelay == 0) {
                // Find reachable nodes
                std::vector<int> reachableIndices;
                for (size_t i = 0; i < mapNodes.size(); i++) {
                    if (mapNodes[i].reachable && !mapNodes[i].visited) {
                        reachableIndices.push_back((int)i);
                    }
                }
                
                if (!reachableIndices.empty()) {
                    // If no node highlighted, start with first reachable
                    if (highlightedNodeIndex == -1 || !mapNodes[highlightedNodeIndex].reachable || mapNodes[highlightedNodeIndex].visited) {
                        highlightedNodeIndex = reachableIndices[0];
                    }
                    
                    // Find current position in reachable list
                    int currentPosInReachable = 0;
                    for (size_t i = 0; i < reachableIndices.size(); i++) {
                        if (reachableIndices[i] == highlightedNodeIndex) {
                            currentPosInReachable = (int)i;
                            break;
                        }
                    }
                    
                    // Arrow keys or WASD to navigate
                    bool moveLeft = CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_A);
                    bool moveRight = CheckHitKey(KEY_INPUT_RIGHT) || CheckHitKey(KEY_INPUT_D);
                    bool moveUp = CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W);
                    bool moveDown = CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S);
                    
                    if (moveLeft || moveUp) {
                        currentPosInReachable = (currentPosInReachable - 1 + (int)reachableIndices.size()) % (int)reachableIndices.size();
                        highlightedNodeIndex = reachableIndices[currentPosInReachable];
                        keyPressDelay = KEY_PRESS_COOLDOWN;
                    } else if (moveRight || moveDown) {
                        currentPosInReachable = (currentPosInReachable + 1) % (int)reachableIndices.size();
                        highlightedNodeIndex = reachableIndices[currentPosInReachable];
                        keyPressDelay = KEY_PRESS_COOLDOWN;
                    }
                    
                    // Enter or Space to select (with edge detection and cooldown)
                    bool enterPressed = DetectKeyPressEdge(KEY_INPUT_RETURN, prevEnterKeyState);
                    bool spacePressed = DetectKeyPressEdge(KEY_INPUT_SPACE, prevSpaceKeyState);
                    
                    if ((enterPressed || spacePressed) && highlightedNodeIndex >= 0 && mapInputCooldown <= 0.0f) {
                        // Select this node
                        currentNodeIndex = highlightedNodeIndex;
                        mapNodes[currentNodeIndex].visited = true;
                        
                        // Update reachability
                        UpdateReachability(mapNodes, currentNodeIndex);
                        
                        // Enter battle scene based on node type
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
                        } else {
                            // Other node types (event, shop, rest) - just return to map for now
                            // In a full implementation, these would have their own scenes
                            keyPressDelay = KEY_PRESS_COOLDOWN * 2;
                        }
                    }
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
            
            if (!battleEnded) {
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
                                circles[1].hp -= WEAPON_DAMAGE;
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
                                circles[0].hp -= WEAPON_DAMAGE;
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
                // Battle ended - wait for input to return to map
                bool enterPressed = DetectKeyPressEdge(KEY_INPUT_RETURN, prevEnterKeyState);
                bool spacePressed = DetectKeyPressEdge(KEY_INPUT_SPACE, prevSpaceKeyState);
                
                if (enterPressed || spacePressed) {
                    if (playerWon) {
                        // Update player character state
                        playerChar.hp = circles[0].hp;
                        playerChar.vx = circles[0].vx;
                        playerChar.vy = circles[0].vy;
                        playerChar.angularVel = circles[0].angularVel;
                        
                        // If defeated boss, save to JSON
                        if (mapNodes[currentNodeIndex].type == NODE_BOSS) {
                            SaveBossToJSON(playerChar);
                        }
                        
                        // Check if reached boss (end of run)
                        if (mapNodes[currentNodeIndex].type == NODE_BOSS) {
                            // Run completed - could show victory screen or restart
                            // For now, just generate a new map
                            mapNodes = GenerateMap();
                            currentNodeIndex = 0;
                            highlightedNodeIndex = 0;
                            mapNodes[0].visited = true;
                            for (int connectedIdx : mapNodes[0].connectedNodes) {
                                mapNodes[connectedIdx].reachable = true;
                            }
                            
                            // Reset player
                            playerChar.hp = MAX_HP;
                        }
                        
                        // Set input cooldown and clear input state before returning to map
                        ResetMapInputState(mapInputCooldown, prevEnterKeyState, prevSpaceKeyState);
                        currentScene = SCENE_MAP;
                    } else {
                        // Player lost - restart from beginning
                        mapNodes = GenerateMap();
                        currentNodeIndex = 0;
                        highlightedNodeIndex = 0;
                        mapNodes[0].visited = true;
                        for (int connectedIdx : mapNodes[0].connectedNodes) {
                            mapNodes[connectedIdx].reachable = true;
                        }
                        
                        // Reset player
                        playerChar.hp = MAX_HP;
                        
                        // Set input cooldown and clear input state before returning to map
                        ResetMapInputState(mapInputCooldown, prevEnterKeyState, prevSpaceKeyState);
                        currentScene = SCENE_MAP;
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
                    DrawBoomerang(weaponWorldX, weaponWorldY, circles[i].angle, circles[i].weapon.color);
                } else {
                    DrawSpear(weaponWorldX, weaponWorldY, circles[i].angle, circles[i].weapon.color);
                }
            }
            
            // Draw K.O. text for dead players
            for (int i = 0; i < 2; i++) {
                if (!circles[i].isAlive) {
                    DrawFormatString((int)(circles[i].x + KO_TEXT_X_OFFSET), (int)(circles[i].y + KO_TEXT_Y_OFFSET), COLOR_BLACK, "K.O.");
                }
            }
            
            // Draw battle result
            if (battleEnded) {
                if (playerWon) {
                    DrawFormatString(SCREEN_WIDTH / 2 - 50, TITLE_Y_POSITION, COLOR_BLACK, "Victory!");
                    DrawFormatString(SCREEN_WIDTH / 2 - 80, TITLE_Y_POSITION + 20, COLOR_BLACK, "Press Enter/Space to continue");
                } else {
                    DrawFormatString(SCREEN_WIDTH / 2 - 50, TITLE_Y_POSITION, COLOR_BLACK, "Defeat!");
                    DrawFormatString(SCREEN_WIDTH / 2 - 80, TITLE_Y_POSITION + 20, COLOR_BLACK, "Press Enter/Space to retry");
                }
            } else {
                // Draw title text at top
                const char* nodeTypeName = GetNodeTypeName(mapNodes[currentNodeIndex].type);
                DrawFormatString(SCREEN_WIDTH / 2 - 50, TITLE_Y_POSITION, COLOR_BLACK, "Battle: %s", nodeTypeName);
            }
        }
        
        ScreenFlip();
    }
    
    DxLib_End();
    return 0;
}
