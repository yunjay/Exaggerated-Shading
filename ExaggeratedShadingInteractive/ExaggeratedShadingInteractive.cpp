#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "model.h"
#include "loadShader.h"

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLuint loadShader(const GLchar* vertexPath, const GLchar* fragmentPath);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// UI
float xDegrees = 0.0f;
float yDegrees = 0.0f;
bool xOn=true;
float modelSize=10.0f;

int scales=10; //b, num of scales
float contributionScale=-0.5
float contribution[20]={0};//init to zeroe
float sigma[20];
int main()
{
    xOn = false;
    //TODO : sigma values. from featureSize and multiply by sqrt2 every step.

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Exaggerated Shading Interactive", NULL, NULL);
    if (window == NULL)
    {std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;}
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers]
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {    std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;}

    // configure global opengl state
    //z buffer
    glEnable(GL_DEPTH_TEST);

    //IMGui init    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glClearColor(0.15, 0.14, 0.13, 0.0); //background
    
    //Load Model
    Model skull("../skull/12140_Skull_v3_L2.obj");

    //Load Shader
    GLuint cosine = loadShader("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/cosine.vs","C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/cosine.fs");
    GLuint xShade = loadShader("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/xShade.vs","C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/xShade.fs");
    GLuint* currentShader=&xShade;

    //view
    glm::vec3 cameraPos = glm::vec3(0, 0, 1);
    //light settings
    glm::vec3 lightPos = glm::vec3(-1, 1, 1);
    glm::vec3 lightDiffuse = glm::vec3(1, 1, 1);

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //IMGui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //IMGui window
        ImGui::Begin("XShade Interactive");

        ImGui::Checkbox("Exaggerated Shading", &xOn);
        ImGui::SliderFloat("Rotate X", &xDegrees, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotate Y", &yDegrees, 0.0f, 360.0f);
        ImGui::SliderFloat("Model Size", &modelSize, 0.005f, 0.1f);
        ImGui::SliderInt("Number of Smoothing Scales", &scales, 1, 20);
        ImGui::SliderFloat("Contribution factor of ki", &contributionScale, -5.0f, 5.0f);
        ImGui::End();

        //contribution factor
        int sum=0;
        float contributionBeforeNorm[20]={0};
        for(int i=0;i<20;i++){
            contributionBeforeNorm[i]=glm::pow(sigma[i],contributionScale);
        }
        contribution[i]=;

        if (!xOn)currentShader = &cosine;
        else currentShader = &xShade;

        glUseProgram(*currentShader);
        glUniform3f(glGetUniformLocation(*currentShader, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(*currentShader, "light.diffuse"), lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);

        glm::mat4 model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0,-0.4, -1.0f));
        model = glm::scale(model, glm::vec3(modelSize, modelSize, modelSize));
        model = glm::rotate(model, glm::radians(yDegrees), glm::vec3(0,1,0));
        model = glm::rotate(model, glm::radians(xDegrees), glm::vec3(1,0,0));
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0,0,-1), glm::vec3(0,1,0));
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        glUniformMatrix4fv(glGetUniformLocation(*currentShader, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(*currentShader, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(*currentShader, "projection"), 1, GL_FALSE, &projection[0][0]);


        skull.render(*currentShader);




        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();

    }




    // Delete ImGUI instances
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.=
    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}