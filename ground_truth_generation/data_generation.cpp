#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <cmath>
#include <tuple>

class DataGenerator {
public:
    DataGenerator();

    //void store_data(std::string df_output);
    void setup();
    void simulate();

private:
    std::vector<std::tuple<float, float, float, float>> circles;
    std::vector<std::tuple<float, float, float, float>> attractions;
    std::vector<std::tuple<float, float, float, float>> rejections;

    int number_of_frames;
    int number_of_recordings;
    int num_droplets;
    int screen_width;
    int screen_height;
    int droplet_radius;
    int num_attractions;
    int attraction_radius;
    int num_rejections;
    int rejection_radius;
    int larger_droplet_radius;

    float max_random_velocity;
    float max_intrinsic_velocity;
    float disappear_probability;
    float attraction_strength;
    float attraction_strength_droplets;
    float attraction_speed;
    float rejection_strength;
    float rejection_strength_droplets;
    float rejection_speed;
    float larger_droplet_speed_x;
    float larger_droplet_speed_y;
    float larger_droplet_x;
    float larger_droplet_y;
    float larger_droplet_dx;
    float larger_droplet_dy;

    bool random_movement;
    bool intrinsic_movement;
    bool disappearing_droplets;
    bool attraction_points;
    bool attraction_movement;
    bool rejection_points;
    bool rejection_movement;
    bool larger_droplet;

    void _remove_droplets();
    void _generate_droplets();
    void _generate_attraction_points();
    void _generate_rejection_points();
    void _move_giant_droplet();
    void _move_attraction_points();
    void _move_rejection_points();
    void _move_circles(int start, int end, bool random_movement=true);
    void _detect_collisions(int start, int end);

    std::random_device rd;
};

// void DataGenerator::store_data(std::string df_output) {
//     // Open output file
//     std::ofstream outfile(df_output);

//     // Write header row
//     outfile << "Droplet ID,";
//     for (int i = 0; i < this->number_of_recordings; i++) {
//         outfile << "Time Point " << i << ",";
//     }
//     outfile << std::endl;

//     // Write data rows
//     for (int i = 0; i < this->num_droplets; i++) {
//         outfile << i << ",";
//         for (int j = 0; j < this->number_of_recordings; j++) {
//             int frame_index = j * (this->number_of_frames / this->number_of_recordings);
//             float x = this->droplets[i][frame_index][0];
//             float y = this->droplets[i][frame_index][1];
//             outfile << x << "," << y << ",";
//         }
//         outfile << std::endl;
//     }

//     // Close output file
//     outfile.close();
// }

DataGenerator::DataGenerator() {
    // Length of simulation
    this->number_of_frames = 1000;
    this->number_of_recordings = 10;

    // Screen parameters
    this->screen_width = 1920;
    this->screen_height = 1080;

    // Droplet parameters
    this->random_movement = true;
    this->intrinsic_movement = true;
    this->num_droplets = 900;
    this->droplet_radius = 11;
    this->max_random_velocity = 0.1;
    this->max_intrinsic_velocity = 0.1;
    this->disappearing_droplets = false;
    this->disappear_probability = 0.01;

    // Attraction point (positive focal point) parameters
    this->attraction_points = true;
    this->attraction_movement = true;
    this->num_attractions = 15;
    this->attraction_radius = 80;
    this->attraction_strength = 0.2;
    this->attraction_strength_droplets = 0.03;
    this->attraction_speed = 1;

    // Rejection point (negative focal point) parameters
    this->rejection_points = false;
    this->rejection_movement = false;
    this->num_rejections = 10;
    this->rejection_radius = 80;
    this->rejection_strength = 0.2;
    this->rejection_strength_droplets = 0.03;
    this->rejection_speed = 1;

    // Giant droplet parameters
    this->larger_droplet = true;
    this->larger_droplet_radius = 80;
    this->larger_droplet_speed_x = 0.2;
    this->larger_droplet_speed_y = 0.01;
}

void DataGenerator::setup() {
    // Set up the circles
    this->_generate_droplets();

    // Set up the attraction points
    if (this->attraction_points) {
        this->_generate_attraction_points();
    }

    // Set up the rejection points
    if (this->rejection_points) {
        this->_generate_rejection_points();
    }

    // Set up the giant droplet
    if (this->larger_droplet) {
        this->larger_droplet_x = this->screen_width / 5;
        this->larger_droplet_y = this->screen_height / 2;
        this->larger_droplet_dx = this->larger_droplet_speed_x;
        this->larger_droplet_dy = this->larger_droplet_speed_y;

    }
}

