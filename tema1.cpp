#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>
#include <cmath>

#include "lab_m1/tema1/transform2D.h"
#include "lab_m1/tema1/object2D.h"

using namespace std;
using namespace m1;


Tema1::Tema1()
{
    terrainMesh = NULL;
    heightMap = std::vector<float>();
    tank1Position = glm::vec3(100, 0, 0);
    tank2Position = glm::vec3(1150, 0, 0);
    turret1Angle = -0.2f;
    turret2Angle = -2.9f;
    slopeAngle1 = 0.0f;
    slopeAngle2 = 0.0f;
    tankSpeed = 100.0f;
    turretRotationSpeed = 1.0f;
    step = 0.0f;
    gravity = 325.0f;
    projectileSpeed = 400.0f;
    tank1Health = 100.0f;
    tank2Health = 100.0f;
    line = NULL;
	deformationDepth = 30.0f;
    deformationRadius = 40.0f;
}


Tema1::~Tema1()
{
    delete terrainMesh;
}

void Tema1::GenerateHeightMap(int numPoints) {
    // Main wave for broad, smooth hills in the center
    float amplitude1 = 150.0f;
    float frequency1 = 0.003f;

    // Secondary wave for mild variations (small peaks)
    float amplitude2 = 50.0f;
    float frequency2 = 0.02f;

    float amplitude3 = 10.0f;
    float frequency3 = 0.05f;

    // Additional terms to create higher sides
    float sideSlope = 0.1f;
    float sideAmplitude = 50.0f;
    float baseHeight = 50.0f;

    for (int i = 0; i < numPoints; ++i) {
        float t = i * step;

        float height = amplitude1 * sin(frequency1 * t)
            + amplitude2 * sin(frequency2 * t) + amplitude3 * sin(frequency3 * t);

        // Apply linear slope for each side
        float normalizedPosition = static_cast<float>(i) / (numPoints - 1);
        float sideHeight = sideAmplitude * (1.0f - abs(2.0f * normalizedPosition - 1.0f));

        height += sideSlope * t + sideHeight + baseHeight;
        heightMap.push_back(height);
    }
}

void Tema1::UpdateTerrainMesh() {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i < heightMap.size(); ++i) {
        glm::vec3 topVertex(i * step, heightMap[i], 0.0f);
        vertices.emplace_back(topVertex, glm::vec3(0.65f, 0.5f, 0.39f));

        glm::vec3 bottomVertex(i * step, 0.0f, 0.0f);
        vertices.emplace_back(bottomVertex, glm::vec3(0.82f, 0.71f, 0.55f));
    }

    for (int i = 0; i < heightMap.size() - 1; ++i) {
        indices.push_back(i * 2);
        indices.push_back(i * 2 + 1);
        indices.push_back((i + 1) * 2);
        indices.push_back((i + 1) * 2 + 1);
    }

    if (!terrainMesh) {
        terrainMesh = new Mesh("terrainMesh");
        terrainMesh->SetDrawMode(GL_TRIANGLE_STRIP);
    }
    terrainMesh->InitFromData(vertices, indices);
}

