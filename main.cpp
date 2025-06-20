#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <iostream>
#include <optional>

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
    
    Tree(int x, int y, int leaf_size, int roots_size, int maximum_height, int reproduction_num, float reproduction_energy, int maximum_age) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> rand(0.0f, 1.0f);
        this->x = x;
        this->y = y;
        this->leaf_size = leaf_size + (int)((rand(gen) - 0.5) * 9 / 4);
        this->roots_size = roots_size + (int)((rand(gen) - 0.5) * 9 / 4);
        height = 5.0f; // Start small
        // this->maximum_height = maximum_height + (int)((rand(gen) - 0.5) * 10);
        this->maximum_height = maximum_height;
        this->reproduction_num = reproduction_num + (int)((rand(gen) - 0.5) * 21 / 10);
        this->reproduction_energy = reproduction_energy + (int)((rand(gen) - 0.5) * 15 / 7);
        energy = 0.0f;
        age = 0.0f;
        this->maximum_age = maximum_age + (int)((rand(gen) - 0.5) * 5);
        alive = true;
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
        if (height < maximum_height && energy >= sqrt(height) * pow(leaf_size, 1.5) * pow(roots_size, 1.5) / 5) {
            height += 1;
            energy -= sqrt(height);
        }
        
        // If at maximum height, use energy for reproduction
        if (height >= maximum_height) {
            float required_energy = pow(reproduction_num, 2) * reproduction_energy;
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
        std::uniform_real_distribution<float> rand(0.0f, 1.0f);

        for (int i = 0; i < reproduction_num; ++i) {
            if(rand(gen) * 100 > reproduction_energy){
                continue;
            }
            float angle = angle_dis(gen);
            float distance = distance_dis(gen);
            float new_x = x + std::cos(angle) * distance;
            float new_y = y + std::sin(angle) * distance;
            
            // Keep within screen bounds
            new_x = std::max(10.0f, std::min(690.0f, new_x));
            new_y = std::max(10.0f, std::min(690.0f, new_y));
            
            trees.emplace_back(new_x, new_y, leaf_size, roots_size, maximum_height, reproduction_num, reproduction_energy, maximum_age);
        }
    }
    
    void draw(sf::RenderWindow& window) {
        if (!alive) return;
        // Draw trunk
        sf::RectangleShape trunk(sf::Vector2f(sqrt(height), sqrt(height)));
        trunk.setPosition(sf::Vector2f(x, y));
        trunk.setFillColor(sf::Color((255 * age / maximum_age), (255 * age / maximum_age), (255 * age / maximum_age))); // Brown
        window.draw(trunk);
        
        // // Color based on age and health
        // float health_factor = 1.0f - (age / maximum_age);
        // sf::Uint8 green = static_cast<sf::Uint8>(255 * health_factor);
        // sf::Uint8 red = static_cast<sf::Uint8>(255 * (1.0f - health_factor));
        // leaves.setFillColor(sf::Color(red, green, 0));
        // window.draw(leaves);
    }
};

class EcosystemSimulation {
private:
    sf::RenderWindow window;
    std::vector<Tree> trees;
    sf::Clock clock;
    sf::Font font;
    bool showInfo;
    
public:
    EcosystemSimulation() : window(sf::VideoMode(sf::Vector2u(700, 700)), "Plant Simulation"), showInfo(false) {
        window.setFramerateLimit(10);
        // Initialize with 10 random trees
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> pos_dis(50.0f, 650.0f);
        std::uniform_real_distribution<float> rand(0.0f, 1.0f);
        for (int i = 0; i < 10; ++i) {
            float x = pos_dis(gen);
            float y = pos_dis(gen);
            int leaf_size = (int)(rand(gen) * 5) + 2;
            int roots_size = (int)(rand(gen) * 3) + 1;
            int maximum_height = (int)(rand(gen) * 50) + 30;
            maximum_height = 70;
            int reproduction_num = (int)(rand(gen) * 4) + 2;
            int reproduction_energy = (int)(rand(gen) * 20) + 10;
            int maximum_age = (int)(rand(gen) * 50) + 75;

            trees.emplace_back(x, y, leaf_size, roots_size, maximum_height, reproduction_num, reproduction_energy, maximum_age);
        }
    }
    
    void run() {
        printf("3");
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }
    
private:
    void handleEvents() {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
    }
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
        window.clear(sf::Color(150, 180, 250)); // Dark gray background
        
        // Draw all trees
        for (auto& tree : trees) {
            tree.draw(window);
        }
        
        window.display();
    }
};

int main() {
    printf("1");
    EcosystemSimulation simulation;
    printf("2");
    simulation.run();
    
    return 0;
}