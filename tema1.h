#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void GenerateHeightMap(int numPoints);
        void UpdateTerrainMesh();
        float GetTerrainHeight(float x);
        void RenderTank(glm::vec3 position, float slopeAngle, float turretAngle,float health, int which);
        void ProjectileHandle(float deltaTimeSeconds);
        std::vector<glm::vec2> CalculateTrajectory(glm::vec2 startPosition, glm::vec2 velocity);
        void RenderTrajectory(glm::vec2 p1, glm::vec2 p2, glm::vec3 color);
        void DeformTerrain(const glm::vec2& collisionPoint, float radius, float depth);
        void ApplyLandslide(float deltaTime);

    protected:
        Mesh* terrainMesh;
        std::vector<float> heightMap;
        float step;
        glm::vec3 tank1Position;
        glm::vec3 tank2Position;
        float turret1Angle;
        float turret2Angle;
        float tankSpeed;
        float turretRotationSpeed;
        float slopeAngle1;
		float slopeAngle2;

        struct Projectile {
            glm::vec2 position;
            glm::vec2 velocity;
            bool active;

            Projectile(const glm::vec2& pos, const glm::vec2& vel)
                : position(pos), velocity(vel), active(true) {}
        };

        std::vector<Projectile> projectiles;
        float gravity;   
        float projectileSpeed; 
        float deformationRadius;  
        float deformationDepth;
        float tank1Health;
        float tank2Health;
		Mesh* line;
       
    };
}   // namespace m1
