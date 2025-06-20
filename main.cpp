#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <iostream>

//         class Tree {
//             constructor(x, y) {
//                 this.x = x;
//                 this.y = y;
//                 this.leaf_size = Math.random() * 5 + 2; // 2-7
//                 this.roots_size = Math.random() * 3 + 1; // 1-4
//                 this.height = 1;
//                 this.maximum_height = Math.random() * 50 + 30; // 30-80
//                 this.reproduction_num = Math.floor(Math.random() * 4) + 2; // 2-5
//                 this.reproduction_energy = Math.random() * 20 + 10; // 10-30
//                 this.energy = 0;
//                 this.age = 0;
//                 this.maximum_age = Math.random() * 500 + 200; // 200-700
//                 this.generation = 1;
//             }
            
//             update() {
//                 this.age++;
//                 this.energy += this.leaf_size * this.roots_size;
                
//                 // Growth phase
//                 if (this.height < this.maximum_height && this.energy >= 1) {
//                     this.energy -= 1;
//                     this.height += 1;
//                 }
                
//                 // Check if tree dies
//                 if (this.age >= this.maximum_age) {
//                     return { died: true, offspring: [] };
//                 }
                
//                 // Reproduction phase
//                 if (this.height >= this.maximum_height && 
//                     this.energy >= this.reproduction_num * this.reproduction_energy) {
                    
//                     const offspring = [];
//                     this.energy -= this.reproduction_num * this.reproduction_energy;
                    
//                     for (let i = 0; i < this.reproduction_num; i++) {
//                         // Place offspring around the parent tree
//                         const angle = (Math.PI * 2 * i) / this.reproduction_num;
//                         const distance = 50 + Math.random() * 50;
//                         const newX = this.x + Math.cos(angle) * distance;
//                         const newY = this.y + Math.sin(angle) * distance;
                        
//                         // Keep within bounds
//                         if (newX >= 20 && newX <= 680 && newY >= 20 && newY <= 680) {
//                             const newTree = new Tree(newX, newY);
//                             newTree.generation = this.generation + 1;
//                             offspring.push(newTree);
//                         }
//                     }
                    
//                     return { died: false, offspring: offspring };
//                 }
                
//                 return { died: false, offspring: [] };
//             }
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

// Compilation instructions:
// g++ -o plant_simulation plant_simulation.cpp -lsfml-graphics -lsfml-window -lsfml-system
// 
// Make sure you have SFML installed:
// Ubuntu/Debian: sudo apt-get install libsfml-dev
// macOS: brew install sfml
// Windows: Download from https://www.sfml-dev.org/

// <!DOCTYPE html>
// <html lang="en">
// <head>
//     <meta charset="UTF-8">
//     <meta name="viewport" content="width=device-width, initial-scale=1.0">
//     <title>Plant Ecosystem Simulation</title>
//     <style>
//         body {
//             margin: 0;
//             padding: 20px;
//             font-family: Arial, sans-serif;
//             background: linear-gradient(135deg, #87CEEB 0%, #98FB98 100%);
//             min-height: 100vh;
//         }
        
//         .container {
//             max-width: 800px;
//             margin: 0 auto;
//             background: rgba(255, 255, 255, 0.1);
//             border-radius: 20px;
//             padding: 20px;
//             backdrop-filter: blur(10px);
//             box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
//         }
        
//         h1 {
//             text-align: center;
//             color: #2d5016;
//             margin-bottom: 20px;
//             text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
//         }
        
//         .controls {
//             display: flex;
//             justify-content: center;
//             gap: 15px;
//             margin-bottom: 20px;
//             flex-wrap: wrap;
//         }
        
//         button {
//             padding: 12px 24px;
//             border: none;
//             border-radius: 25px;
//             background: linear-gradient(45deg, #4CAF50, #45a049);
//             color: white;
//             font-weight: bold;
//             cursor: pointer;
//             transition: all 0.3s ease;
//             box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
//         }
        
//         button:hover {
//             transform: translateY(-2px);
//             box-shadow: 0 6px 20px rgba(0, 0, 0, 0.3);
//         }
        
//         button:disabled {
//             background: #ccc;
//             cursor: not-allowed;
//             transform: none;
//         }
        
//         #canvas {
//             border: 3px solid #2d5016;
//             border-radius: 15px;
//             background: linear-gradient(180deg, #87CEEB 0%, #90EE90 50%, #228B22 100%);
//             display: block;
//             margin: 0 auto;
//             box-shadow: 0 8px 25px rgba(0, 0, 0, 0.3);
//         }
        
//         .stats {
//             margin-top: 20px;
//             display: grid;
//             grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
//             gap: 15px;
//         }
        
//         .stat-card {
//             background: rgba(255, 255, 255, 0.2);
//             padding: 15px;
//             border-radius: 15px;
//             text-align: center;
//             backdrop-filter: blur(5px);
//             border: 1px solid rgba(255, 255, 255, 0.3);
//         }
        
//         .stat-value {
//             font-size: 2em;
//             font-weight: bold;
//             color: #2d5016;
//             text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3);
//         }
        
