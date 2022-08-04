#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include "renderable.h"
#include "grass_renderable.h"
#include "elbow_renderable.h"
#include "straight_renderable.h"
#include "cabin_renderable.h"

#include <iostream>
#include <filesystem>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderScene(Shader shader, vector<Renderable*> models);
void generateTerrain();
void generateLights(Shader shader, glm::vec3* pointLightPositions);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// terrain data
const int terrainHeight = 10;
const int terrainWidth = 10;
std::string terrainData[terrainHeight][terrainWidth];



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // generate terrain
    generateTerrain();
    
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader defaultShader("vertexShader.glsl", "fragmentShader.glsl");

    // load models
    // -----------
    //Model ourModel("resources/objects/backpack/backpack.obj");
    //Model ourModel2("resources/objects/tree2/tree.obj");
    //Model ourModel3("resources/objects/tree3/tree4.obj");
    //Model straight("resources/objects/straight_path/straight_path.obj");
    //Model elbow("resources/objects/elbow_path/elbow_path.obj");
    //Model grass("resources/objects/grass/grass.obj");
    //Model shrine("resources/objects/shrine/shrine.obj");
    //Model cabin("resources/objects/cabin/cabin.obj");


    vector<Renderable*> models;
    
    models.push_back(new Grass);
    models.push_back(new StraightPath);
    models.push_back(new ElbowPath);
    models.push_back(new Cabin);

    srand(time(NULL));

    glm::vec3 pointLightPositions[] = {
        glm::vec3((float)(rand() % 100),  10.0f,  (float)(rand() % 100)),
        glm::vec3((float)(rand() % 100), 10.0f, (float)(rand() % 100)),
        glm::vec3((float)(rand() % 100),  10.0f, (float)(rand() % 100)),
        glm::vec3((float)(rand() % 100),  10.0f,(float)(rand() % 100))
    };

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        defaultShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        defaultShader.setMat4("projection", projection);
        defaultShader.setMat4("view", view);
        generateLights(defaultShader, pointLightPositions);

        // render the loaded model
        renderScene(defaultShader, models);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime*3.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime*3.0f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime*3.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime*3.0f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void renderScene(Shader shader, vector<Renderable*> models)
{
    // [0] = grass
    // [1] = straightpath
    // [2] = elbowpath
    // [3] = cabin
    //
    
    glm::mat4 model = glm::mat4(1.0f);

    for (int i = 0; i < terrainHeight; i++)
    {
        for (int j = 0; j < terrainWidth; j++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3((float)j * 10.0f, 0.0f, (float)i * 10.0f));
            if (terrainData[i][j].compare("pV") == 0)
            {
                shader.setMat4("model", model);
                models[1]->Draw(model, shader);
            }
            else if (terrainData[i][j].compare("pH") == 0)
            {
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                shader.setMat4("model", model);
                models[1]->Draw(model, shader);
            }
            else if (terrainData[i][j].compare("pNW") == 0)
            {
                model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                shader.setMat4("model", model);
                models[2]->Draw(model, shader);
            }
            else if (terrainData[i][j].compare("pSW") == 0)
            {
                model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                shader.setMat4("model", model);
                models[2]->Draw(model, shader);
            }
            else if (terrainData[i][j].compare("pNE") == 0)
            {
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                shader.setMat4("model", model);
                models[2]->Draw(model, shader);
            }
            else if (terrainData[i][j].compare("pSE") == 0)
            {
                model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                shader.setMat4("model", model);
                models[2]->Draw(model, shader);
            }
            else if (terrainData[i][j].compare("1") == 0)
            {
                shader.setMat4("model", model);
                models[0]->Draw(model, shader);
            }
            else if (terrainData[i][j].compare("2") == 0)
            {
                shader.setMat4("model", model);
                models[3]->Draw(model, shader);
            }
            else
            {
                shader.setMat4("model", model);
                models[0]->Draw(model, shader);
            }
        }
    }


    //shader.setMat4("model", model);
    //elbow.Draw(shader);

    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    ////model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 10.0f)); // translate it down so it's at the center of the scene
    ////model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    //shader.setMat4("model", model);
    //obj.Draw(shader);

}

