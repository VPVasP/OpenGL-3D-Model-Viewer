#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <sstream>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>

#include <iostream>
#include <vector>

//#include <stb_image.h>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include <mmstream.h>
#pragma comment(lib,"winmm.lib")
//using namespace std;

float randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}

// Callbacks
void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
// ---------

// function declarations
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path, bool flipVertically);
unsigned int loadCubeMap(std::vector<std::string> faces);
// ---------------------

// settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 1024;

// time settings
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;
// --------

// Mouse position data
float previousX = 0.0f;
float previousY = 0.0f;
bool isFirstFrame = true;
// -------------------
bool roughMatteGlossyScenario = false;
float roughnessValue = 0.2f;
float matteValue = 0.5f;
float glossinessValue = 0.8f;
// handling input
//glm::vec3 displacement = glm::vec3(0.0f, 0.0f, -2.0f);
//float movementSpeed = 1.0f;
float keyTimeCounter = 0.0f;
// --------------

// Camera declaration
Camera myCamera1(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f));
// ------------------

// Lighting configurations
bool useBlinnPhong = false;
bool useSpecular2 = false;
int currentModel = 0;
int modelCount = 0;
// -----------------------

int main()
{
   
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // ------------------------------

    // glfw: window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLProjectVP", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // ---------------------

    // Cursor grabbing settings
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // ------------------------

    // glad: load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to load OpenGL function pointers!" << std::endl;
        glfwTerminate();
        return -1;
    }
    // -----------------------------------

    //Enabling-Disabling depth testing
    glEnable(GL_DEPTH_TEST);
    //--------------------------------

    // Shader loading and building
    //Shader myFirstShader("Shaders/vertexShader.vs", "Shaders/fragmentShader.fs");
    Shader lampShader("Shaders/vertexShader.vs", "Shaders/lampFragShader.fs");
    Shader skyboxShader("Shaders/skyboxShaderVertex.vs", "Shaders/skyboxShaderFragment.fs");
    Shader envMappingShader("Shaders/vertexShader.vs", "Shaders/envMappingFragment.fs");
    Shader modelShader("Shaders/vertexShader.vs", "Shaders/modelPhongFragShader.fs");

  
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

     // Vertex attribute for POSITION
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // -----------------------------

    // Vertex attribute for NORMAL
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // ---------------------------

    // Vertex attribute for TEXTURE
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // --------------------------

    // Skybox VAO and VBO
    unsigned int skyboxVBO, skyboxVAO;

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // ------------------

    // Model loading
    std::vector<std::string> modelPaths
    {
        "Models/Girl/girl.obj",
        "Models/nanosuit/nanosuit.obj",
        "Models/tower/Medieval_tower_High.obj",
        "Models/aircraft/E 45 Aircraft_obj.obj"
    };

    std::vector<glm::vec3> modelScaleFactors
    {
        glm::vec3(0.2f),
        glm::vec3(0.1f),
        glm::vec3(0.1f),
        glm::vec3(0.2f)
    };
    std::vector<float> roughnessValues
    {

        0.2f,
        0.13f,
        0.12f,
        0.11f
    };

    std::vector<float> matteValues
    {
        0.5f,
        0.9f,
        0.11f,
        0.13f
    };

    std::vector<float> glossinessValues
    {
        0.1f,
        0.9f,
        0.6f,
        1.5f
    };

    std::vector<Model> loadedModels;
    for (int i = 0; i < modelPaths.size(); i++)
    {
        modelShader.setFloat("roughness", roughnessValues[i]);
        modelShader.setFloat("matte", matteValues[i]);
        modelShader.setFloat("glossiness", glossinessValues[i]);
        Model temp(modelPaths[i]);
        loadedModels.push_back(temp);

        // Print the values for the current model
        std::cout << "Model at index " << i << ": "
            << "Roughness = " << roughnessValues[i] << ", "
            << "Matte = " << matteValues[i] << ", "
            << "Glossiness = " << glossinessValues[i] << std::endl;
        std::cout << "These are the values we set in our vectors for matteValues,glossinessValues and roughness values. " << currentModel << std::endl;
    }
    modelCount = loadedModels.size();
    // -------------

    // Texture loading
    unsigned int diffuseMap = loadTexture("Textures/diffuseMap.png", false);
    unsigned int specularMap1 = loadTexture("Textures/specularMap.png", false);
    unsigned int specularMap2 = loadTexture("Textures/specularMap2.png", false);

    // Cubemap for skybox
    std::vector<std::string> cubeFaces
    {
        "Textures/skybox_2/0_right.png",
        "Textures/skybox_2/1_left.png",
        "Textures/skybox_2/2_top.png",
        "Textures/skybox_2/3_bottom.png",
        "Textures/skybox_2/4_front.png",
        "Textures/skybox_2/5_back.png"
    };

    unsigned int cubeMapTexture = loadCubeMap(cubeFaces);
    // ------------------

    // ---------------

    // Unbinding VAO and VBO. This is not mandatory, but it will make the pipeline clearer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // ----------------------

    // Setting up surface material
    // for more materials visit: http://devernay.free.fr/cours/opengl/materials.html

    // Set up model shader uniforms
    modelShader.use();
    modelShader.setFloat("ambientCoeff", 0.05);
    modelShader.setFloat("shininess", 128 * 0.6);

    glm::vec3 lightCol = glm::vec3(1.0, 1.0, 1.0);
    modelShader.setVec3("lightColor", lightCol);
    // ----------------------------

    // -------------------
    // FPS counter variables
    int frameCount = 0;
    float fpsTimer = 0.0f;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    PlaySound(TEXT("SpaceMusic.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    // MAIN RENDERING LOOP
   // MAIN RENDERING LOOP
   // PlaySound(TEXT("megalonavia.wav"), NULL, SND_FILENAME | SND_SYNC);
    while (!glfwWindowShouldClose(window))
    {
        // DeltaTime calculation
       
        float currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        // ---------------------

        // handle user input
        processInput(window);
        // -----------------

        // RENDER STUFF

        // Reinitialize frame buffer
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // -------------------------

        // Declare transforms
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        // ------------------

        // Calculate projection matrix
        // Attributes: fov, aspect ratio, near clipping plane, far clipping plane
        projection = glm::perspective(glm::radians(myCamera1.fov), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        // ---------------------------

        // Update view matrix
        view = myCamera1.GetViewMatrix();
        // ------------------

        // Real time uniforms for blinn-phong shader
        glm::vec3 lightPos = glm::vec3(glm::sin(glfwGetTime()), glm::cos(glfwGetTime()), 2.0f);

        // Increase frame count
        frameCount++;

        fpsTimer += deltaTime;

        
        if (fpsTimer >= 1.0f)
        {
            float fps = static_cast<float>(frameCount) / fpsTimer;

           
            std::stringstream ss;
            std::string fpsString= " FPS: ";
            ss << "OpenGLProjectVP " <<fpsString<< std::fixed << std::setprecision(1) << fps;

            glfwSetWindowTitle(window, ss.str().c_str());

          
            frameCount = 0;
            fpsTimer = 0.0f;
        }

        modelShader.use();
        modelShader.setVec3("lightPosition", lightPos);
        modelShader.setVec3("viewPosition", myCamera1.position);
        modelShader.setInt("useBlinnPhong", useBlinnPhong);

        modelShader.setMat4("view", view);
        modelShader.setMat4("projection", projection);

        model = glm::translate(model, glm::vec3(0.0));
        model = glm::scale(model, modelScaleFactors[currentModel]);
        model = glm::rotate(model, (float)glfwGetTime() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        modelShader.setMat4("model", model);

        loadedModels[currentModel].Draw(modelShader);

        // Render the lamp
        lampShader.use();

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lampShader.setMat4("model", model);
        lampShader.setMat4("view", view);
        lampShader.setMat4("projection", projection);

        lampShader.setVec3("lightColor", lightCol);

        // Normally we should bind the appropriate VAO and/or textures
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // ---------------

        // Skybox rendering routine ** OPTIMAL **
        glDepthFunc(GL_LEQUAL); // Skybox pixels pass the depth test because their depth value (1)
                                // is "lower than or equal" to the depth value of the "empty" areas
                                // of the depth buffer (where no geometry has been rendered)

        skyboxShader.use();

        skyboxShader.setInt("skybox", 0);
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skyboxShader.setMat4("projection", projection);

        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS); // Reset to default depth testing strategy

        float fps = static_cast<float>(frameCount) / fpsTimer;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));
        ImGui::SetNextWindowSize(ImVec2(200.0f,50.0f));
        ImGui::Begin("FPS COUNTER");

        ImGui::Text("FPS: %.1f", fps);

        ImGui::End();
        ImGui::SetNextWindowPos(ImVec2(10.0f,700.0f));
        ImGui::SetNextWindowSize(ImVec2(800.0f,180.0f));
        ImGui::Begin("Model Values");

     
        for (int i = 0; i < loadedModels.size(); i++)
        {
            ImGui::Text("Model at index %d: Roughness = %.2f, Matte = %.2f, Glossiness = %.2f",
                i, roughnessValues[i], matteValues[i], glossinessValues[i]);
            ImGui::Text("These are the values we set in our vectors for matteValues, glossinessValues, and roughness values.");
            ImGui::Separator();
        }
       
        

   
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      

       
        glfwSwapBuffers(window);
        glfwPollEvents();
       
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

//Handle keyboard input events
void processInput(GLFWwindow* window)
{


    if (keyTimeCounter + deltaTime <= FLT_MAX)
        keyTimeCounter += deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        myCamera1.HandleKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        myCamera1.HandleKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        myCamera1.HandleKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        myCamera1.HandleKeyboard(LEFT, deltaTime);
    }

    // Move up or down
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        myCamera1.HandleKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        myCamera1.HandleKeyboard(DOWN, deltaTime);
    }

    // Toggle Blinn-Phong
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        if (keyTimeCounter >= 1.0f)
        {
            useBlinnPhong = !useBlinnPhong;
            std::cout << "Using BlinnPhong " << currentModel << std::endl;
            keyTimeCounter = 0.0f;
        }
    }

    // Toggle model
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {


        if (keyTimeCounter >= 1.0f)
        {
            currentModel++;
            if (currentModel == modelCount)
                currentModel = 0;
            keyTimeCounter = 0.0f;
            std::cout << "Added a new model " << currentModel << std::endl;





            //std::cout << "Roughness: " << roughness << ", Glossiness: " << glossiness << ", Matte: " << matte << "Shineness: " << shineness << std::endl;
        }

    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        Shader modelShader("Shaders/vertexShader.vs", "Shaders/modelPhongFragShader.fs");
        if (keyTimeCounter >= 1.0f)
        {
            roughMatteGlossyScenario = !roughMatteGlossyScenario;
            keyTimeCounter = 0.0f;

            // Update shader uniform based on the scenario


        }

        // ------------------
    }
}

