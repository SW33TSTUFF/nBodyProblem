#include <iostream>
#include <vector>
#include <cmath> // For sqrt, atan2, cos, sin
#include <raylib.h>

using namespace std;

const double G = 6.674e-11;
const double AU = 149.6e6 * 1000;
double scale = 1500 / AU;
double timestep = 3600 * 24 / 800; // 1 day
int count = 1000;
// min fling distance 1.560086e11


const int screenWidth = 1440;
const int screenHeight = 810;

float getSeperation(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

class Planet {
public:
    float xPos;
    float yPos;
    float radius;
    Color color;
    float mass;
    vector<Vector2> orbit;
    bool sun = false;
    double distanceToSun = 0;
    float velocity_x = 0;
    float velocity_y = 0;

    Planet(float x, float y, float radius, Color color, float mass)
        : xPos(x), yPos(y), radius(radius), color(color), mass(mass) {}

    void Draw() {
        // Calculate position
        float x = xPos * scale + screenWidth / 2;
        float y = yPos * scale + screenHeight / 2;

        // Draw orbit
        if (orbit.size() > 2) {
            // Updated points for orbit
            vector<Vector2> updatedPoints;
            for (auto& point : orbit) {
                float updatedX = point.x * scale + screenWidth / 2;
                float updatedY = point.y * scale + screenHeight / 2;
                updatedPoints.push_back({ updatedX, updatedY });
            }
            // Draw orbit lines
            for (size_t i = 0; i < updatedPoints.size() - 1; ++i) {
                DrawLineEx(updatedPoints[i], updatedPoints[i + 1], 2, color);
                if(count%50) {
                    count++;
                    updatedPoints.erase(updatedPoints.begin(), updatedPoints.begin());

                }
                count++;
            }
        }

        // Draw planet
        DrawCircle(x, y, radius, color);

        // Draw distance text
        if (!sun) {
            // Implement your own method to draw text on Raylib window
            DrawText(TextFormat("%.1f km", distanceToSun / 1000), x + 20, y, 10, WHITE);
        }
    }

    std::pair<float, float> attraction(Planet& other_planet) {
        float other_planet_x = other_planet.xPos;
        float other_planet_y = other_planet.yPos;

        float horizontal_separation = other_planet_x - xPos;
        float vertical_separation = other_planet_y - yPos;

        float separation = sqrt(horizontal_separation * horizontal_separation + vertical_separation * vertical_separation);

        if (other_planet.sun) {
            distanceToSun = separation;
        }

        float force = G * mass * other_planet.mass / (separation * separation); // F = GMm/r^2
        float theta = atan2(vertical_separation, horizontal_separation);

        float force_x = force * cos(theta);
        float force_y = force * sin(theta);

        return std::make_pair(force_x, force_y);
    }

    void update_position(const vector<Planet>& planets) {
        float total_fx = 0;
        float total_fy = 0;

        for (const Planet& planet : planets) {
            if (this == &planet) continue;

            auto [fx, fy] = attraction(const_cast<Planet&>(planet));
            total_fx += fx;
            total_fy += fy;
        }

        velocity_x += (total_fx / mass) * timestep;
        velocity_y += (total_fy / mass) * timestep;

        xPos += velocity_x * timestep;
        yPos += velocity_y * timestep;

        // for (const Planet& planet : planets) {
        //     if (this == &planet) continue;
        //     // This somewhat works
        //     cout << planet.mass << "-" << planet.xPos << "," << planet.yPos << "|" << planet.velocity_x << planet.velocity_y << endl;
        //     if (getSeperation(this->xPos, this->yPos, planet.xPos, planet.yPos) <= 1e11) {
        //         this->mass = 0;
        //         this->radius = 0;
        //     }
        // }

        orbit.push_back({ xPos, yPos });
    }

    void canCollide(vector<Planet>& planets) {
        for (size_t i = 0; i < planets.size(); i++) {
            for (size_t j = i + 1; j < planets.size(); j++) {
                float distance = sqrt(pow(planets[i].xPos - planets[j].xPos, 2) + pow(planets[i].yPos - planets[j].yPos, 2));
                if (distance < (0.01*AU)) {
                    // Handle collision

                    // Update velocities and masses
                    planets[i].velocity_x = 0;
                    planets[i].velocity_y = 0;
                    planets[j].velocity_x = 0;
                    planets[j].velocity_y = 0;
                    planets[i].mass = 0;
                    planets[j].mass = 0;
                    
                    // Adjust positions to avoid overlap
                    float overlap = (planets[i].radius + planets[j].radius) - distance;
                    float dx = (planets[i].xPos - planets[j].xPos) / distance;
                    float dy = (planets[i].yPos - planets[j].yPos) / distance;
                    planets[i].xPos += overlap * dx;
                    planets[i].yPos += overlap * dy;
                    planets[j].xPos -= overlap * dx;
                    planets[j].yPos -= overlap * dy;
                }
            }
        }
    }


};


int main() {
    InitWindow(screenWidth, screenHeight, "SIMULATION 1.0.0");
    // SetTargetFPS(60);

    // Initialize planets
    Planet sun(0, 0, 10, ORANGE, 6e30);
    // Planet sun2(0, 0, 10, RED, 6e30);
    // Planet x1(-1 * AU, 0, 4, PURPLE, 5.972e32);
    // Planet x2(-1 * AU, -1 * AU, 2, BLUE, 2.972e31);
    // Planet x3(-1 * AU, 0.6 * AU, 3, GRAY, 5.972e31);
    // Planet x4(0 * AU, 0.6 * AU, 3, GRAY, 5.972e31);
    sun.sun = true;


    Planet x1(-0.4 * AU, -1 * AU, 5, DARKGREEN, 5.972e32);
    Planet x2(-0.4 * AU, 0, 6, BROWN, 5.972e32);
    Planet x3(-0.2 * AU, AU , 8, WHITE, 5.972e32);
    // Planet x4(1 * AU, -1 * AU, 4, DARKBROWN, 5.972e32);
    // sun.velocity_y = sqrt(G*sun.mass/AU*0.5);
    // sun2.velocity_y = -sqrt(G*sun.mass/AU*0.5);
    // x1.velocity_x = 20 * 100;
    // x1.velocity_y = -30.783 * 100;
    x2.velocity_x = 40 * 10000;
    // x2.velocity_y = 45.783 * 100;
    // x3.velocity_x = 120 * 10000;
    // x3.velocity_y = 100 * 100;
    // x4.velocity_x = -10 * 100;
    // x4.velocity_y = 100 * 100;

    vector<Planet> planets = {x1, x2, x3};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Update and draw planets
        for (Planet& planet : planets) {
            planet.update_position(planets);
            planet.canCollide(planets);
            planet.Draw();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