//         .stat-label {
//             color: #556b2f;
//             font-weight: bold;
//             margin-top: 5px;
//         }
        
//         .speed-control {
//             display: flex;
//             align-items: center;
//             gap: 10px;
//             justify-content: center;
//             margin-top: 15px;
//         }
        
//         .speed-control label {
//             color: #2d5016;
//             font-weight: bold;
//         }
        
//         .speed-control input {
//             width: 150px;
//         }
//     </style>
// </head>
// <body>
//     <div class="container">
//         <h1>🌳 Plant Ecosystem Simulation 🌱</h1>
        
//         <div class="controls">
//             <button id="startBtn">Start Simulation</button>
//             <button id="pauseBtn" disabled>Pause</button>
//             <button id="resetBtn">Reset</button>
//         </div>
        
//         <div class="speed-control">
//             <label for="speedSlider">Simulation Speed:</label>
//             <input type="range" id="speedSlider" min="50" max="500" value="200" step="50">
//             <span id="speedValue">200ms</span>
//         </div>
        
//         <canvas id="canvas" width="700" height="700"></canvas>
        
//         <div class="stats">
//             <div class="stat-card">
//                 <div class="stat-value" id="treeCount">10</div>
//                 <div class="stat-label">Living Trees</div>
//             </div>
//             <div class="stat-card">
//                 <div class="stat-value" id="totalBorn">10</div>
//                 <div class="stat-label">Total Born</div>
//             </div>
//             <div class="stat-card">
//                 <div class="stat-value" id="totalDied">0</div>
//                 <div class="stat-label">Total Died</div>
//             </div>
//             <div class="stat-card">
//                 <div class="stat-value" id="generations">1</div>
//                 <div class="stat-label">Generations</div>
//             </div>
//         </div>
//     </div>

//     <script>
//         class Tree {
//             constructor(x, y) {
//                 this.x = x;
//                 this.y = y;
//                 this.leaf_size = Math.random() * 5 + 2; // 2-7
//                 this.roots_size = Math.random() * 3 + 1; // 1-4
//                 this.height = 1;
//                 this.maximum_height = Math.random() * 50 + 30; // 30-80
//                 this.reproduction_num = Math.floor(Math.random() * 4) + 2; // 2-5
//                 this.reproduction_energy = Math.random() * 20 + 10; // 10-30
//                 this.energy = 0;
//                 this.age = 0;
//                 this.maximum_age = Math.random() * 500 + 200; // 200-700
//                 this.generation = 1;
//             }
            
//             update() {
//                 this.age++;
//                 this.energy += this.leaf_size * this.roots_size;
                
//                 // Growth phase
//                 if (this.height < this.maximum_height && this.energy >= 1) {
//                     this.energy -= 1;
//                     this.height += 1;
//                 }
                
//                 // Check if tree dies
//                 if (this.age >= this.maximum_age) {
//                     return { died: true, offspring: [] };
//                 }
                
//                 // Reproduction phase
//                 if (this.height >= this.maximum_height && 
//                     this.energy >= this.reproduction_num * this.reproduction_energy) {
                    
//                     const offspring = [];
//                     this.energy -= this.reproduction_num * this.reproduction_energy;
                    
//                     for (let i = 0; i < this.reproduction_num; i++) {
//                         // Place offspring around the parent tree
//                         const angle = (Math.PI * 2 * i) / this.reproduction_num;
//                         const distance = 50 + Math.random() * 50;
//                         const newX = this.x + Math.cos(angle) * distance;
//                         const newY = this.y + Math.sin(angle) * distance;
                        
//                         // Keep within bounds
//                         if (newX >= 20 && newX <= 680 && newY >= 20 && newY <= 680) {
//                             const newTree = new Tree(newX, newY);
//                             newTree.generation = this.generation + 1;
//                             offspring.push(newTree);
//                         }
//                     }
                    
//                     return { died: false, offspring: offspring };
//                 }
                
//                 return { died: false, offspring: [] };
//             }
            
//             draw(ctx) {
//                 // Draw roots (underground)
//                 ctx.fillStyle = '#8B4513';
//                 ctx.fillRect(this.x - this.roots_size * 2, this.y + 5, this.roots_size * 4, this.roots_size * 3);
                
//                 // Draw trunk
//                 const trunkWidth = Math.max(2, this.height / 10);
//                 ctx.fillStyle = '#654321';
//                 ctx.fillRect(this.x - trunkWidth/2, this.y - this.height, trunkWidth, this.height);
                
//                 // Draw leaves (crown)
//                 const crownRadius = this.leaf_size * (this.height / this.maximum_height);
//                 ctx.fillStyle = `hsl(${120 + this.generation * 20}, 70%, ${30 + (this.energy / 100) * 20}%)`;
//                 ctx.beginPath();
//                 ctx.arc(this.x, this.y - this.height, crownRadius, 0, Math.PI * 2);
//                 ctx.fill();
                
