#include "lab_m1/tema2/tema2.h"

//#include <vector>
//#include <string>
//#include <iostream>

using namespace std;
using namespace m1;
using namespace enemy;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    window->DisablePointer();
    srand(time(0));
    playerSpeed = 5;
    ramainingHealth = MAX_HEALTH;
    // start with camera in third person mode
    thirdPersonCamera = true;
    initialTime = clock();
    canShoot = true;
    bulletDist = 0;
    maxBulletDist = 10;
    InitMaze();
    InitCamera();
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("BodyShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "tema2-shaders", "T2VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "tema2-shaders", "T2FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

}

void Tema2::InitCamera() {
    camera = new CameraT2();
    camera->Set(glm::vec3(playerInitialX, 1.25f, playerInitialZ), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    HUDcamera = new CameraT2();
    HUDcamera->Set(glm::vec3(-5, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    projectionMatrixHUD = glm::ortho(0.f, 5.f, 0.f, 5.f, .01f, 10.f);
}

void Tema2::InitMaze() {
    pair<vector<vector<int>>, set<pair<int, int>>> tmp = GenerateMaze();
    mazeBoundUpper = 2 * GRID_SIZE + 1;
    mazeBoundLower = 1;

    grid = tmp.first;
    playerValidPoz = tmp.second;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++)
            cout << grid[i][j] << " ";
        cout << endl;
    }
    auto it = playerValidPoz.begin();
    std::advance(it, rand() % playerValidPoz.size());
    playerInitialX = 2 * it->first + 2;
    playerInitialZ = 2 * it->second + 2;
    //cout << "initial poz= " << playerInitialX << " " << playerInitialZ << endl;

    playerValidPoz.erase(it);

    for (int i = 0; i < ENEMIES_NO; i++) {
        auto enemy_it = playerValidPoz.begin();
        std::advance(enemy_it, rand() % playerValidPoz.size());
            
        Enemy *newEnemy = new Enemy();
        // initial poz = marginea celulei (stg jos)
        newEnemy->initialPoz = glm::vec3(2 * enemy_it->first + 0.5, 1, 2 * enemy_it->second + 0.5);
        newEnemy->currentPoz = glm::vec3(2 * enemy_it->first + 0.5, 1, 2 * enemy_it->second + 0.5);
        newEnemy->direction = 1;
        cout << enemy_it->first << " " << enemy_it->second << endl;
        cout << newEnemy->initialPoz.x << " " << newEnemy->initialPoz.z << endl;
        cout << endl;
        enemies.push_back(newEnemy);
    }    
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // ========================================================================

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex normal attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    // ========================================================================

    // Unbind the VAO
    glBindVertexArray(0);

// Check for OpenGL errors
CheckOpenGLError();

// Mesh information is saved into a Mesh object
meshes[name] = new Mesh(name);
meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
meshes[name]->vertices = vertices;
meshes[name]->indices = indices;
return meshes[name];
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.6, 0.8, 1, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

bool Tema2::solvedMaze() {
    if (camera->GetTargetPosition().x >= mazeBoundUpper || camera->GetTargetPosition().x <= mazeBoundLower ||
        camera->GetTargetPosition().z >= mazeBoundUpper || camera->GetTargetPosition().z <= mazeBoundLower) {
        return true;
    }
    return false;
}

bool Tema2::BulletEnemyCollision(bullet b, Enemy* e) {
    return  ((e->currentPoz.x + 1.25f >= b.position.x && b.position.x + 0.25f >= e->currentPoz.x)
        && (e->currentPoz.y + 1.25f >= b.position.y && b.position.y + 0.25f >= e->currentPoz.y)
        && (e->currentPoz.z + 1.25f >= b.position.z && b.position.z + 0.25f >= e->currentPoz.z));
}

bool Tema2::EnemyPlayerCollision(Enemy* e) {
    glm::vec3 playerPoz = getPlayerLocation();
    return (playerPoz.x + 0.5 >= e->currentPoz.x && e->currentPoz.x + 1.25f >= playerPoz.x) 
            && (playerPoz.z + 0.5 >= e->currentPoz.z && e->currentPoz.z + 1.25f >= playerPoz.z);
}

bool Tema2::PlayerWallCollision(int wallX, int wallZ) {
    /*
bool CheckPlayerWallCollision(float playerX, float playerY, float playerZ, gridSquare s) {
    bool collisionX = playerX >= s.x * 4 && s.x + s.length >= playerX;
    bool collisionZ = playerZ >= s.z * 4 && s.z + s.length >= playerZ;
    return (collisionX && collisionZ);
}
*/
}

glm::vec3 Tema2::getPlayerLocation() {
    if (thirdPersonCamera) {
        return camera->GetTargetPosition();
    }
    else {
        return camera->position;
    }
}

void Tema2::Update(float deltaTimeSeconds)
{
    DrawMaze(grid, deltaTimeSeconds);

    currentTime = clock();
    if (currentTime - initialTime >= GAME_TIME * CLOCKS_PER_SEC) {
        cout << "GAME OVER, TIME'S UP!" << endl;
        exit(0);
    }

    // Healthbar
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, HUDcamera->position + glm::vec3(1, 1, 0.6));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.5f + (ramainingHealth-70)/50));
    RenderSimpleMeshHUD(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0.45f, 0.25f, 0.55f));
    

    // Timer
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, HUDcamera->position + glm::vec3(1, 1.7, 0.6));
    if ((5.5f - 0.15 * (currentTime - initialTime) / CLOCKS_PER_SEC) > 0) {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 5.5f - 0.15 * ((currentTime - initialTime) / CLOCKS_PER_SEC)));
        RenderSimpleMeshHUD(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0.85f, 0.85f, 0.85f));
    }
       

    if (ramainingHealth <= 0) {
        cout << "GAME OVER, YOU DIED!" << endl;
        exit(0);
    }

    /*if (solvedMaze()) {
        cout << "CONGRATULATIONS! YOU WIN!" << endl;
        exit(0);
    }*/

    if (thirdPersonCamera) {
        DrawPlayer(deltaTimeSeconds);

    }

    if (!thirdPersonCamera) {
        if (!canShoot) {
            bool hitEnemy = false;
            int enemyHit = -1;
            for (int i = 0; i < enemies.size(); i++) {
                if (BulletEnemyCollision(currentBullet, enemies[i])) {
                    cout << "coliz bullet inamic";
                    hitEnemy = true;
                    enemyHit = i;
                    break;
                }
            }
            if (enemyHit != -1) {
                enemies.erase(enemies.begin() + enemyHit);
            }
            if (bulletDist < maxBulletDist && !hitEnemy) {
                bulletDist += 10 * deltaTimeSeconds;
                currentBullet.position += currentBullet.direction * 0.5f;
                DrawBullet();
            }
            else {
                bulletDist = 0;
                canShoot = true;
            }
        }
    }

    for (int i = 0; i < enemies.size(); i++) {
        Enemy* e = enemies[i];
        if (EnemyPlayerCollision(e)) {
            ramainingHealth -= 0.5f;
            break;
        }
    }
    
}

