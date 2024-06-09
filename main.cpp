#include <iostream>
#include <vector>
#include <cmath> // For sqrt, atan2, cos, sin
#include <raylib.h>

using namespace std;

const double G = 6.674e-11;
const double AU = 149.6e6 * 1000;
double scale = 250 / AU;
double timestep = 3600 * 24; // 1 day

const int screenWidth = 1440;
const int screenHeight = 810;

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

        orbit.push_back({ xPos, yPos });
    }
};

int main() {
    InitWindow(screenWidth, screenHeight, "SIMULATION 1.0.0");
    SetTargetFPS(10);

    // Initialize planets
    Planet sun(0, 0, 30, ORANGE, 1.989e30);
    Planet earth(-1 * AU, 0, 16, BLUE, 5.972e24);
    Planet mars(-1.534 * AU, 0, 12, RED, 6.39e23);
    Planet mercury(0.387 * AU, 0, 8, DARKGRAY, 3.30e23);
    Planet venus(0.723 * AU, 0, 14, WHITE, 4.8685e24);
    sun.sun = true;

    earth.velocity_y = 29.783 * 1000;
    mars.velocity_y = 24.077 * 1000;
    mercury.velocity_y = -47.4 * 1000;
    venus.velocity_y = -35.02 * 1000;

    vector<Planet> planets = { sun, earth, mars, mercury, venus };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Update and draw planets
        for (Planet& planet : planets) {
            planet.update_position(planets);
            planet.Draw();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
