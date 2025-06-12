#ifndef PROCEDURAL_MAP_GENERATOR_HPP
#define PROCEDURAL_MAP_GENERATOR_HPP

#include <vector>
#include <random>
#include <queue>
#include <unordered_set>
#include <array>

class ProceduralMapGenerator {
private:
    static constexpr int MapWidth = 20;
    static constexpr int MapHeight = 13;
    static constexpr std::array<std::pair<int, int>, 4> directions = {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    }};
    
    std::vector<std::vector<int>> map;
    std::mt19937 rng;
    
    // Cached data structures for optimization
    std::vector<std::vector<bool>> visited;
    std::queue<std::pair<int, int>> bfsQueue;
    std::vector<std::pair<int, int>> pathBuffer;
    
    // Hash function for coordinate pairs
    struct PairHash {
        std::size_t operator()(const std::pair<int, int>& p) const {
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };
    
    std::unordered_set<std::pair<int, int>, PairHash> pathSet;

public:
    explicit ProceduralMapGenerator(uint32_t seed) : rng(seed) {
        map.resize(MapHeight, std::vector<int>(MapWidth));
        visited.resize(MapHeight, std::vector<bool>(MapWidth));
        pathBuffer.reserve(MapWidth * MapHeight);
        pathSet.reserve(MapWidth * MapHeight / 4);
    }
    
    std::vector<std::vector<int>> generateMap(int algorithm = 0);

private:
    // Optimized bounds checking
    constexpr bool isValid(int x, int y) const noexcept {
        return static_cast<unsigned>(x) < MapHeight && static_cast<unsigned>(y) < MapWidth;
    }
    
    // Optimized path validation with early termination
    bool hasValidPath(int startX, int startY, int endX, int endY);
    
    // Optimized path generation algorithms
    void generateStraightPath(int startX, int startY, int endX, int endY);
    void generateWindingPath();
    void generateMazeLikePaths();
    void generateBranchingPaths(int numBranches = 3);
    
    // Utility functions
    void clearVisited() noexcept;
    void initializeMap(int fillValue = 1) noexcept;
    void ensureEndpointsAccessible() noexcept;
    
    // Advanced generation techniques
    void generateSpiralPath();
    void generateRoomConnector();
    void generateDualPathSystem();
    void generateOrganicCaverns();
    // void generateUsingCellularAutomata(double initialDensity = 0.45, int iterations = 5);
    void smoothPaths(int iterations = 2);
};

#endif