void Tema2::DrawMaze(vector<vector<int>> grid, float deltaTimeSeconds) {
    glm::mat4 floorMatrix = glm::mat4(1);
    floorMatrix = glm::translate(floorMatrix, glm::vec3(GRID_SIZE, -0.25f, GRID_SIZE));
    floorMatrix = glm::scale(floorMatrix, glm::vec3(2*GRID_SIZE, 0.25f, 2*GRID_SIZE));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], floorMatrix, glm::vec3(0, 0, 0.2f));

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 1) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(2*i + 1, 1, 2*j + 1));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 2, 2));
                RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.1f, 0.2f));
            }
        }  
    }

    for (int i = 0; i < enemies.size(); i++) {
        Enemy* e = enemies[i];
        MoveEnemy(e, deltaTimeSeconds);
        DrawEnemy(e->currentPoz);
        
    }
}

void Tema2::MoveEnemy(Enemy *e, float deltaTimeSeconds) {
    glm::vec3 current = e->currentPoz;
    int dir = e->direction;
    // up
    if (dir == 1) {
        if (current.z + deltaTimeSeconds > e->initialPoz.z + 1) {
            e->direction = 2;
            return;
        }
        else {
            current.z += deltaTimeSeconds;
            e->currentPoz = current;
        }
    }
    //// left
   if (dir == 2) {
       if (current.x + deltaTimeSeconds > e->initialPoz.x + 1) {
           e->direction = 3;
           return;
       }
       else {
           current.x += deltaTimeSeconds;
           e->currentPoz = current;
       }
   }
    // down
    if (dir == 3) {
        if (current.z - deltaTimeSeconds < e->initialPoz.z) {
            e->direction = 4;
            return;
        }
        else {
            current.z -= deltaTimeSeconds;
            e->currentPoz = current;
        }
    }
   
    //// right
    if (dir == 4) {
        if (current.x - deltaTimeSeconds < e->initialPoz.x) {
            e->direction = 1;
            return;
        }
        else {
            current.x -= deltaTimeSeconds;
            e->currentPoz = current;
        }
    }
}

