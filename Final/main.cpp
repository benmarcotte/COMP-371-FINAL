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
#include "shrine_renderable.h"
#include "ruins_renderable.h"
#include "well_renderable.h"
#include "skybox_renderable.h"
#include "large_rock_1_renderable.h"
#include "large_rock_2_renderable.h"
#include "large_rock_3_renderable.h"
#include "medium_rock_1_renderable.h"
#include "medium_rock_2_renderable.h"
#include "small_rock_1_renderable.h"
#include "small_rock_2_renderable.h"
#include "tree1_renderable.h"
#include "tree2_renderable.h"
#include "tree3_renderable.h"
#include "tree4_renderable.h"

#include <iostream>
#include <filesystem>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void renderScene(Shader shader, vector<Renderable*> models);
void generateTerrain();
void generateRenderArray(vector<Renderable*> models);
void generateLights(Shader shader, vector<glm::vec3> pointLightPositions);
vector<glm::vec3> pointLightPositions;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

vector<glm::mat4> positions;
vector<glm::vec3> scalings;
vector<float> rotations;
vector<int> modelsToRender;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Camera cabin(glm::vec3(0.0f, 0.0f, 3.0f));
Camera well(glm::vec3(0.0f, 0.0f, 3.0f));
Camera ruins(glm::vec3(0.0f, 0.0f, 3.0f));
Camera shrine(glm::vec3(0.0f, 0.0f, 3.0f));
Camera* currentCam = &camera;
glm::vec3 lightPos;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
int activeCam = 0;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// terrain data
const int terrainHeight = 10;
const int terrainWidth = 10;
std::string terrainData[terrainHeight][terrainWidth];



int main()
{
    srand(time(NULL));

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
    glfwSetKeyCallback(window, key_callback);

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
    Shader shadowShader("shadowVertexShader.glsl", "shadowFragmentShader.glsl");
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
    models.push_back(new Shrine);
    models.push_back(new Ruins);
    models.push_back(new Well);
    models.push_back(new LargeRock1);
    models.push_back(new LargeRock2);
    models.push_back(new LargeRock3);
    models.push_back(new MediumRock1);
    models.push_back(new MediumRock2);
    models.push_back(new SmallRock1);
    models.push_back(new SmallRock2);
    models.push_back(new Tree1);
    models.push_back(new Tree2);
    models.push_back(new Tree3);
    models.push_back(new Tree4);
    models.push_back(new Tree1);

    generateRenderArray(models);




    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024*4, SHADOW_HEIGHT = 1024*4;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    defaultShader.use();
    defaultShader.setInt("shadowMap", 2);
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
        lightPos = glm::vec3(-30.0f, 60.0f, 50.0f);
        //glm::mat4 projection = glm::ortho(-60.0f,60.0f, -30.0f, 30.0f, 1.0f, 150.0f);
        //glm::mat4 view = glm::lookAt(lightPos, glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        defaultShader.setMat4("projection", projection);

        if (activeCam == 0)
        {
            glm::mat4 view = camera.GetViewMatrix();
            defaultShader.setMat4("view", view);
        }
        else if (activeCam == 1)
        {
            glm::mat4 view = cabin.GetViewMatrix();
            defaultShader.setMat4("view", view);
        }
        else if (activeCam == 2)
        {
            glm::mat4 view = shrine.GetViewMatrix();
            defaultShader.setMat4("view", view);
        }
        else if (activeCam == 3)
        {
            glm::mat4 view = ruins.GetViewMatrix();
            defaultShader.setMat4("view", view);
        }
        else if (activeCam == 4)
        {
            glm::mat4 view = well.GetViewMatrix();
            defaultShader.setMat4("view", view);
        }

        generateLights(defaultShader, pointLightPositions);

        //shader.setVec3("dirLight.direction", 0.2f, -1.0f, 0.3f);

        //set lights
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 1000.0f;
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::ortho(-60.0f, 60.0f, -35.0f, 35.0f, 1.0f, 150.0f);
        lightView = glm::lookAt(lightPos, glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        shadowShader.use();
        shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderScene(shadowShader, models);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the loaded model
        defaultShader.use();
        defaultShader.setVec3("lightPos", lightPos);
        defaultShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);

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


    if (true)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            currentCam->ProcessKeyboard(FORWARD, deltaTime*6.0f);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            currentCam->ProcessKeyboard(BACKWARD, deltaTime*6.0f);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            currentCam->ProcessKeyboard(LEFT, deltaTime*6.0f);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            currentCam->ProcessKeyboard(RIGHT, deltaTime*6.0f);
    }
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

    currentCam->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    currentCam->ProcessMouseScroll(static_cast<float>(yoffset));
}