void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // Generate height map for the terrain once
    int numPoints = 100;
    step = static_cast<float>(resolution.x) / (numPoints - 1);
    GenerateHeightMap(numPoints);
    UpdateTerrainMesh();

    // Create tank1
    Mesh* base1_top = object2D::CreateTrapezoid("base1_top", glm::vec3(-40, -15, 0), 80, 50, 20, glm::vec3(0.8f, 0.2f, 0.2f));
    Mesh* base1_bottom = object2D::CreateTrapezoid("base1_bottom", glm::vec3(-30, -15, 0), 60, 40, 10, glm::vec3(0.0f, 0.0f, 0.0f));
    AddMeshToList(base1_top);
    AddMeshToList(base1_bottom);
    Mesh* turret1 = object2D::CreateArc("turret1", 12, glm::vec3(0, 0, 0), glm::vec3(0.8f, 0.2f, 0.2f), true);
    AddMeshToList(turret1);
    Mesh* cannon1 = object2D::CreateRectangle("cannon1", glm::vec3(0, -2, 0), 35, 5, glm::vec3(0.0f, 0.0f, 0.0f));
    AddMeshToList(cannon1);

    // Create tank2
    Mesh* base2_top = object2D::CreateTrapezoid("base2_top", glm::vec3(-40, -15, 0), 80, 50, 20, glm::vec3(0.2f, 0.8f, 0.2f));
    Mesh* base2_bottom = object2D::CreateTrapezoid("base2_bottom", glm::vec3(-30, -15, 0), 60, 40, 10, glm::vec3(0.0f, 0.0f, 0.0f));
    AddMeshToList(base2_top);
    AddMeshToList(base2_bottom);
    Mesh* turret2 = object2D::CreateArc("turret2", 12, glm::vec3(0, 0, 0), glm::vec3(0.2f, 0.8f, 0.2f), true);
    AddMeshToList(turret2);
    Mesh* cannon2 = object2D::CreateRectangle("cannon2", glm::vec3(0, -2, 0), 35, 5, glm::vec3(0.0f, 0.0f, 0.0f));
    AddMeshToList(cannon2);

    // Create a projectile
    Mesh* projectile = object2D::CreateCircle("projectile", glm::vec3(0, 0, 0), 5, glm::vec3(0.5f, 0.5f, 0.5f), 50);
    AddMeshToList(projectile);

	// Create health bar
    Mesh* healthBarOutline = object2D::CreateRectangleOutline("healthBarOutline", glm::vec3(0, 0, 0), 60.0f, 15.0f, glm::vec3(0, 0, 0));
    AddMeshToList(healthBarOutline);
    Mesh* healthBarFill = object2D::CreateRectangle("healthBarFill", glm::vec3(0, 0, 0), 60.0f, 15.0f, glm::vec3(1, 1, 1));
    AddMeshToList(healthBarFill);

}

