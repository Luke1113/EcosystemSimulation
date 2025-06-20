#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

class Tree {
private:
    static std::random_device rd;
    static std::mt19937 gen;
    static std::uniform_real_distribution<float> dis;

public:
    float x, y;
    float leaf_size;
    float roots_size;
    float height;
    float maximum_height;
    int reproduction_num;
    float reproduction_energy;
    float energy;
    int age;
    int maximum_age;
    int generation;
    
    Tree(float x, float y) : x(x), y(y), generation(1) {
        leaf_size = dis(gen) * 5.0f + 2.0f;        // 2-7
        roots_size = dis(gen) * 3.0f + 1.0f;       // 1-4
        height = 1.0f;
        maximum_height = dis(gen) * 50.0f + 30.0f; // 30-80
        reproduction_num = static_cast<int>(dis(gen) * 4) + 2; // 2-5
        reproduction_energy = dis(gen) * 20.0f + 10.0f; // 10-30
        energy = 0.0f;
        age = 0;
        maximum_age = static_cast<int>(dis(gen) * 500) + 200; // 200-700
    }
    
    struct UpdateResult {
        bool died;
        std::vector<Tree> offspring;
    };
    
    UpdateResult update() {
        age++;
        energy += leaf_size * roots_size;
        
        UpdateResult result;
        result.died = false;
        
        // Growth phase
        if (height < maximum_height && energy >= 1.0f) {
            energy -= 1.0f;
            height += 1.0f;
        }
        
        // Check if tree dies
        if (age >= maximum_age) {
            result.died = true;
            return result;
        }
        
        // Reproduction phase
        if (height >= maximum_height && 
            energy >= reproduction_num * reproduction_energy) {
            
            energy -= reproduction_num * reproduction_energy;
            
            for (int i = 0; i < reproduction_num; i++) {
                // Place offspring around the parent tree
                float angle = (2.0f * M_PI * i) / reproduction_num;
                float distance = 50.0f + dis(gen) * 50.0f;
                float newX = x + std::cos(angle) * distance;
                float newY = y + std::sin(angle) * distance;
                
                // Keep within bounds
                if (newX >= 20 && newX <= 680 && newY >= 20 && newY <= 680) {
                    Tree newTree(newX, newY);
                    newTree.generation = generation + 1;
                    result.offspring.push_back(newTree);
                }
            }
        }
        
        return result;
    }
    
    void draw(sf::RenderWindow& window) {
        // Draw roots (underground)
        sf::RectangleShape roots(sf::Vector2f(roots_size * 4, roots_size * 3));
        roots.setPosition(x - roots_size * 2, y + 5);
        roots.setFillColor(sf::Color(139, 69, 19)); // Brown
        window.draw(roots);
        
        // Draw trunk
        float trunkWidth = std::max(2.0f, height / 10.0f);
        sf::RectangleShape trunk(sf::Vector2f(trunkWidth, height));
        trunk.setPosition(x - trunkWidth/2, y - height);
        trunk.setFillColor(sf::Color(101, 67, 33)); // Dark brown
        window.draw(trunk);
        
        // Draw leaves (crown)
        float crownRadius = leaf_size * (height / maximum_height);
        sf::CircleShape crown(crownRadius);
        crown.setPosition(x - crownRadius, y - height - crownRadius);
        
        // Color based on generation and energy
        int hue = 120 + generation * 20;
        int lightness = 30 + static_cast<int>((energy / 100.0f) * 20);
        lightness = std::min(50, std::max(10, lightness));
        
        // Simple HSL to RGB conversion for green variations
        sf::Uint8 green = static_cast<sf::Uint8>(lightness * 5);
        green = std::min(static_cast<sf::Uint8>(255), green);
        crown.setFillColor(sf::Color(0, green, 0));
        window.draw(crown);
        
        // Age indicator (darker overlay for older trees)
        float ageRatio = static_cast<float>(age) / maximum_age;
        if (ageRatio > 0.7f) {
            sf::CircleShape ageOverlay(crownRadius);
            ageOverlay.setPosition(x - crownRadius, y - height - crownRadius);
            sf::Uint8 alpha = static_cast<sf::Uint8>(ageRatio * 127);
            ageOverlay.setFillColor(sf::Color(139, 69, 19, alpha));
            window.draw(ageOverlay);
        }
    }
};