void renderScene(Shader shader, vector<Renderable*> models)
{
    for (int i = 0; i < modelsToRender.size(); i++)
    {
        models[modelsToRender[i]]->Draw(positions[i], shader, scalings[i], rotations[i]);
    }
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
            camera.Front = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
        }
        else if (sideStart == 1)
        {
            pathStart = ((rand() % 10) * 10) + 9;
            pathPos = pathStart - 1;
            terrainData[pathStart / 10][pathStart % 10] = "pH";
            pathDir = 4;
            nextDir = 0;
            camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));

        }
        else if (sideStart == 2)
        {
            pathStart = (rand() % 10) + 90;
            pathPos = pathStart - 10;
            terrainData[pathStart / 10][pathStart % 10] = "pV";
            pathDir = 1;
            nextDir = 0;
            camera.Front = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else
        {
            pathStart = (rand() % 10) * 10;
            pathPos = pathStart + 1;
            terrainData[pathStart / 10][pathStart % 10] = "pH";
            pathDir = 2;
            nextDir = 0;
            camera.Front = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
        }
        int pathTry;
        bool columnHasPath = true;
        bool pathDone = false;
        bool pathAdded = false;
        camera.Position = glm::vec3(2.5f + (float)(pathStart % 10) * 10.0f, 2.0f, 2.5f + (float)(pathStart / 10) * 10.0f);
        
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
        std::cout << "generated path: " << endl;
        for (int i = 0; i < terrainHeight; ++i)
        {
            for (int j = 0; j < terrainWidth; ++j)
            {
                std::cout << terrainData[i][j] << ' ';
            }
            std::cout << std::endl;
        }
        std::cout << "press n to regenerate path" << endl;
        std::cin >> temp;
        if (temp == 'n')
            continue;
        else
            pathAccepted = true;
    }
}

void generateRenderArray(vector<Renderable*> models)
{
    glm::mat4 model = glm::mat4(1.0f);
    for (int i = 0; i < terrainHeight; i++)
    {
        for (int j = 0; j < terrainWidth; j++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3((float)j * 10.0f, 0.0f, (float)i * 10.0f));
            if (terrainData[i][j].compare("pV") == 0)
            {
                positions.push_back(model);
                modelsToRender.push_back(1);
                scalings.push_back(models[1]->generateScalings());
                rotations.push_back(models[1]->generateRotations());
            }
            else if (terrainData[i][j].compare("pH") == 0)
            {
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                positions.push_back(model);
                modelsToRender.push_back(1);
                scalings.push_back(models[1]->generateScalings());
                rotations.push_back(models[1]->generateRotations());
            }
            else if (terrainData[i][j].compare("pNW") == 0)
            {
                model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                positions.push_back(model);
                modelsToRender.push_back(2);
                scalings.push_back(models[2]->generateScalings());
                rotations.push_back(models[2]->generateRotations());
            }
            else if (terrainData[i][j].compare("pSW") == 0)
            {
                model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                positions.push_back(model);
                modelsToRender.push_back(2);
                scalings.push_back(models[2]->generateScalings());
                rotations.push_back(models[2]->generateRotations());
            }
            else if (terrainData[i][j].compare("pNE") == 0)
            {
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                positions.push_back(model);
                modelsToRender.push_back(2);
                scalings.push_back(models[2]->generateScalings());
                rotations.push_back(models[2]->generateRotations());
            }
            else if (terrainData[i][j].compare("pSE") == 0)
            {
                model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                positions.push_back(model);
                modelsToRender.push_back(2);
                scalings.push_back(models[2]->generateScalings());
                rotations.push_back(models[2]->generateRotations());
            }
            else if (terrainData[i][j].compare("1") == 0)
            {
                positions.push_back(model);
                modelsToRender.push_back(0);
                scalings.push_back(models[0]->generateScalings());
                rotations.push_back(models[0]->generateRotations());
                positions.push_back(model);
                modelsToRender.push_back(3);
                scalings.push_back(models[3]->generateScalings());
                rotations.push_back(models[3]->generateRotations());
                pointLightPositions.push_back(glm::vec3((float)j * 10.0f-5.0f, 10.0f, (float)i * 10.0f));
                cabin.Position = glm::vec3((float)j * 10.0f - 3.0f, 10.0f, (float)i * 10.0f - 3.0f);
            }
            else if (terrainData[i][j].compare("2") == 0)
            {
                positions.push_back(model);
                modelsToRender.push_back(0);
                scalings.push_back(models[0]->generateScalings());
                rotations.push_back(models[0]->generateRotations());
                positions.push_back(model);
                modelsToRender.push_back(4);
                scalings.push_back(models[4]->generateScalings());
                rotations.push_back(models[4]->generateRotations());
                pointLightPositions.push_back(glm::vec3((float)j * 10.0f - 5.0f, 10.0f, (float)i * 10.0f));
                shrine.Position = glm::vec3((float)j * 10.0f - 3.0f, 10.0f, (float)i * 10.0f - 3.0f);
            }
            else if (terrainData[i][j].compare("3") == 0)
            {
                positions.push_back(model);
                modelsToRender.push_back(0);
                scalings.push_back(models[0]->generateScalings());
                rotations.push_back(models[0]->generateRotations());
                positions.push_back(model);
                modelsToRender.push_back(5);
                scalings.push_back(models[5]->generateScalings());
                rotations.push_back(models[5]->generateRotations());
                pointLightPositions.push_back(glm::vec3((float)j * 10.0f-5.0f, 10.0f, (float)i * 10.0f));
                ruins.Position = glm::vec3((float)j * 10.0f - 3.0f, 10.0f, (float)i * 10.0f - 3.0f);
            }
            else if (terrainData[i][j].compare("4") == 0)
            {
                positions.push_back(model);
                modelsToRender.push_back(0);
                scalings.push_back(models[0]->generateScalings());
                rotations.push_back(models[0]->generateRotations());
                positions.push_back(model);
                modelsToRender.push_back(6);
                scalings.push_back(models[6]->generateScalings());
                rotations.push_back(models[6]->generateRotations());
                pointLightPositions.push_back(glm::vec3((float)j * 10.0f-5.0f, 10.0f, (float)i * 10.0f));
                well.Position = glm::vec3((float)j * 10.0f - 3.0f, 10.0f, (float)i * 10.0f - 3.0f);
            }
            else
            {
                int r;
                positions.push_back(model);
                modelsToRender.push_back(0);
                scalings.push_back(models[0]->generateScalings());
                rotations.push_back(models[0]->generateRotations());
                model = glm::translate(model, glm::vec3((float)(rand() % 80 - 40) * 0.1f, 0.0f, (float)(rand() % 80 - 40) * 0.1f));
                positions.push_back(model);
                r = (rand() % 12) + 7;
                modelsToRender.push_back(r);
                scalings.push_back(models[r]->generateScalings());
                rotations.push_back(models[r]->generateRotations());
                
            }
        }
    }
    while (pointLightPositions.size() < 4)
        pointLightPositions.push_back(glm::vec3((float)(rand() % 100), 10.0f, (float)(rand() % 100)));
}