void Tema1::FrameStart()
{
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

float Tema1::GetTerrainHeight(float x) {
    // Calculate index in the height map
    float indexFloat = x / step;
    int index = static_cast<int>(indexFloat);

    if (index < 0) {
        return heightMap[0];
    }
    if (index >= heightMap.size() - 1) {
        return heightMap.back();
    }

    float t = indexFloat - index;
    float height = (1 - t) * heightMap[index] + t * heightMap[index + 1];

    return height;
}

void Tema1::RenderTank(glm::vec3 position, float slopeAngle, float turretAngle, float health, int which) {
    if (which == 1) {
        glm::mat3 modelMatrix = glm::mat3(1.0f);
        modelMatrix *= transform2D::Translate(position.x, position.y);
        modelMatrix *= transform2D::Rotate(-slopeAngle);
        RenderMesh2D(meshes["base1_top"], shaders["VertexColor"], modelMatrix);

        glm::mat3 bottomMatrix = modelMatrix;
        bottomMatrix *= transform2D::Translate(0, -30.f);
        bottomMatrix *= transform2D::Scale(1, -1);
        RenderMesh2D(meshes["base1_bottom"], shaders["VertexColor"], bottomMatrix);

        glm::mat3 turretMatrix = modelMatrix;
        turretMatrix *= transform2D::Translate(0, 4);
        RenderMesh2D(meshes["turret1"], shaders["VertexColor"], turretMatrix);

        glm::mat3 cannonMatrix = turretMatrix;
        cannonMatrix *= transform2D::Rotate(turret1Angle);
        RenderMesh2D(meshes["cannon1"], shaders["VertexColor"], cannonMatrix);

        glm::mat3 healthBarMatrix = glm::mat3(1.0f);
        healthBarMatrix *= transform2D::Translate(tank1Position.x - 50, tank1Position.y + 40);
        RenderMesh2D(meshes["healthBarOutline"], shaders["VertexColor"], healthBarMatrix);
        glm::mat3 healthBarFilledMatrix = healthBarMatrix;
        float tank1HealthScale = tank1Health / 100.0f; 
        healthBarFilledMatrix *= transform2D::Scale(tank1HealthScale, 1.0f);
        RenderMesh2D(meshes["healthBarFill"], shaders["VertexColor"], healthBarFilledMatrix);

    }
    else if (which == 2) {
        glm::mat3 modelMatrix = glm::mat3(1.0f);
        modelMatrix *= transform2D::Translate(position.x, position.y);
        modelMatrix *= transform2D::Rotate(-slopeAngle); 
        RenderMesh2D(meshes["base2_top"], shaders["VertexColor"], modelMatrix);

        glm::mat3 bottomMatrix = modelMatrix;
        bottomMatrix *= transform2D::Translate(0, -30.f); 
        bottomMatrix *= transform2D::Scale(1, -1); 
        RenderMesh2D(meshes["base2_bottom"], shaders["VertexColor"], bottomMatrix);

        glm::mat3 turretMatrix = modelMatrix;
        turretMatrix *= transform2D::Translate(0, 4); 
        RenderMesh2D(meshes["turret2"], shaders["VertexColor"], turretMatrix);

        glm::mat3 cannonMatrix = turretMatrix;
        cannonMatrix *= transform2D::Rotate(turret2Angle); 
        RenderMesh2D(meshes["cannon2"], shaders["VertexColor"], cannonMatrix);

        glm::mat3 healthBarMatrix = glm::mat3(1.0f);
        healthBarMatrix *= transform2D::Translate(tank2Position.x, tank2Position.y + 40); 
        RenderMesh2D(meshes["healthBarOutline"], shaders["VertexColor"], healthBarMatrix);
        glm::mat3 healthBarFilledMatrix2 = healthBarMatrix;
        float tank2HealthScale = tank2Health / 100.0f; 
        healthBarFilledMatrix2 *= transform2D::Scale(tank2HealthScale, 1.0f); 
        RenderMesh2D(meshes["healthBarFill"], shaders["VertexColor"], healthBarFilledMatrix2);
    }
}

void Tema1::ApplyLandslide(float deltaTime) {
    float threshold = 20.0f;
    float epsilon = 25.0f * deltaTime; 

    for (int i = 0; i < heightMap.size() - 1; ++i) {
        float heightDifference = heightMap[i] - heightMap[i + 1];

        if (abs(heightDifference) > threshold) {
            if (heightDifference > 0) {
                // Transfer height from i to i+1
                float transferAmount = min(epsilon, heightDifference - threshold);
                heightMap[i] -= transferAmount;
                heightMap[i + 1] += transferAmount;
            }
            else {
                // Transfer height from i+1 to i
                float transferAmount = min(epsilon, -heightDifference - threshold);
                heightMap[i] += transferAmount;
                heightMap[i + 1] -= transferAmount;
            }
        }
    }
    UpdateTerrainMesh();
}


void Tema1::DeformTerrain(const glm::vec2& collisionPoint, float radius, float depth) {
    int startX = static_cast<int>((collisionPoint.x - radius) / step);
    int endX = static_cast<int>((collisionPoint.x + radius) / step);

    for (int i = startX; i <= endX; ++i) {
        if (i < 0 || i >= heightMap.size()) {
            continue;
        }

        float distance = abs(i * step - collisionPoint.x);
        if (distance < radius) {
            float deformation = depth * exp(-pow(distance / radius, 2));
            heightMap[i] = max(0.0f, heightMap[i] - deformation);
        }
    }
    UpdateTerrainMesh();
}

void Tema1::ProjectileHandle(float deltaTimeSeconds) {
    float projectileRadius = 5.0f;
    float tankRadius = 30.0f;

    for (auto& projectile : projectiles) {
        if (projectile.active) {
            // Update position and velocity with gravity
            projectile.position += projectile.velocity * deltaTimeSeconds;
            projectile.velocity.y -= gravity * deltaTimeSeconds;

            glm::mat3 modelMatrix = glm::mat3(1.0f);
            modelMatrix *= transform2D::Translate(projectile.position.x, projectile.position.y);
            RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);

            // Check collision with terrain
            float terrainHeight = GetTerrainHeight(projectile.position.x);
            if (projectile.position.y <= terrainHeight) {
                projectile.active = false;
                DeformTerrain(projectile.position, deformationRadius, deformationDepth);
            }

            // Check collision with Tank 1
            float distanceToTank1 = glm::distance(projectile.position, glm::vec2(tank1Position.x, tank1Position.y));
            if (distanceToTank1 <= (projectileRadius + tankRadius)) {
                projectile.active = false;
                tank1Health -= 20.0f;
            }

            // Check collision with Tank 2
            float distanceToTank2 = glm::distance(projectile.position, glm::vec2(tank2Position.x, tank2Position.y));
            if (distanceToTank2 <= (projectileRadius + tankRadius)) {
                projectile.active = false;
                tank2Health -= 20.0f; 
            }
        }
    }
    // Remove inactive projectiles
    for (auto p = projectiles.begin(); p != projectiles.end(); ) {
        if (!p->active) {
            p = projectiles.erase(p);
        }
        else {
            p++;
        }
    }
	// Render projectile trajectories
    float angle1 = -(turret1Angle - slopeAngle1);
    glm::vec2 startPosition1 = glm::vec2(tank1Position.x + cos(angle1) * 35.0f, tank1Position.y + 5 + sin(angle1) * 35.0f);
    glm::vec2 velocity1 = glm::vec2(cos(angle1), sin(angle1)) * projectileSpeed;
    std::vector<glm::vec2> trajectory1 = CalculateTrajectory(startPosition1, velocity1);

    for (size_t i = 0; i < trajectory1.size() - 1; ++i) {
        glm::vec2 p1 = trajectory1[i];
        glm::vec2 p2 = trajectory1[i + 1];
        RenderTrajectory(p1, p2, glm::vec3(1, 0, 0));
    }

    float angle2 = -(turret2Angle - slopeAngle2);
    glm::vec2 startPosition2 = glm::vec2(tank2Position.x + cos(angle2) * 35.0f, tank2Position.y + 5 + sin(angle2) * 35.0f);
    glm::vec2 velocity2 = glm::vec2(cos(angle2), sin(angle2)) * projectileSpeed;
    std::vector<glm::vec2> trajectory2 = CalculateTrajectory(startPosition2, velocity2);

    for (size_t i = 0; i < trajectory2.size() - 1; ++i) {
        glm::vec2 p1 = trajectory2[i];
        glm::vec2 p2 = trajectory2[i + 1];
        RenderTrajectory(p1, p2, glm::vec3(0, 1, 0));
    }
}

