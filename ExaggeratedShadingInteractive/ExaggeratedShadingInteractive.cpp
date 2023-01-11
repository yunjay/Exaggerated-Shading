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

#include "loadShader.h"
#include "yjReader.h"
#include "smoothing.h"

// settings
const unsigned int SCR_WIDTH = 2400;
const unsigned int SCR_HEIGHT = 1350;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// UI
float xDegrees = 0.0f;
float yDegrees = 0.0f;
bool PDon = false;
float modelSize=1.0f;
float diffuse = 1.0f;
float lightDegrees = 0.0f;
glm::vec3 background(30.0 / 255, 30.0 / 255, 30.0 / 255);
glm::vec3 textureHigh(1.0f);
glm::vec3 textureLow(0.0f);

// Shading Variables
int scales=10; //b, num of scales
float contributionScale = -0.5f;
GLfloat contribution[20]={0};//init to zeros
GLfloat sigma[20];
float ambient = 0.5f;
float clampCoef = 20.0f;
float magScale = 1.0f;

int main()
{
    //xOn = false;
    enum shadingType { ExaggeratedShading, ExaggeratedShadingPD, Diffuse };
    shadingType shadingTypeArr[] = { ExaggeratedShading,ExaggeratedShadingPD, Diffuse };
    shadingType currentShading = ExaggeratedShading;

    float pdScaler = 0.1;

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
    glLineWidth(3);

    //IMGui init    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    //Load Model
    YJ bunny(".\\models\\golfball\\GolfBallOBJ.yj");
    //YJ bunny(".\\models\\bunny\\stanford-bunny.yj");
    //YJ bunny(".\\models\\lucy\\lucy.yj");
    //YJ bunny(".\\models\\david\\rapid.yj");

    
    bunny.pdPath = bunny.path;
    bunny.pdPath.replace(bunny.pdPath.end()-2, bunny.pdPath.end(),"pd");
    bunny.loadPD(); bunny.setup();
    cout << "First Max PD : " << bunny.maxPDs[0].x<<", "<<bunny.maxPDs[0].y << ", " << bunny.maxPDs[0].z << ", First Max Curvature : " << bunny.maxCurvs[0] << ".\n";
    cout << "First Min PD : " << bunny.minPDs[0].x << ", " << bunny.minPDs[0].y << ", " << bunny.minPDs[0].z << ", First Min Curvature : " << bunny.minCurvs[0] << ".\n";

    //Sigma values. from featureSize and multiplied by sqrt2 every step.
    float feature = featureSize(bunny.vertices);
    float modelScaleFactor = feature * 1000;
    cout << "Feature size of model : " << feature << "\n";
    for (int i = 0; i < 20; i++) sigma[i] = 0.4 * feature * glm::pow(glm::sqrt(2),float(i));
    glm::vec3 cen = center(bunny.vertices); cout << "Center of model : " << cen.x << ", " << cen.y << ", " << cen.x << "\n";
    cout << "Diagonal Size of Model : "<< modelScaleFactor <<"\n";


    //Load Shaders
    GLuint cosine = loadShader("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/cosine.vs","C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/cosine.fs");
    GLuint xShade = loadShader("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/xShade.vs","C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/xShade.fs");
    GLuint softToon = loadShader("C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/cosine.vs", "C:/Users/lab/Desktop/yj/ExaggeratedShadingInteractive/shaders/softToon.fs");
    GLuint xShadePD = loadShader("..\\shaders\\xShadePD.vs","..\\shaders\\xShadePD.fs");
    GLuint PDmax = loadShader("..\\shaders\\PDmax.vs", "..\\shaders\\PDmax.fs", "..\\shaders\\PDmax.gs");
    GLuint PDmin = loadShader("..\\shaders\\PDmin.vs", "..\\shaders\\PDmin.fs", "..\\shaders\\PDmin.gs");
 

    
    
    GLuint* currentShader=&xShade;

    //view
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    //light settings
    glm::vec3 lightPosInit = glm::vec3(-1.0f, 1.0f, 1.5f);
    glm::vec3 lightPos = lightPosInit;
    glm::vec3 lightDiffuse = glm::vec3(1, 1, 1)*diffuse;



    //render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //yDegrees += 1;
        //yDegrees =int(yDegrees)%360;

        glClearColor(background.x, background.y, background.z, 0.0); //background

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //IMGui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //IMGui window
        ImGui::Begin("XShade Interactive");


        
        const char* listbox_shading_items[] = { "Exaggerated Shading", "Principal Direction Exaggerated Shading", "Diffuse"};
        static int listbox_item_shading = 2;
        ImGui::ListBox("Shading Method", &listbox_item_shading, listbox_shading_items, IM_ARRAYSIZE(listbox_shading_items), 3);
        currentShading = shadingType(listbox_item_shading);

        ImGui::Checkbox("Show principal directions", &PDon);
        
        ImGui::SliderFloat("Rotate X", &xDegrees, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotate Y", &yDegrees, 0.0f, 360.0f);
        ImGui::SliderFloat("Model Size", &modelSize, 0.1f, 10.0f);
        ImGui::SliderFloat("Rotate Global Light Source", &lightDegrees, 0.0f, 360.0f);
        //ImGui::SliderFloat("Brightness", &diffuse, 0.0f, 2.0f);
        ImGui::SliderInt("Number of Smoothing Scales", &scales, 1, 20);
        ImGui::SliderFloat("Contribution factor of ki", &contributionScale, -5.0f, 5.0f);
        ImGui::SliderFloat("Clamp Coefficient for Light at Each Scale", &clampCoef, 1.0f, 200.0f);
        ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
        ImGui::SliderFloat("Principal Direction Contribution", &pdScaler, 0.0f, 10.0f);
        ImGui::SliderFloat("Principal Direction Arrow Size", &magScale, 0.0f, 2.0f);
        ImGuiColorEditFlags misc_flags = (0 | ImGuiColorEditFlags_NoDragDrop | 0 | ImGuiColorEditFlags_NoOptions);
        ImGui::ColorEdit3("Background Color", (float*)&background, misc_flags);
        ImGui::ColorEdit3("Texture Bright Color", (float*)&textureHigh, misc_flags);
        ImGui::ColorEdit3("Texture Dark Color", (float*)&textureLow, misc_flags);
        ImGui::End();

        switch (currentShading) {
        case ExaggeratedShading:
            currentShader = &xShade;
            break;
        case ExaggeratedShadingPD:
            currentShader = &xShadePD;
            break;
        case Diffuse:
            currentShader = &cosine;
            break;
        }


        //contribution factor
        GLfloat contributionBeforeNorm[20]={0};
        GLfloat contributionSum = 0;
        for(int i=0;i<scales;i++){
            contributionBeforeNorm[i]=glm::pow(sigma[i],contributionScale);
            contributionSum += contributionBeforeNorm[i];
        }
        for (int i = 0; i < scales; i++) {
            contribution[i]=contributionBeforeNorm[i]/contributionSum;
            //cout << "Contribution " << i << " : " << contribution[i] << "\n";
        }
        

        glUseProgram(*currentShader);

        //Uniforms
        glm::mat4 lightRotate = glm::rotate(glm::mat4(1), glm::radians(lightDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
        lightPos = glm::vec3(lightRotate * glm::vec4(lightPosInit, 0.0f));

        glUniform3f(glGetUniformLocation(*currentShader, "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(*currentShader, "light.diffuse"), lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
        glUniform3f(glGetUniformLocation(*currentShader,"highColor"),textureHigh.x, textureHigh.y, textureHigh.z);
        glUniform3f(glGetUniformLocation(*currentShader, "lowColor"), textureLow.x, textureLow.y, textureLow.z);
        glUniform1f(glGetUniformLocation(*currentShader, "ambient"), ambient);
        if (currentShading!=Diffuse) {
            // YOU NEED TO BIND PROGRAM WITH glUseProgram BEFORE glUniform
            //send contribution ki to shader as uniform (array)
            glUniform1fv(glGetUniformLocation(*currentShader, "contribution"), 20, contribution);
            glUniform1f(glGetUniformLocation(*currentShader,"clampCoef"),clampCoef);
            glUniform1i(glGetUniformLocation(*currentShader, "scales"), scales);
            if(currentShading==ExaggeratedShadingPD){
                glUniform1f(glGetUniformLocation(*currentShader,"pdScaler"),pdScaler);
            }
        }

        //opengl matrice transforms are applied from the right side. (last first)
        glm::mat4 model = glm::mat4(1);
        //model = glm::translate(model, glm::vec3(0,0.0, -1.0f));
        //model = glm::translate(model, ((-1.0f) * 1.0f / modelScaleFactor) * cen + glm::vec3(0.0,-0.5,-1.0f));
        
        //model = glm::translate(model, glm::vec3(0.0f, -0.5f, -1.0f));
        model = glm::translate(model, glm::vec3(0.0f,0.0f,-1.0f));
        model = glm::scale(model, glm::vec3(modelSize, modelSize, modelSize));
        model = glm::rotate(model, glm::radians(yDegrees), glm::vec3(0.0f,1.0f,0.0f));
        model = glm::rotate(model, glm::radians(xDegrees), glm::vec3(1.0f,0.0f,0.0f));
       
        model = glm::scale(model, glm::vec3(1.0f / modelScaleFactor, 1.0f / modelScaleFactor, 1.0f / modelScaleFactor));
        model = glm::translate(model, (-1.0f * cen));
        //glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0,0,-1), glm::vec3(0,1,0));
        glm::mat4 view = glm::lookAt(cameraPos,  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        glUniformMatrix4fv(glGetUniformLocation(*currentShader, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(*currentShader, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(*currentShader, "projection"), 1, GL_FALSE, &projection[0][0]);

        //printShader(bunny, contribution);

        bunny.render(*currentShader);


        //Principal Directions
        /**/
        if (PDon) {
            glUseProgram(PDmax);
            glUniform1f(glGetUniformLocation(PDmax,"magnitude"), magScale / modelScaleFactor*modelSize);
            glUniformMatrix4fv(glGetUniformLocation(PDmax, "model"), 1, GL_FALSE, &model[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(PDmax, "view"), 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(PDmax, "projection"), 1, GL_FALSE, &projection[0][0]);
            bunny.render(PDmax);

            glUseProgram(PDmin);
            //glUniform1f(glGetUniformLocation(PDmin, "magnitude"), magScale* 0.000024f / modelScaleFactor* modelSize);
            glUniform1f(glGetUniformLocation(PDmin, "magnitude"), magScale  / modelScaleFactor * modelSize);
            glUniformMatrix4fv(glGetUniformLocation(PDmin, "model"), 1, GL_FALSE, &model[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(PDmin, "view"), 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(PDmin, "projection"), 1, GL_FALSE, &projection[0][0]);
            bunny.render(PDmin);
        }

        glUseProgram(0);

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