void generateLights(Shader shader, vector<glm::vec3> pointLightPositions)
{
        shader.setFloat("material.shininess", 16.0f);
        // directional light
        shader.setVec3("dirLight.direction", 0.2f, -1.0f, 0.3f);
        shader.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.02f);
        shader.setVec3("dirLight.diffuse", 0.1f, 0.1f, 0.2f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.8f);
        //light 1
        shader.setVec3("pointLights[0].position", pointLightPositions[0]);
        shader.setVec3("pointLights[0].ambient", 0.001f, 0.001f, 0.001f);
        shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[0].constant", 1.0f);
        shader.setFloat("pointLights[0].linear", 0.09f);
        shader.setFloat("pointLights[0].quadratic", 0.032f);
        //slight 2
        shader.setVec3("pointLights[1].position", pointLightPositions[1]);
        shader.setVec3("pointLights[1].ambient", 0.001f, 0.001f, 0.001f);
        shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[1].constant", 1.0f);
        shader.setFloat("pointLights[1].linear", 0.09f);
        shader.setFloat("pointLights[1].quadratic", 0.032f);
        //slight 3
        shader.setVec3("pointLights[2].position", pointLightPositions[2]);
        shader.setVec3("pointLights[2].ambient", 0.001f, 0.001f, 0.001f);
        shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[2].constant", 1.0f);
        shader.setFloat("pointLights[2].linear", 0.09f);
        shader.setFloat("pointLights[2].quadratic", 0.032f);
        //slight 4
        shader.setVec3("pointLights[3].position", pointLightPositions[3]);
        shader.setVec3("pointLights[3].ambient", 0.001f, 0.001f, 0.001f);
        shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[3].constant", 1.0f);
        shader.setFloat("pointLights[3].linear", 0.09f);
        shader.setFloat("pointLights[3].quadratic", 0.032f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        activeCam = 1;
        currentCam = &cabin;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        activeCam = 2;
        currentCam = &shrine;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        activeCam = 3;
        currentCam = &ruins;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        activeCam = 4;
        currentCam = &well;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        activeCam = 0;
        currentCam = &camera;
    }
}