std::vector<glm::vec2> Tema1::CalculateTrajectory(glm::vec2 startPosition, glm::vec2 velocity) {
    std::vector<glm::vec2> trajectory;
    glm::vec2 position = startPosition;
    glm::vec2 currentVelocity = velocity;
    float projectileRadius = 5.0f;
    float tankRadius = 30.0f;

    for (int i = 0; i < 100; ++i) {
        trajectory.push_back(position);
        // Check for collision with terrain
        float terrainHeight = GetTerrainHeight(position.x);
        if (position.y + 5 < terrainHeight) {
            break;
        }
        // Check for collision with tanks
        float distanceToTank1 = glm::distance(position, glm::vec2(tank1Position.x, tank1Position.y));
        if (distanceToTank1 + 5 <= projectileRadius + tankRadius) {
            break;
        }
        float distanceToTank2 = glm::distance(position, glm::vec2(tank2Position.x, tank2Position.y));
        if (distanceToTank2 + 5 <= projectileRadius + tankRadius) {
            break;
        }
        position += currentVelocity * 0.04f;
        currentVelocity.y -= gravity * 0.04f;
    }

    return trajectory;
}

void Tema1::RenderTrajectory(glm::vec2 p1, glm::vec2 p2, glm::vec3 color) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(p1, 0), color),
        VertexFormat(glm::vec3(p2, 0), color)
    };
    std::vector<unsigned int> indices = { 0, 1 };

    if (!line) {
        line = new Mesh("line");
        line->SetDrawMode(GL_LINES);
    }
    line->InitFromData(vertices, indices);
    RenderMesh2D(line, shaders["VertexColor"], glm::mat3(1.0f));
}


