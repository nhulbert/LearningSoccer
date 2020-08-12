#ifndef VIEW_H
#define VIEW_H

#include "object.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <vector>

class View
{
	public:
	    View() : isRendering(true)
	    {
	        View::setup();
	    }
	    ~View();
	    int setup();
	    int displayView(std::vector<Object*>* objects, float delta, int* pressedKey);
	    void processInput(GLFWwindow *window, int* pressedKey);
	    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	private:
	    GLFWwindow* window;
	    int vertexShader;
	    int success;
	    char infoLog[512];
	    int fragmentShader;
	    int shaderProgram;
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec4 lightPosition;
        bool isRendering;
};

#endif