void DataGenerator::simulate() {
    // Main game loop
    int counter = 0;

    // Run as long as the counter is running, the number of frames is reached and there are still droplets
    while (counter < this->number_of_frames && this->num_droplets > 0) {
        // Move the giant droplet
        if (this->larger_droplet) {
            this->_move_giant_droplet();
        }

        // Remove droplets
        if (this->disappearing_droplets) {
            this->_remove_droplets();
        }

        // Move the circles
        this->_move_circles(0, this->num_droplets);

        // Move the attraction points
        if (this->attraction_points && this->attraction_movement) {
            this->_move_attraction_points();
        }

        // Move the rejection points
        if (this->rejection_points && this->rejection_movement) {
            this->_move_rejection_points();
        }

        // Update dataframe with position data
        if (counter % (this->number_of_frames / this->number_of_recordings) == 0) {
            // Print circle positions
            for (int i = 0; i < this->num_droplets; i++) {
                std::cout << "Droplet " << i << ": (" << std::get<0>(this->circles[i]) << ", " << std::get<1>(this->circles[i]) << ")" << std::endl;
            }
        }

        // Update counter
        counter += 1;
    }
}

void DataGenerator::_remove_droplets() {
    // Remove droplets with certain probability
    if (rand() < this->disappear_probability) {
        this->num_droplets -= 1;
    }
}

void DataGenerator::_generate_droplets() {
    for (int i = 0; i < this->num_droplets; i++) {
        int x = rand() % (this->screen_width - 2 * this->droplet_radius) + this->droplet_radius;
        int y = rand() % (this->screen_height - 2 * this->droplet_radius) + this->droplet_radius;

        float dx = 0;
        float dy = 0;

        std::mt19937 gen(this->rd());
        std::uniform_int_distribution<> dis(-this->max_intrinsic_velocity, this->max_intrinsic_velocity);
        if (this->intrinsic_movement) {
            dx = dis(gen);
            dy = dis(gen);
        }

        this->circles.push_back(std::make_tuple(x, y, dx, dy));
    }
}

void DataGenerator::_generate_attraction_points() {
    for (int i = 0; i < this->num_attractions; i++) {
        int x = rand() % (this->screen_width - 2 * this->attraction_radius) + this->attraction_radius;
        int y = rand() % (this->screen_height - 2 * this->attraction_radius) + this->attraction_radius;
        
        float dx = 0;
        float dy = 0;

        std::mt19937 gen(this->rd());
        std::uniform_int_distribution<> dis(-this->attraction_speed, this->attraction_speed);
        if (this->attraction_movement) {
            dx = dis(gen);
            dy = dis(gen);
        }
       
        this->attractions.push_back(std::make_tuple(x, y, dx, dy));
    }
}

void DataGenerator::_generate_rejection_points() {
    for (int i = 0; i < this->num_rejections; i++) {
        int x = rand() % (this->screen_width - 2 * this->rejection_radius) + this->rejection_radius;
        int y = rand() % (this->screen_height - 2 * this->rejection_radius) + this->rejection_radius;
        
        float dx = 0;
        float dy = 0;

        std::mt19937 gen(this->rd());
        std::uniform_int_distribution<> dis(-this->rejection_speed, this->rejection_speed);
        if (this->rejection_movement) {
            dx = dis(gen);
            dy = dis(gen);
        }
        
        this->rejections.push_back(std::make_tuple(x, y, dx, dy));
    }
}

void DataGenerator::_move_giant_droplet() {
    this->larger_droplet_x += this->larger_droplet_dx;
    this->larger_droplet_y += this->larger_droplet_dy;
}

void DataGenerator::_move_attraction_points() {
    for (int i = 0; i < this->num_attractions; i++) {
        float x, y, dx, dy;
        std::tie(x, y, dx, dy) = this->attractions[i];

        if (x < this->attraction_radius || x > this->screen_width - this->attraction_radius) {
            dx = -dx;
        }
        if (y < this->attraction_radius || y > this->screen_height - this->attraction_radius) {
            dy = -dy;
        }

        x += dx;
        y += dy;
        this->attractions[i] = std::make_tuple(x, y, dx, dy);
    }
}