void generateTerrain()
{
    // pV == vertical path
    // pH == horizontal path
    // pNE == north-east path
    // pSE == south-east path
    // pSW == south-west path
    // pNW == north-west path
    // 1 == shrine
    // 2 == cabin
    // 3 == item3
    // 4 == item4
    bool pathAccepted = false;

    while (!pathAccepted)
    {

        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                terrainData[i][j] = "0";
            }
        }
    

        bool hasShrine = false;
        bool hasCabin = false;
        bool hasItem3 = false;
        bool hasItem4 = false;
        int pathRand;
        srand(time(NULL));
        int sideStart = rand() % 4;
        int pathStart;
        int pathPos;
        int pathDir; // 1 = north, 2 = east, etc
        int nextDir; // -1 = left, 0 = str, 1 = right
        if (sideStart == 0)
        {
            pathStart = rand() % 10;
            pathPos = pathStart + 10;
            terrainData[pathStart / 10][pathStart % 10] = "pV";
            pathDir = 3;
            nextDir = 0;
        }
        else if (sideStart == 1)
        {
            pathStart = ((rand() % 10) * 10) + 9;
            pathPos = pathStart - 1;
            terrainData[pathStart / 10][pathStart % 10] = "pH";
            pathDir = 4;
            nextDir = 0;
        }
        else if (sideStart == 2)
        {
            pathStart = (rand() % 10) + 90;
            pathPos = pathStart - 10;
            terrainData[pathStart / 10][pathStart % 10] = "pV";
            pathDir = 1;
            nextDir = 0;
        }
        else
        {
            pathStart = (rand() % 10) * 10;
            pathPos = pathStart + 1;
            terrainData[pathStart / 10][pathStart % 10] = "pH";
            pathDir = 2;
            nextDir = 0;
        }
        int pathTry;
    
        bool columnHasPath = true;
        bool pathDone = false;
        bool pathAdded = false;


        while (!pathDone)
        {
            pathAdded = false;
            while (!pathAdded)
            {
                if (nextDir <= 2)
                {
                    if (pathDir == 1)
                    {
                        pathTry = pathPos - 1;
                        if (pathTry % 10 == 9) {
                            terrainData[pathPos / 10][pathPos % 10] = "pSW";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pSW";
                            pathPos = pathTry;
                            pathDir = 4;
                            pathAdded = true;
                        }
                    }
                    else if (pathDir == 2)
                    {
                        pathTry = pathPos - 10;
                        if (pathTry < 0) {
                            terrainData[pathPos / 10][pathPos % 10] = "pNW";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pNW";
                            pathPos = pathTry;
                            pathDir = 1;
                            pathAdded = true;
                        }
                    }
                    else if (pathDir == 3)
                    {
                        pathTry = pathPos + 1;
                        if (pathTry % 10 == 0) {
                            terrainData[pathPos / 10][pathPos % 10] = "pNE";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pNE";
                            pathPos = pathTry;
                            pathDir = 2;
                            pathAdded = true;
                        }
                    }
                    else if (pathDir == 4)
                    {
                        pathTry = pathPos + 10;
                        if (pathTry > 99) {
                            terrainData[pathPos / 10][pathPos % 10] = "pSE";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pSE";
                            pathPos = pathTry;
                            pathDir = 3;
                            pathAdded = true;
                        }
                    }
                }
                else if (nextDir >= 3 && nextDir <= 6)
                {
                    if (pathDir == 1)
                    {
                        pathTry = pathPos - 10;
                        if (pathTry < 0) {
                            terrainData[pathPos / 10][pathPos % 10] = "pV";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pV";
                            pathPos = pathTry;
                            pathDir = 1;
                            pathAdded = true;
                        }
                    }
                    else if (pathDir == 2)
                    {
                        pathTry = pathPos + 1;
                        if (pathTry % 10 == 0) {
                            terrainData[pathPos / 10][pathPos % 10] = "pH";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pH";
                            pathPos = pathTry;
                            pathDir = 2;
                            pathAdded = true;
                        }
                    }
                    else if (pathDir == 3)
                    {
                        pathTry = pathPos + 10;
                        if (pathTry > 99) {
                            terrainData[pathPos / 10][pathPos % 10] = "pV";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pV";
                            pathPos = pathTry;
                            pathDir = 3;
                            pathAdded = true;
                        }
                    }
                    else if (pathDir == 4)
                    {
                        pathTry = pathPos - 1;
                        if (pathTry % 10 == 9) {
                            terrainData[pathPos / 10][pathPos % 10] = "pH";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pH";
                            pathPos = pathTry;
                            pathDir = 4;
                            pathAdded = true;
                        }
                    }
                }
                else if (nextDir >= 7)
                {
                    if (pathDir == 1)
                    {
                        pathTry = pathPos + 1;
                        if (pathTry % 10 == 0) {
                            terrainData[pathPos / 10][pathPos % 10] = "pSE";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pSE";
                            pathPos = pathTry;
                            pathDir = 2;
                            pathAdded = true;
                        }
                    }
                    else if (pathDir == 2)
                    {
                        pathTry = pathPos + 10;
                        if (pathTry > 99) {
                            terrainData[pathPos / 10][pathPos % 10] = "pSW";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pSW";
                            pathPos = pathTry;
                            pathDir = 3;
                            pathAdded = true;
                        }
                    }
                    else if (pathDir == 3)
                    {
                        pathTry = pathPos - 1;
                        if (pathTry % 10 == 9) {
                            terrainData[pathPos / 10][pathPos % 10] = "pNW";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pNW";
                            pathPos = pathTry;
                            pathDir = 4;
                            pathAdded = true;
                        }
                    }
                    else if (pathDir == 4)
                    {
                        pathTry = pathPos - 10;
                        if (pathTry < 0) {
                            terrainData[pathPos / 10][pathPos % 10] = "pNE";
                            pathDone = true;
                            break;
                        }
                        if (terrainData[pathTry / 10][pathTry % 10].compare("0") == 0)
                        {
                            terrainData[pathPos / 10][pathPos % 10] = "pNE";
                            pathPos = pathTry;
                            pathDir = 1;
                            pathAdded = true;
                        }
                    }
                }
                nextDir = (rand() % 10);
            }
        }



        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++) 
            {
                if (terrainData[i][j].compare("0") == 0 && (hasShrine == false || hasCabin == false || hasItem3 == false || hasItem4 == false))
                {
                    //int g = rand() % 25;
                    //cout << g << endl;
                    if (rand() % 25 == 0)
                    {
                        bool itemTaken = false;
                        while (!itemTaken)
                        {
                            int t = (rand() % 4) + 1;
                            if (t == 1 && hasShrine == false)
                            {
                                terrainData[i][j] = "1";
                                hasShrine = true;
                                itemTaken = true;
                            }
                            if (t == 2 && hasCabin == false)
                            {
                                terrainData[i][j] = "2";
                                hasCabin = true;
                                itemTaken = true;
                            }
                            if (t == 3 && hasItem3 == false)
                            {
                                terrainData[i][j] = "3";
                                hasItem3 = true;
                                itemTaken = true;
                            }
                            if (t == 4 && hasItem4 == false)
                            {
                                terrainData[i][j] = "4";
                                hasItem4 = true;
                                itemTaken = true;
                            }
                        }
                    }
                }
            }
        }
        char temp;
        cout << "generated path: " << endl;
        for (int i = 0; i < terrainHeight; ++i)
        {
            for (int j = 0; j < terrainWidth; ++j)
            {
                std::cout << terrainData[i][j] << ' ';
            }
            std::cout << std::endl;
        }
        cout << "press n to regenerate path" << endl;
        cin >> temp;
        if (temp == 'n')
            continue;
        else
            pathAccepted = true;
    }
}

