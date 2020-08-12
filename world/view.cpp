#include <glad/glad.h>
#include <iostream>
#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <math.h>
#include <vector>
#include <string>
#include "view.h"
#include "object.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3  aNormal;\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform mat3 normMat;\n"
    "out vec3 Normal;\n"
    "out vec3 FragPos;\n"
    
    "void main()\n"
    "{\n"
    "   vec4 camPos = modelview * vec4(aPos, 1.0);\n"
    "   gl_Position = projection * camPos;\n"
    "	Normal = normMat * aNormal;\n"
    "   FragPos = vec3(camPos);\n"
    "}\n\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "out vec4 FragColor;\n"
    
    "uniform vec3 viewPos;\n"
    "uniform vec3 lightPosition;\n"
    
    "void main()\n"
    "{\n"
    "   vec3 normNorm = normalize(Normal);\n"
    "	vec3 lightDirNorm = normalize(lightPosition - FragPos);\n"
    "   float ambientComp = 0.2;\n"
    "	float diffComp = 0.5*max(dot(normNorm, lightDirNorm), 0.0);\n"
    "   float specComp = pow(max(0.0, dot(normalize(-FragPos), reflect(-lightDirNorm, normNorm))), 32);\n"
    "   FragColor = vec4(min(ambientComp+diffComp+specComp, 1.0) * vec3(0.0f, 1.0f, 0.0f), 1.0);\n"
    "}\n\0";

int View::setup()
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Soccer Agent Simulator", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    // vertex shader    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    
    view = glm::lookAt(glm::vec3(0.0f, 50.0f, 80.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    /*for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            printf("%f, ", view[i][j]);
        }
        printf("\n");
    }*/
    
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f);
    lightPosition = glm::vec4(0.0f, 50.0f, 0.0f, 1.0f);
    
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    return 0;
}

int View::displayView(std::vector<Object*>* objects, float delta, int* pressedKey)
{
    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        isRendering = true;
    }
    
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        isRendering = false;
    }
    
    processInput(window, pressedKey);
    
    if (glfwWindowShouldClose(window))
    {
        return 1;
    }
    
    if (isRendering)
    {        
        glm::mat4 rot(1.0f);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            rot = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f * delta), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            rot = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f * delta), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        
        view = view * rot;

        glm::vec4 viewPos = *(glm::vec4*)(&view[3][0]);

        glUseProgram(shaderProgram);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glBindVertexArray(0); // no need to unbind it every time 

        

        int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
        glUniform3fv(viewPosLoc, 1, (float*)&viewPos);

        int lightPositionLoc = glGetUniformLocation(shaderProgram, "lightPosition");
        glUniform3fv(lightPositionLoc, 1, glm::value_ptr(view * lightPosition));

        for (int i=0; i<objects->size(); i++)
        {
            (*objects)[i]->draw(shaderProgram, view);
        }
        
        glfwSwapBuffers(window);
    }
    
    glfwPollEvents();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void View::processInput(GLFWwindow *window, int* pressedKey)
{
    *pressedKey = 0;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        *pressedKey |= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        *pressedKey |= 2;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        *pressedKey |= 4;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        *pressedKey |= 8;
    }
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void View::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

View::~View()
{
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteProgram(shaderProgram);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    
    glfwTerminate();
    glfwDestroyWindow(window);
    window = NULL;
}
