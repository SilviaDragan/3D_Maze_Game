#include "lab_m1/tema2/tema2.h"

//#include <vector>
//#include <string>
//#include <iostream>

using namespace std;
using namespace m1;


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

    transPlayerX = 0;
    transPlayerY = 0;
    transPlayerZ = 0;

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
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

    InitMaze();
}

void Tema2::InitMaze() {
    pair<vector<vector<int>>, set<pair<int, int>>> tmp = GenerateMaze();
    grid = tmp.first;
    playerValidPoz = tmp.second;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++)
            cout << grid[i][j] << " ";
        cout << endl;
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
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    DrawMaze(grid);
    DrawPlayer(deltaTimeSeconds); // va trebui translatat la pozitia initiala valida in maze
    DrawEnemy(deltaTimeSeconds);
}

void Tema2::DrawMaze(vector<vector<int>> grid) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 1) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, j));
                RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.1f, 0.2f));
            }
        }
           
    }
}

void Tema2::DrawEnemy(float deltaTimeSeconds) {}

void Tema2::DrawPlayer(float deltaTimeSeconds) {
    
    //corp
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.25f, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(transPlayerX, transPlayerY, transPlayerZ));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1, 0.5f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.5f, 0.2f));

    //cap
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.9f, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(transPlayerX, transPlayerY, transPlayerZ));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0.7f, 0.7f, 0.3f));

    //picioare
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.25, 0.5f, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(transPlayerX, transPlayerY, transPlayerZ));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.75f, 0.3f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.4f ,0.8f));

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.25f, 0.5f, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(transPlayerX, transPlayerY, transPlayerZ));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.75f, 0.3f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.4f, 0.8f));

    // maini
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.3f, 1.6f, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(transPlayerX, transPlayerY, transPlayerZ));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.5, 0.2));

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.3f, 1.6f, 0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(transPlayerX, transPlayerY, transPlayerZ));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
    RenderSimpleMesh(meshes["box"], shaders["BodyShader"], modelMatrix, glm::vec3(0, 0.5, 0.2));

}



void Tema2::FrameEnd()
{
    DrawCoordinateSystem();
}


void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // TODO(student): Set material property uniforms (shininess, kd, ks, object color)
    GLint object_color_poz = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(object_color_poz, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // Add key press event
    
    // aici ar trebui pentru buton jump sau ceva
    //if (window->KeyHold(GLFW_KEY_SPACE)) {
    //    // translatare sus pe oy
    //    transPlayerY += 10 * deltaTime;

    //}
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        // translatare dreapta pe ox
        transPlayerX += 10 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        // translatare stanga pe ox
        transPlayerX -= 10 * deltaTime;

    }
    if (window->KeyHold(GLFW_KEY_UP)) {
        // translatare sus pe oz
        transPlayerZ += 10 * deltaTime;

    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        // translatare jos pe oz
        transPlayerZ -= 10 * deltaTime;
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
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