//                 // Age indicator (lighter color for older trees)
//                 const ageRatio = this.age / this.maximum_age;
//                 if (ageRatio > 0.7) {
//                     ctx.fillStyle = `rgba(139, 69, 19, ${ageRatio * 0.5})`;
//                     ctx.beginPath();
//                     ctx.arc(this.x, this.y - this.height, crownRadius, 0, Math.PI * 2);
//                     ctx.fill();
//                 }
//             }
//         }
        
//         class EcosystemSimulation {
//             constructor() {
//                 this.canvas = document.getElementById('canvas');
//                 this.ctx = this.canvas.getContext('2d');
//                 this.trees = [];
//                 this.isRunning = false;
//                 this.totalBorn = 0;
//                 this.totalDied = 0;
//                 this.maxGeneration = 1;
//                 this.speed = 200;
                
//                 this.initializeTrees();
//                 this.setupControls();
//                 this.updateStats();
//                 this.draw();
//             }
            
//             initializeTrees() {
//                 this.trees = [];
//                 this.totalBorn = 10;
//                 this.totalDied = 0;
//                 this.maxGeneration = 1;
                
//                 for (let i = 0; i < 10; i++) {
//                     const x = Math.random() * 660 + 20;
//                     const y = Math.random() * 660 + 20;
//                     this.trees.push(new Tree(x, y));
//                 }
//             }
            
//             setupControls() {
//                 const startBtn = document.getElementById('startBtn');
//                 const pauseBtn = document.getElementById('pauseBtn');
//                 const resetBtn = document.getElementById('resetBtn');
//                 const speedSlider = document.getElementById('speedSlider');
//                 const speedValue = document.getElementById('speedValue');
                
//                 startBtn.addEventListener('click', () => this.start());
//                 pauseBtn.addEventListener('click', () => this.pause());
//                 resetBtn.addEventListener('click', () => this.reset());
                
//                 speedSlider.addEventListener('input', (e) => {
//                     this.speed = parseInt(e.target.value);
//                     speedValue.textContent = this.speed + 'ms';
//                 });
//             }
            
//             start() {
//                 this.isRunning = true;
//                 document.getElementById('startBtn').disabled = true;
//                 document.getElementById('pauseBtn').disabled = false;
//                 this.simulate();
//             }
            
//             pause() {
//                 this.isRunning = false;
//                 document.getElementById('startBtn').disabled = false;
//                 document.getElementById('pauseBtn').disabled = true;
//             }
            
//             reset() {
//                 this.isRunning = false;
//                 document.getElementById('startBtn').disabled = false;
//                 document.getElementById('pauseBtn').disabled = true;
//                 this.initializeTrees();
//                 this.updateStats();
//                 this.draw();
//             }
            
//             simulate() {
//                 if (!this.isRunning) return;
                
//                 const newTrees = [];
                
//                 for (let i = 0; i < this.trees.length; i++) {
//                     const tree = this.trees[i];
//                     const result = tree.update();
                    
//                     if (!result.died) {
//                         newTrees.push(tree);
                        
//                         // Add offspring
//                         for (const offspring of result.offspring) {
//                             newTrees.push(offspring);
//                             this.totalBorn++;
//                             this.maxGeneration = Math.max(this.maxGeneration, offspring.generation);
//                         }
//                     } else {
//                         this.totalDied++;
//                     }
//                 }
                
//                 this.trees = newTrees;
//                 this.updateStats();
//                 this.draw();
                
//                 setTimeout(() => this.simulate(), this.speed);
//             }
            
//             updateStats() {
//                 document.getElementById('treeCount').textContent = this.trees.length;
//                 document.getElementById('totalBorn').textContent = this.totalBorn;
//                 document.getElementById('totalDied').textContent = this.totalDied;
//                 document.getElementById('generations').textContent = this.maxGeneration;
//             }
            
//             draw() {
//                 // Clear canvas with gradient background
//                 const gradient = this.ctx.createLinearGradient(0, 0, 0, 700);
//                 gradient.addColorStop(0, '#87CEEB');
//                 gradient.addColorStop(0.5, '#90EE90');
//                 gradient.addColorStop(1, '#228B22');
//                 this.ctx.fillStyle = gradient;
//                 this.ctx.fillRect(0, 0, 700, 700);
                
//                 // Draw ground line
//                 this.ctx.strokeStyle = '#8B4513';
//                 this.ctx.lineWidth = 2;
//                 this.ctx.beginPath();
//                 this.ctx.moveTo(0, 680);
//                 this.ctx.lineTo(700, 680);
//                 this.ctx.stroke();
                
//                 // Draw trees
//                 for (const tree of this.trees) {
//                     tree.draw(this.ctx);
//                 }
                
//                 // Draw sun
//                 this.ctx.fillStyle = '#FFD700';
//                 this.ctx.beginPath();
//                 this.ctx.arc(650, 50, 30, 0, Math.PI * 2);
//                 this.ctx.fill();
//             }
//         }
        
//         // Initialize the simulation
//         const simulation = new EcosystemSimulation();
//     </script>
// </body>
// </html>