void Tema1::Update(float deltaTimeSeconds) {
    glm::mat3 modelMatrix = glm::mat3(1.0f);
    RenderMesh2D(terrainMesh, shaders["VertexColor"], modelMatrix);

    ApplyLandslide(deltaTimeSeconds);

    // Render tank 1 (Red)
    float tank1X = tank1Position.x;
    tank1Position.y = GetTerrainHeight(tank1X);
    // Calculate slope for tank orientation
    float offset = 25.0f;
    glm::vec2 A1(tank1X - offset, GetTerrainHeight(tank1X - offset));
    glm::vec2 B1(tank1X + offset, GetTerrainHeight(tank1X + offset));
    glm::vec2 V1 = B1 - A1; 
    slopeAngle1 = atan2(V1.y, V1.x);
    float tank1Offset = 24.0f / cos(slopeAngle1);
    tank1Position.y += tank1Offset;

    if (tank1Health > 0) {
        RenderTank(tank1Position, slopeAngle1, turret1Angle, tank1Health, 1);
    }
    else tank1Position.x = -1000;

    // Render tank 2 (Green)
    float tank2X = tank2Position.x;
    tank2Position.y = GetTerrainHeight(tank2X);
    glm::vec2 A2(tank2X - offset, GetTerrainHeight(tank2X - offset));
    glm::vec2 B2(tank2X + offset, GetTerrainHeight(tank2X + offset));
    glm::vec2 V2 = B2 - A2;
    slopeAngle2 = atan2(V2.y, V2.x);
    float tank2Offset = 24.0f / cos(slopeAngle2);
    tank2Position.y += tank2Offset;
    if (tank2Health > 0) {
        RenderTank(tank2Position, slopeAngle2, turret2Angle, tank2Health, 2);
    }
    else tank2Position.x = -1000;

    ProjectileHandle(deltaTimeSeconds);
}

void Tema1::FrameEnd()
{
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Tank 1 controls
    if (window->KeyHold(GLFW_KEY_A)) {
        if (tank1Position.x - tankSpeed * deltaTime > 70) {
            tank1Position.x -= tankSpeed * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        if (tank1Position.x + tankSpeed * deltaTime < 1200) {
            tank1Position.x += tankSpeed * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_W)) {
        if (turret1Angle - turretRotationSpeed * deltaTime >= -2.9f) {
            turret1Angle -= turretRotationSpeed * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        if (turret1Angle + turretRotationSpeed * deltaTime <= -0.2f) {
            turret1Angle += turretRotationSpeed * deltaTime;
        }
    }

    // Tank 2 controls
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        if (tank2Position.x - tankSpeed * deltaTime > 70) {
            tank2Position.x -= tankSpeed * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        if (tank2Position.x + tankSpeed * deltaTime < 1200) {
            tank2Position.x += tankSpeed * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_UP)) {
        if (turret2Angle + turretRotationSpeed * deltaTime <= -0.2f) {
            turret2Angle += turretRotationSpeed * deltaTime;
        }
    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        if (turret2Angle - turretRotationSpeed * deltaTime >= -2.9f) {
            turret2Angle -= turretRotationSpeed * deltaTime;
        }
    }

}

void Tema1::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE) {
        float angle = -(turret1Angle - slopeAngle1);
        glm::vec2 position = glm::vec2(tank1Position.x + cos(angle) * 35.0f,
            tank1Position.y + 5 + sin(angle) * 35.0f);
        glm::vec2 velocity = glm::vec2(cos(angle), sin(angle)) * projectileSpeed;

        projectiles.emplace_back(position, velocity);
    }

    if (key == GLFW_KEY_ENTER) {
        float angle = -(turret2Angle - slopeAngle2);
        glm::vec2 position = glm::vec2(tank2Position.x + cos(angle) * 35.0f,
            tank2Position.y + 5 + sin(angle) * 35.0f);
        glm::vec2 velocity = glm::vec2(cos(angle), sin(angle)) * projectileSpeed;

        projectiles.emplace_back(position, velocity);
    }
}

void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{

}