// Static member definitions
std::random_device Tree::rd;
std::mt19937 Tree::gen(Tree::rd());
std::uniform_real_distribution<float> Tree::dis(0.0f, 1.0f);

class EcosystemSimulation {
private:
    sf::RenderWindow window;
    std::vector<Tree> trees;
    bool isRunning;
    int totalBorn;
    int totalDied;
    int maxGeneration;
    sf::Clock clock;
    sf::Time simulationSpeed;
    sf::Font font;
    
    // UI elements
    sf::RectangleShape startButton, pauseButton, resetButton;
    sf::Text startText, pauseText, resetText;
    sf::Text statsText;
    sf::Text titleText, speedText;
    
    bool startButtonPressed, pauseButtonPressed, resetButtonPressed;
    
public:
    EcosystemSimulation() : 
        window(sf::VideoMode(900, 800), "Plant Ecosystem Simulation"),
        isRunning(false),
        totalBorn(10),
        totalDied(0),
        maxGeneration(1),
        simulationSpeed(sf::milliseconds(200)),
        startButtonPressed(false),
        pauseButtonPressed(false),
        resetButtonPressed(false) {
        
        window.setFramerateLimit(60);
        
        // Try to load font (you may need to adjust the path)
        if (!font.loadFromFile("arial.ttf")) {
            // If arial.ttf is not found, try default system font locations
            if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf") &&
                !font.loadFromFile("/System/Library/Fonts/Arial.ttf") &&
                !font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
                std::cout << "Warning: Could not load font. Using default font.\n";
            }
        }
        