void Tema2::DrawEnemy(glm::vec3 poz) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, poz);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1));
    RenderSimpleMesh(meshes["sphere"], shaders["BodyShader"], modelMatrix, glm::vec3(0.2f, 0, 0.2f));
}

void Tema2::DrawBullet() {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(currentBullet.position));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
    RenderSimpleMesh(meshes["sphere"], shaders["BodyShader"], modelMatrix, glm::vec3(1, 1, 1));


}

void Tema2::DrawPlayer(float deltaTimeSeconds) {
    //corp
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.1f, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.7f, 0.2f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.5f, 0.2f));

    //cap
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.35f, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0.7f, 0.7f, 0.3f));

    //picioare
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.2, -0.7f, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.5f, 0.2f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.4f ,0.8f));

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.2f, -0.7f, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.5f, 0.2f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.4f, 0.8f));

    //// maini
    //modelMatrix = glm::mat4(1);
    //modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
    //modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3f, 1, 0));
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
    //RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.5, 0.2));

    //modelMatrix = glm::mat4(1);
    //modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
    //modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3f, 1, 0));
    //modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
    //RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.5, 0.2));

}

void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    GLint locObject = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(locObject, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderSimpleMeshHUD(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    GLint locObject = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(locObject, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = HUDcamera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrixHUD));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // Add key press event
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        camera->TranslateRight(playerSpeed * deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        camera->TranslateRight(-playerSpeed * deltaTime);

    }
    if (window->KeyHold(GLFW_KEY_UP)) {
        camera->MoveForward(playerSpeed * deltaTime);

    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        camera->MoveForward(-playerSpeed * deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        camera->TranslateUpward(-1 * deltaTime * playerSpeed);
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        camera->TranslateUpward(1 * deltaTime * playerSpeed);
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_LEFT_CONTROL) {
        if (thirdPersonCamera) {
            camera->position = glm::vec3(camera->position.x + camera->forward.x, camera->position.y, camera->position.z + camera->forward.z);
        }
        else {
            camera->position = glm::vec3(camera->position.x - camera->forward.x, camera->position.y, camera->position.z - camera->forward.z);
        }
        thirdPersonCamera = !thirdPersonCamera;
    }

    if (!thirdPersonCamera && key == GLFW_KEY_SPACE && canShoot) {
        // shoot bullet
        cout << "shoot" << endl;
        currentBullet.position = camera->GetTargetPosition();
        currentBullet.direction = camera->forward;
        canShoot = false;
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
    
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        if (!thirdPersonCamera) {
            camera->RotateFirstPerson_OX(-0.0015f * deltaY);
            camera->RotateFirstPerson_OY(-0.0015f * deltaX);
        }
        else {
            camera->RotateThirdPerson_OX(-0.0015f * deltaY);
            camera->RotateThirdPerson_OY(-0.0015f * deltaX);

        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