void generateLights(Shader shader, glm::vec3* pointLightPositions)
{
    

    shader.setFloat("material.shininess", 16.0f);
  // directional light
        shader.setVec3("dirLight.direction", 0.2f, -1.0f, 0.3f);
        shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        //light 1
        shader.setVec3("pointLights[0].position", pointLightPositions[0]);
        shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[0].constant", 1.0f);
        shader.setFloat("pointLights[0].linear", 0.09f);
        shader.setFloat("pointLights[0].quadratic", 0.032f);
        //slight 2
        shader.setVec3("pointLights[1].position", pointLightPositions[1]);
        shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[1].constant", 1.0f);
        shader.setFloat("pointLights[1].linear", 0.09f);
        shader.setFloat("pointLights[1].quadratic", 0.032f);
        //slight 3
        shader.setVec3("pointLights[2].position", pointLightPositions[2]);
        shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[2].constant", 1.0f);
        shader.setFloat("pointLights[2].linear", 0.09f);
        shader.setFloat("pointLights[2].quadratic", 0.032f);
        //slight 4
        shader.setVec3("pointLights[3].position", pointLightPositions[3]);
        shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[3].constant", 1.0f);
        shader.setFloat("pointLights[3].linear", 0.09f);
        shader.setFloat("pointLights[3].quadratic", 0.032f);
        //sght
        shader.setVec3("spotLight.position", camera.Position);
        shader.setVec3("spotLight.direction", camera.Front);
        shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
        shader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        shader.setFloat("spotLight.constant", 1.0f);
        shader.setFloat("spotLight.linear", 0.09f);
        shader.setFloat("spotLight.quadratic", 0.032f);
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));     
}