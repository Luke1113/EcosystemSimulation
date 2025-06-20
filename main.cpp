#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <iostream>

class Tree {
public:
    int x, y;
    int leaf_size;
    int roots_size;
    int height;
    int maximum_height;
    int reproduction_num;
    float reproduction_energy;
    float energy;
    int age;
    int maximum_age;
    bool alive;
    
    Tree(float x, float y) : x(x), y(y), alive(true) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> rand(0.0f, 1.0f);
        
        leaf_size = (int)(rand(gen) * 5) + 2;
        roots_size = (int)(rand(gen) * 3) + 1;
        height = 5.0f; // Start small
        maximum_height = (int)(rand(gen) * 50) + 30;
        reproduction_num = (int)(rand(gen) * 4) + 2;
        reproduction_energy = (int)(rand(gen) * 20) + 10;
        energy = 0.0f;
        age = 0.0f;
        maximum_age = (int)(rand(gen) * 50) + 75;
    }
    
    void update(std::vector<Tree>& trees) {
        if (!alive) return;
        
        // Age increases continuously
        age += 1; // Scaling factor for visible aging
        
        // Check if tree dies from old age
        if (age >= maximum_age) {
            alive = false;
            return;
        }
        
        // Energy increases by leaf_size * roots_size
        energy += leaf_size * roots_size; // Scaling factor
        
        // Use energy to grow in height until maximum_height is reached
        if (height < maximum_height && energy >= sqrt(height)) {
            height += 1;
            energy -= sqrt(height);
        }
        
        // If at maximum height, use energy for reproduction
        if (height >= maximum_height) {
            float required_energy = reproduction_num * reproduction_energy;
            if (energy >= required_energy) {
                reproduce(trees);
                energy -= required_energy;
            }
        }
    }
    
    void reproduce(std::vector<Tree>& trees) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> angle_dis(0.0f, 2.0f * M_PI);
        std::uniform_real_distribution<float> distance_dis(30.0f, 80.0f);
        
        for (int i = 0; i < reproduction_num; ++i) {
            float angle = angle_dis(gen);
            float distance = distance_dis(gen);
            float new_x = x + std::cos(angle) * distance;
            float new_y = y + std::sin(angle) * distance;
            
            // Keep within screen bounds
            new_x = std::max(10.0f, std::min(690.0f, new_x));
            new_y = std::max(10.0f, std::min(690.0f, new_y));
            
            trees.emplace_back(new_x, new_y);
        }
    }
    
    void draw(sf::RenderWindow& window) {
        if (!alive) return;
        // Draw trunk
        sf::RectangleShape trunk(sf::Vector2f(sqrt(height), sqrt(height)));
        trunk.setPosition(sf::Vector2f(x, y));
        trunk.setFillColor(sf::Color(101, 67, 33)); // Brown
        window.draw(trunk);
        
        // // Color based on age and health
        // float health_factor = 1.0f - (age / maximum_age);
        // sf::Uint8 green = static_cast<sf::Uint8>(255 * health_factor);
        // sf::Uint8 red = static_cast<sf::Uint8>(255 * (1.0f - health_factor));
        // leaves.setFillColor(sf::Color(red, green, 0));
        // window.draw(leaves);
        
        // Draw height indicator (optional visual aid)
        // if (height < maximum_height) {
        //     sf::RectangleShape growth_indicator(sf::Vector2f(2, maximum_height - height));
        //     growth_indicator.setPosition(x + 3, y - (maximum_height - height));
        //     growth_indicator.setFillColor(sf::Color(255, 255, 255, 50));
        //     window.draw(growth_indicator);
        // }
    }
    
    // void drawInfo(sf::RenderWindow& window, sf::Font& font) {
    //     if (!alive) return;
        
    //     sf::Text info;
    //     info.setFont(font);
    //     info.setCharacterSize(10);
    //     info.setFillColor(sf::Color::White);
        
    //     std::string text = "H:" + std::to_string(static_cast<int>(height)) + "/" + 
    //                       std::to_string(static_cast<int>(maximum_height)) +
    //                       " E:" + std::to_string(static_cast<int>(energy)) +
    //                       " A:" + std::to_string(static_cast<int>(age)) + "/" +
    //                       std::to_string(static_cast<int>(maximum_age));
        
    //     info.setString(text);
    //     info.setPosition(x - 30, y - 20);
    //     window.draw(info);
    // }
};

class EcosystemSimulation {
private:
    sf::RenderWindow window;
    std::vector<Tree> trees;
    sf::Clock clock;
    sf::Font font;
    bool showInfo;
    
public:
    EcosystemSimulation() : window(sf::VideoMode(700, 700), "Plant Simulation"), showInfo(false) {
        // Try to load a font (SFML default font might not be available)
        // For this simulation, we'll handle the case where font loading fails
        // font.loadFromFile("arial.ttf"); // You might need to provide a font file
        
        // Initialize with 10 random trees
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> pos_dis(50.0f, 650.0f);
        
        for (int i = 0; i < 10; ++i) {
            float x = pos_dis(gen);
            float y = pos_dis(gen);
            trees.emplace_back(x, y);
        }
    }
    
    void run() {
        while (window.isOpen()) {
            // handleEvents();
            update();
            render();
        }
    }
    
private:
    // void handleEvents() {
    //     sf::Event event;
    //     while (window.pollEvent(event)) {
    //         if (event.type == sf::Event::Closed) {
    //             window.close();
    //         }
    //         if (event.type == sf::Event::KeyPressed) {
    //             if (event.key.code == sf::Keyboard::Space) {
    //                 showInfo = !showInfo;
    //             }
    //             if (event.key.code == sf::Keyboard::R) {
    //                 // Reset simulation
    //                 trees.clear();
    //                 std::random_device rd;
    //                 std::mt19937 gen(rd());
    //                 std::uniform_real_distribution<float> pos_dis(50.0f, 650.0f);
                    
    //                 for (int i = 0; i < 10; ++i) {
    //                     float x = pos_dis(gen);
    //                     float y = pos_dis(gen);
    //                     trees.emplace_back(x, y);
    //                 }
    //             }
    //         }
    //     }
    // }
    
    void update() {
        float deltaTime = clock.restart().asSeconds();
        
        // Update all trees
        for (auto& tree : trees) {
            tree.update(trees);
        }
        
        // Remove dead trees
        trees.erase(std::remove_if(trees.begin(), trees.end(), 
                                 [](const Tree& tree) { return !tree.alive; }), 
                   trees.end());
    }
    
    void render() {
        window.clear(sf::Color(50, 50, 50)); // Dark gray background
        
        // Draw all trees
        for (auto& tree : trees) {
            tree.draw(window);
            // if (showInfo) {
            //     tree.drawInfo(window, font);
            // }
        }
        
        // Draw instructions
        // sf::Text instructions;
        // instructions.setFont(font);
        // instructions.setCharacterSize(16);
        // instructions.setFillColor(sf::Color::White);
        // instructions.setString("SPACE: Toggle Info | R: Reset | Trees: " + std::to_string(trees.size()));
        // instructions.setPosition(10, 10);
        // window.draw(instructions);
        
        window.display();
    }
};

int main() {
    EcosystemSimulation simulation;
    simulation.run();
    
    return 0;
}