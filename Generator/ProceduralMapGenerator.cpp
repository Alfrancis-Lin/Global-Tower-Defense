#include "ProceduralMapGenerator.hpp"
#include <cmath>

std::vector<std::vector<int>> ProceduralMapGenerator::generateMap(int algorithm) {
    initializeMap(1); // Start with all obstacles
    pathSet.clear();
    
    switch (algorithm) {
        case 1:
            generateWindingPath();
            break;
        case 2:
            generateWindingPath();
            generateBranchingPaths(2);
            smoothPaths(1);
            break;
        case 3:
            generateMazeLikePaths();
            break;
        default:
            generateWindingPath();
    }
    
    ensureEndpointsAccessible();
    return map;
}

bool ProceduralMapGenerator::hasValidPath(int startX, int startY, int endX, int endY) {
    if (!isValid(startX, startY) || !isValid(endX, endY) || 
        map[startX][startY] == 1 || map[endX][endY] == 1) {
        return false;
    }
    
    clearVisited();
    
    // Clear queue efficiently
    while (!bfsQueue.empty()) bfsQueue.pop();
    
    bfsQueue.emplace(startX, startY);
    visited[startX][startY] = true;
    
    while (!bfsQueue.empty()) {
        auto [x, y] = bfsQueue.front();
        bfsQueue.pop();
        
        if (x == endX && y == endY) return true;
        
        for (const auto& [dx, dy] : directions) {
            const int nx = x + dx;
            const int ny = y + dy;
            
            if (isValid(nx, ny) && !visited[nx][ny] && map[nx][ny] == 0) {
                visited[nx][ny] = true;
                bfsQueue.emplace(nx, ny);
            }
        }
    }
    return false;
}

void ProceduralMapGenerator::generateWindingPath() {
    pathBuffer.clear();
    pathSet.clear();
    
    int x = 0, y = 0;
    map[x][y] = 0;
    pathBuffer.emplace_back(x, y);
    pathSet.emplace(x, y);
    
    // Use A* inspired approach with randomness
    while (x < MapHeight - 1 || y < MapWidth - 1) {
        std::vector<std::pair<int, int>> candidates;
        candidates.reserve(4);
        
        // Priority to moves towards goal
        const int distToGoal = (MapHeight - 1 - x) + (MapWidth - 1 - y);
        
        for (const auto& [dx, dy] : directions) {
            const int nx = x + dx;
            const int ny = y + dy;
            
            if (!isValid(nx, ny)) continue;
            
            const int newDist = (MapHeight - 1 - nx) + (MapWidth - 1 - ny);
            
            // Prefer moves that get closer to goal, but allow some randomness
            if (newDist < distToGoal || (newDist == distToGoal && rng() % 3 == 0)) {
                candidates.emplace_back(nx, ny);
            }
        }
        
        if (candidates.empty()) {
            // Fallback: move towards goal deterministically
            if (x < MapHeight - 1) x++;
            else if (y < MapWidth - 1) y++;
        } else {
            const auto [nx, ny] = candidates[rng() % candidates.size()];
            x = nx;
            y = ny;
        }
        
        if (pathSet.find({x, y}) == pathSet.end()) {
            map[x][y] = 0;
            pathBuffer.emplace_back(x, y);
            pathSet.emplace(x, y);
        }
    }
}

void ProceduralMapGenerator::generateMazeLikePaths() {
    // Recursive backtracking maze generation adapted for tower defense
    std::vector<std::pair<int, int>> stack;
    std::vector<std::vector<bool>> mazeVisited(MapHeight, std::vector<bool>(MapWidth, false));
    
    const int startX = 0, startY = 0;
    stack.emplace_back(startX, startY);
    map[startX][startY] = 0;
    mazeVisited[startX][startY] = true;
    
    while (!stack.empty()) {
        auto [x, y] = stack.back();
        
        std::vector<std::pair<int, int>> neighbors;
        for (const auto& [dx, dy] : directions) {
            const int nx = x + dx * 2; // Skip one cell for maze effect
            const int ny = y + dy * 2;
            
            if (isValid(nx, ny) && !mazeVisited[nx][ny]) {
                neighbors.emplace_back(nx, ny);
            }
        }
        
        if (!neighbors.empty()) {
            const auto [nx, ny] = neighbors[rng() % neighbors.size()];
            
            // Connect current cell to chosen neighbor
            const int wallX = x + (nx - x) / 2;
            const int wallY = y + (ny - y) / 2;
            
            map[wallX][wallY] = 0;
            map[nx][ny] = 0;
            mazeVisited[nx][ny] = true;
            stack.emplace_back(nx, ny);
        } else {
            stack.pop_back();
        }
    }
    
    // Ensure path to end exists
    generateStraightPath(0, 0, MapHeight - 1, MapWidth - 1);
}

