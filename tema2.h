#pragma once

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <random>
#include <set>
#include <iterator>

#include "components/simple_scene.h"
#include "tema2-camera.h"
#include "Maze.h"
#include "Enemy.h"

using namespace std;
using namespace implemented;
using namespace enemy;


#define ENEMIES_NO 5
#define GAME_TIME 60
#define Z_FAR		(200.f)
#define Z_NEAR		(0.01f)

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        //void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void DrawPlayer(float deltaTimeSeconds);
        void DrawEnemy(glm::vec3 poz);
        void InitCamera();
        void InitMaze();
        bool solvedMaze();
        void DrawBullet();
        void DrawMaze(vector<vector<int>> grid);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
    protected:
        float transPlayerX, transPlayerY, transPlayerZ;

        vector<vector<int>> grid;
        set<pair<int, int>> playerValidPoz;
        int playerInitialX;
        int playerInitialZ;
        int playerSpeed;

        CameraT2 *camera;
        glm::mat4 projectionMatrix;
        bool thirdPersonCamera;

        CameraT2* HUDCamera;
        glm::mat4 projectionMatrixHUD;

        int mazeBoundUpper;
        int mazeBoundLower;
        
        clock_t currentTime;
        clock_t initialTime;

        int ramainingHealth;

        vector<Enemy*> enemies;

        bool canShoot;
        float bulletDist, maxBulletDist;

        /*vector<Bullet*> bullets;
        bool spawnNewBullet;
        clock_t currentTimeForBullets;
        float fireRate;
        float countForBullets;
        float bulletMaxDistance;
        clock_t currentTime;
        clock_t lastTime;
        clock_t lastBullet;
        float timeCount;
        */


    };
}   // namespace m1