        initializeTrees();
        setupUI();
    }
    
    void initializeTrees() {
        trees.clear();
        totalBorn = 10;
        totalDied = 0;
        maxGeneration = 1;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(20.0f, 680.0f);
        
        for (int i = 0; i < 10; i++) {
            float x = dis(gen);
            float y = dis(gen);
            trees.emplace_back(x, y);
        }
    }
    
    void setupUI() {
        // Setup buttons
        startButton.setSize(sf::Vector2f(80, 30));
        startButton.setPosition(720, 50);
        startButton.setFillColor(sf::Color(76, 175, 80));
        
        pauseButton.setSize(sf::Vector2f(80, 30));
        pauseButton.setPosition(810, 50);
        pauseButton.setFillColor(sf::Color(158, 158, 158));
        
        resetButton.setSize(sf::Vector2f(80, 30));
        resetButton.setPosition(720, 90);
        resetButton.setFillColor(sf::Color(244, 67, 54));
        
        // Setup button texts
        startText.setFont(font);
        startText.setString("Start");
        startText.setCharacterSize(16);
        startText.setFillColor(sf::Color::White);
        startText.setPosition(740, 57);
        
        pauseText.setFont(font);
        pauseText.setString("Pause");
        pauseText.setCharacterSize(16);
        pauseText.setFillColor(sf::Color::White);
        pauseText.setPosition(825, 57);
        
        resetText.setFont(font);
        resetText.setString("Reset");
        resetText.setCharacterSize(16);
        resetText.setFillColor(sf::Color::White);
        resetText.setPosition(740, 97);
        
        // Setup title
        titleText.setFont(font);
        titleText.setString("Plant Ecosystem Simulation");
        titleText.setCharacterSize(24);
        titleText.setFillColor(sf::Color(45, 80, 22));
        titleText.setPosition(720, 10);
        
        // Setup stats text
        statsText.setFont(font);
        statsText.setCharacterSize(14);
        statsText.setFillColor(sf::Color(45, 80, 22));
        statsText.setPosition(720, 150);
        
        // Setup speed text
        speedText.setFont(font);
        speedText.setString("Speed: 200ms\n[1-9] to change");
        speedText.setCharacterSize(12);
        speedText.setFillColor(sf::Color(45, 80, 22));
        speedText.setPosition(720, 120);
    }
    
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                
                if (startButton.getGlobalBounds().contains(mousePos)) {
                    start();
                }
                else if (pauseButton.getGlobalBounds().contains(mousePos)) {
                    pause();
                }
                else if (resetButton.getGlobalBounds().contains(mousePos)) {
                    reset();
                }
            }
            
            if (event.type == sf::Event::KeyPressed) {
                // Speed control with number keys
                if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num9) {
                    int speedLevel = event.key.code - sf::Keyboard::Num0;
                    simulationSpeed = sf::milliseconds(speedLevel * 50);
                    
                    std::stringstream ss;
                    ss << "Speed: " << speedLevel * 50 << "ms\n[1-9] to change";
                    speedText.setString(ss.str());
                }
            }
        }
    }
    
    void start() {
        isRunning = true;
        startButton.setFillColor(sf::Color(158, 158, 158));
        pauseButton.setFillColor(sf::Color(76, 175, 80));
    }
    
    void pause() {
        isRunning = false;
        startButton.setFillColor(sf::Color(76, 175, 80));
        pauseButton.setFillColor(sf::Color(158, 158, 158));
    }
    
    void reset() {
        isRunning = false;
        startButton.setFillColor(sf::Color(76, 175, 80));
        pauseButton.setFillColor(sf::Color(158, 158, 158));
        initializeTrees();
        clock.restart();
    }
    
    void update() {
        if (!isRunning) return;
        
        if (clock.getElapsedTime() >= simulationSpeed) {
            std::vector<Tree> newTrees;
            
            for (auto& tree : trees) {
                auto result = tree.update();
                
                if (!result.died) {
                    newTrees.push_back(tree);
                    
                    // Add offspring
                    for (const auto& offspring : result.offspring) {
                        newTrees.push_back(offspring);
                        totalBorn++;
                        maxGeneration = std::max(maxGeneration, offspring.generation);
                    }
                } else {
                    totalDied++;
                }
            }
            
            trees = std::move(newTrees);
            clock.restart();
        }
    }
    
    void updateStats() {
        std::stringstream ss;
        ss << "Living Trees: " << trees.size() << "\n";
        ss << "Total Born: " << totalBorn << "\n";
        ss << "Total Died: " << totalDied << "\n";
        ss << "Generations: " << maxGeneration << "\n";
        statsText.setString(ss.str());
    }
    
    void draw() {
        window.clear();
        
        // Draw gradient background (simplified as solid colors)
        sf::RectangleShape sky(sf::Vector2f(700, 350));
        sky.setPosition(0, 0);
        sky.setFillColor(sf::Color(135, 206, 235)); // Sky blue
        window.draw(sky);
        
        sf::RectangleShape ground(sf::Vector2f(700, 350));
        ground.setPosition(0, 350);
        ground.setFillColor(sf::Color(34, 139, 34)); // Forest green
        window.draw(ground);
        
        // Draw ground line
        sf::RectangleShape groundLine(sf::Vector2f(700, 2));
        groundLine.setPosition(0, 680);
        groundLine.setFillColor(sf::Color(139, 69, 19));
        window.draw(groundLine);
        
        // Draw sun
        sf::CircleShape sun(30);
        sun.setPosition(620, 20);
        sun.setFillColor(sf::Color(255, 215, 0));
        window.draw(sun);
        
        // Draw simulation area border
        sf::RectangleShape border(sf::Vector2f(700, 700));
        border.setPosition(0, 0);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(3);
        border.setOutlineColor(sf::Color(45, 80, 22));
        window.draw(border);
        
        // Draw trees
        for (auto& tree : trees) {
            tree.draw(window);
        }
        
        // Draw UI elements
        window.draw(startButton);
        window.draw(pauseButton);
        window.draw(resetButton);
        window.draw(startText);
        window.draw(pauseText);
        window.draw(resetText);
        window.draw(titleText);
        window.draw(speedText);
        window.draw(statsText);
        
        window.display();
    }
    
    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            updateStats();
            draw();
        }
    }
};

int main() {
    try {
        EcosystemSimulation simulation;
        simulation.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}

// Compilation instructions:
// g++ -o ecosystem main.cpp -lsfml-graphics -lsfml-window -lsfml-system
// 
// On Windows with MSVC:
// cl main.cpp /I"path_to_sfml/include" /link /LIBPATH:"path_to_sfml/lib" sfml-graphics.lib sfml-window.lib sfml-system.lib
//
// Make sure to have arial.ttf or another font file in your project directory
// or update the font loading paths in the constructor