// frame buffer resizing callback
void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

// Mouse position callback
void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (isFirstFrame)
    {
        previousX = xPos;
        previousY = yPos;
        isFirstFrame = false;
    }

    float xOffset = xPos - previousX;
    float yOffset = yPos - previousY;

    previousX = xPos;
    previousY = yPos;

    myCamera1.HandleMouseMovement(xOffset, yOffset, deltaTime);
}

// Scrolling callback
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    myCamera1.HandleMouseScroll((float)yOffset);
}

unsigned int loadTexture(const char* path, bool flipVertically)
{
    unsigned int textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture wrapping properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // ---------------------------

    // Texture filtering properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // ----------------------------

    stbi_set_flip_vertically_on_load(flipVertically);

    int width, height, numOfChannels;
    unsigned char* data = stbi_load(path, &width, &height, &numOfChannels, 0);

    if (data)
    {
        GLenum format;

        if (numOfChannels == 1)
        {
            format = GL_RED;
        }
        else if (numOfChannels == 3)
        {
            format = GL_RGB;
        }
        else if (numOfChannels == 4)
        {
            format = GL_RGBA;
        }
        else
        {
            std::cout << "TEXTURE FILE FAILED TO LOAD: INCOMPATIBLE NUMBER OF CHANNELS!!" << std::endl;
            stbi_image_free(data);
            return textureID;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "TEXTURE FILE FAILED TO LOAD FROM PATH " << path << "!!" << std::endl;
    }

    stbi_image_free(data);

    return textureID;
}

// Face order should follow the rule:
// 0. Right face (+X)
// 1. Left face (-X)
// 2. Top face (+Y)
// 3. Bottom face (-Y)
// 4. Front face (+Z)
// 5. Back face (-Z)
unsigned int loadCubeMap(std::vector<std::string> faces)
{
    unsigned int textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Texture wrapping properties
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // ---------------------------

    // Texture filtering properties
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ----------------------------

    int width, height, numOfChannels;

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &numOfChannels, 0);

        if (data)
        {
            GLenum format;

            if (numOfChannels == 1)
            {
                format = GL_RED;
            }
            else if (numOfChannels == 3)
            {
                format = GL_RGB;
            }
            else if (numOfChannels == 4)
            {
                format = GL_RGBA;
            }
            else
            {
                std::cout << "TEXTURE FILE " << faces[i] << " FAILED TO LOAD: INCOMPATIBLE NUMBER OF CHANNELS!!" << std::endl;
                stbi_image_free(data);
                continue;
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "TEXTURE FILE FAILED TO LOAD FROM PATH " << faces[i] << "!!" << std::endl;
        }

        stbi_image_free(data);
    }

    return textureID;
}