void DataGenerator::_move_rejection_points() {
    for (int i = 0; i < this->num_rejections; i++) {
        float x, y, dx, dy;
        std::tie(x, y, dx, dy) = this->rejections[i];

        if (x < this->rejection_radius || x > this->screen_width - this->rejection_radius) {
            dx = -dx;
        }
        if (y < this->rejection_radius || y > this->screen_height - this->rejection_radius) {
            dy = -dy;
        }

        x += dx;
        y += dy;
        this->rejections[i] = std::make_tuple(x, y, dx, dy);
    }
}

void DataGenerator::_move_circles(int start, int end, bool random_movement) {
    for (int i = start; i < end; i++) {
        float x, y, dx, dy;
        std::tie(x, y, dx, dy) = this->circles[i];

        // Apply intrinsic velocities
        x += dx;
        y += dy;

        // Apply random movement
        std::mt19937 gen(this->rd());
        std::uniform_int_distribution<> dis(-this->max_random_velocity, this->max_random_velocity);
        if (random_movement) {
            x += dis(gen);
            y += dis(gen);
        }

        // Apply attraction forces
        if (this->attraction_points) {
            for (int j = 0; j < this->num_attractions; j++) {
                float ax, ay, adx, ady;
                std::tie(ax, ay, adx, ady) = this->attractions[j];
                float dist = sqrt(pow(x - ax, 2) + pow(y - ay, 2));
                if (dist < this->attraction_radius) {
                    float angle = atan2(ay - y, ax - x);
                    x += this->attraction_strength * cos(angle);
                    y += this->attraction_strength * sin(angle);
                }
            }
        }

        // Apply rejection forces
        if (this->rejection_points) {
            for (int j = 0; j < this->num_rejections; j++) {
                float rx, ry, rdx, rdy;
                std::tie(rx, ry, rdx, rdy) = this->rejections[j];
                float dist = sqrt(pow(x - rx, 2) + pow(y - ry, 2));
                if (dist < this->rejection_radius) {
                    float angle = atan2(ry - y, rx - x);
                    x -= this->rejection_strength * cos(angle);
                    y -= this->rejection_strength * sin(angle);
                }
            }
        }

        // Check for collisions with other circles
        for (int j = 0; j < this->num_droplets; j++) {
            float x2, y2, dx2, dy2;
            std::tie(x2, y2, dx2, dy2) = this->circles[j];
            float dist = sqrt(pow(x - x2, 2) + pow(y - y2, 2));

            if (dist < this->droplet_radius * 2) {
                // Adjust velocities to prevent overlap
                float diff_x = x2 - x;
                float diff_y = y2 - y;
                float ndiff_x = diff_x / dist;
                float ndiff_y = diff_y / dist;
                x2 = x + ndiff_x * this->droplet_radius * 2;
                y2 = y + ndiff_y * this->droplet_radius * 2;

                this->circles[j] = std::make_tuple(x2, y2, dx2, dy2);
            }
        }

        // Avoid the giant droplet
        if (this->larger_droplet) {
            float dist = sqrt(pow(x - this->larger_droplet_x, 2) + pow(y - this->larger_droplet_y, 2));
            if (dist < this->larger_droplet_radius + this->droplet_radius) {
                float diff_x = x - this->larger_droplet_x;
                float diff_y = y - this->larger_droplet_y;
                float ndiff_x = diff_x / dist;
                float ndiff_y = diff_y / dist;
                x = this->larger_droplet_x + ndiff_x * (this->larger_droplet_radius + this->droplet_radius);
                y = this->larger_droplet_y + ndiff_y * (this->larger_droplet_radius + this->droplet_radius);
            }
        }

        // Check for collisions with walls
        if (x < this->droplet_radius) {
            x = this->droplet_radius;
            dx = -dx;
        } else if (x > this->screen_width - this->droplet_radius) {
            x = this->screen_width - this->droplet_radius;
            dx = -dx;
        }

        if (y < this->droplet_radius) {
            y = this->droplet_radius;
            dy = -dy;
        } else if (y > this->screen_height - this->droplet_radius) {
            y = this->screen_height - this->droplet_radius;
            dy = -dy;
        }

        // Move circles
        this->circles[i] = std::make_tuple(x, y, dx, dy);
    }
}


int main() {
    DataGenerator data_generator;
    data_generator.setup();
    data_generator.simulate();
    return 0;
}