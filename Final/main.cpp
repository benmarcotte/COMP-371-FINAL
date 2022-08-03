#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <filesystem>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderScene(Shader shader, Model obj);
void generateTerrain();
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
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

    // load models
    // -----------
    //Model ourModel("resources/objects/backpack/backpack.obj");
    //Model ourModel2("resources/objects/tree2/tree.obj");
    //Model ourModel3("resources/objects/tree3/tree4.obj");
    Model ourModel4("resources/objects/shrine/shrine.obj");


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
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        renderScene(ourShader, ourModel4);


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
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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

void renderScene(Shader shader, Model obj)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    shader.setMat4("model", model);
    obj.Draw(shader);
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
    // 2 == bridge
    // 3 == item3
    // 4 == item4
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            terrainData[i][j] = "0";
        }
    }
    

    bool hasShrine = false;
    bool hasBridge = false;
    bool hasItem3 = false;
    bool hasItem4 = false;
    int pathRand;
    int pathStart = rand() % 10;
    int pathPos = pathStart;
    terrainData[0][pathStart] = "pV";
    bool columnHasPath = true;
    srand(time(NULL));

    for (int i = 0; i < 10; i++)
    {
        if (pathPos != 0 && pathPos != 9)
        {
            pathRand = (rand() % 3) - 1;
        }
        else if (pathPos == 0)
        {
            pathRand = (rand() % 2);
        }
        else
        {
            pathRand = (rand() % 2) - 1;
        }
        for (int j = 0; j < 10; j++) 
        {
            if (columnHasPath != true)
            {
                if (pathRand + pathPos == j)
                {
                    if (pathRand == -1)
                    {
                        terrainData[i][j] = "pSW";
                        columnHasPath = true;
                        pathPos = j + pathRand;
                    }
                    else if (pathRand == 0)
                    {
                        terrainData[i][j] = "pV";
                        columnHasPath = true;
                        pathPos = j + pathRand;
                    }
                    else
                    {
                        terrainData[i][j] = "pSE";
                        columnHasPath = true;
                        pathPos = j + pathRand;
                    }
                }
            }
            
            if (terrainData[i][j].compare("0") == 0 && (hasShrine == false || hasBridge == false || hasItem3 == false || hasItem4 == false))
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
                        if (t == 2 && hasBridge == false)
                        {
                            terrainData[i][j] = "2";
                            hasBridge = true;
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
        columnHasPath = false;
    }
    for (int i = 0; i < terrainHeight; ++i)
    {
        for (int j = 0; j < terrainWidth; ++j)
        {
            std::cout << terrainData[i][j] << ' ';
        }
        std::cout << std::endl;
    }

}