void ProceduralMapGenerator::generateBranchingPaths(int numBranches) {
    if (pathBuffer.empty()) {
        // Build path buffer from existing paths
        for (int i = 0; i < MapHeight; ++i) {
            for (int j = 0; j < MapWidth; ++j) {
                if (map[i][j] == 0) {
                    pathBuffer.emplace_back(i, j);
                }
            }
        }
    }
    
    if (pathBuffer.empty()) return;
    
    for (int branch = 0; branch < numBranches; ++branch) {
        const auto [startX, startY] = pathBuffer[rng() % pathBuffer.size()];
        const int branchLength = 3 + rng() % 6;
        
        int x = startX, y = startY;
        
        for (int step = 0; step < branchLength; ++step) {
            std::vector<std::pair<int, int>> validMoves;
            validMoves.reserve(4);
            
            for (const auto& [dx, dy] : directions) {
                const int nx = x + dx;
                const int ny = y + dy;
                
                if (isValid(nx, ny) && map[nx][ny] == 1) {
                    validMoves.emplace_back(nx, ny);
                }
            }
            
            if (validMoves.empty()) break;
            
            const auto [nx, ny] = validMoves[rng() % validMoves.size()];
            x = nx;
            y = ny;
            map[x][y] = 0;
        }
    }
}

void ProceduralMapGenerator::smoothPaths(int iterations) {
    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<std::vector<int>> tempMap = map;
        
        for (int i = 1; i < MapHeight - 1; ++i) {
            for (int j = 1; j < MapWidth - 1; ++j) {
                if (map[i][j] == 0) continue; // Don't modify existing paths
                
                int pathNeighbors = 0;
                for (const auto& [dx, dy] : directions) {
                    if (map[i + dx][j + dy] == 0) pathNeighbors++;
                }
                
                // Convert obstacles to paths if surrounded by many paths
                if (pathNeighbors >= 3) {
                    tempMap[i][j] = 0;
                }
            }
        }
        
        map = tempMap;
    }
}

void ProceduralMapGenerator::generateStraightPath(int startX, int startY, int endX, int endY) {
    // Use Bresenham-like algorithm for optimal path
    int x = startX, y = startY;
    const int dx = endX - startX;
    const int dy = endY - startY;
    const int steps = std::max(std::abs(dx), std::abs(dy));
    
    for (int i = 0; i <= steps; ++i) {
        const int currentX = startX + (dx * i) / steps;
        const int currentY = startY + (dy * i) / steps;
        
        if (isValid(currentX, currentY)) {
            map[currentX][currentY] = 0;
            pathSet.emplace(currentX, currentY);
        }
    }
}

void ProceduralMapGenerator::clearVisited() noexcept {
    for (auto& row : visited) {
        std::fill(row.begin(), row.end(), false);
    }
}

void ProceduralMapGenerator::initializeMap(int fillValue) noexcept {
    for (auto& row : map) {
        std::fill(row.begin(), row.end(), fillValue);
    }
}

void ProceduralMapGenerator::ensureEndpointsAccessible() noexcept {
    map[0][0] = 0;
    map[MapHeight - 1][MapWidth - 1] = 0;
    
    // Ensure there's always a valid path
    if (!hasValidPath(0, 0, MapHeight - 1, MapWidth - 1)) {
        generateStraightPath(0, 0, MapHeight - 1, MapWidth - 1);
    }
}
