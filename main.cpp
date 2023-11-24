#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/transform.hpp"
using namespace glm;
struct STRVertex
{
vec3 position;
vec3 couleur; };
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// const char *vertexShaderSource = "#version 330 core\n"
//                                  "layout(location = 0) in vec4 VertexPosition;\n"
//                                  "layout(location = 1) in vec4 VertexColor;\n"
//                                  "uniform mat4 MVP;\n"
//                                  "out vec4 Color;\n" // Declare an output variable for color
//                                  "void main()\n"
//                                  "{\n"
//                                  "   gl_Position = MVP * vec4(VertexPosition.xyz, 1.0);\n"
//                                  "   Color = VertexColor;\n" // Pass the color to the fragment shader
//                                  "}\0";
const char *vertexShaderSource = 
                                "#version 330 core\n"

                                "layout(location = 0) in vec3 VertexPosition;\n"
                                "layout(location = 1) in vec3 VertexColor;\n"
                                "uniform mat4 MVP;\n"

                                "out vec3 Vcolor;\n"
                                "void main(){\n"
                                    "gl_Position = MVP * vec4(VertexPosition, 1);\n"

                                    //gl_Position.xyz = VertexPosition;
                                    //gl_Position.w = 1.0;

                                    "Vcolor = VertexColor;\n"
                                "}\0";


// const char *fragmentShaderSource = "#version 330 core\n"
//                                    "out vec3 FragColor;\n"
//                                    "void main()\n"
//                                    "{\n"
//                                    "   FragColor = vec3(1, 0, 0);\n"
//                                    "}\n\0";

const char *fragmentShaderSource = 
                            "#version 330 core\n"

                            "out vec3 color;\n"
                            "in vec3 Vcolor;\n"

                            "void main()\n"
                            "{\n"   
                                "color = Vcolor;   //vec3(1,0,0);\n"
                            "}\n\0";

mat4 Model = mat4(1.0f);
mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
mat4 View = lookAt(vec3(5,4,7),vec3(-1,0,0),vec3(2,3,2) );  
static double Xpos = 0.0, Ypos = 0.0;
static void cursor_position(GLFWwindow* window, double x, double y)
{

double diffX;
diffX=Xpos-x;
double diffY;
diffY=y-Ypos;
Model = rotate(Model,radians(float(diffX)),vec3(0.0f,0.0f,1.0f)); 
Model = rotate(Model,radians(float(diffY)),vec3(1.0f,0.0f,0.0f));
}
int fov = 45;
void scroll (GLFWwindow * window, double xoffset, double yoffset){
if (fov >= 1.0f && fov <= 45.0f)
fov -= yoffset; if (fov <= 1.0f)
fov = 1.0f; if (fov >= 45.0f)
fov = 45.0f;
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
if (action == GLFW_PRESS)
switch (key)
{
case GLFW_KEY_R :
glClearColor(1.0f, 0.0f, 0.0f, 0.5f);
break;
case GLFW_KEY_V :
glClearColor(0.0f, 1.0f, 0.0f, 0.5f);
break;
case GLFW_KEY_B :
glClearColor(0.0f, 0.0f, 1.0f, 0.5f);
break;
case GLFW_KEY_ESCAPE:
glfwSetWindowShouldClose(window, GLFW_TRUE);
break; default:
glClearColor(0.0f, 0.0f, 0.0f, 0.5f); }
else if (action== GLFW_RELEASE){
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
}
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    

    
    Model = translate(Model,vec3(-1.0f, 0.0f, 0.0f));
    Model = scale(Model,vec3(2.5f, 1.5f ,1.0f));
    mat4 MVP = Projection * View * Model;
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");





    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    STRVertex vertices[] = {
        //2 triangles de la base :
        vec3(1.0f, 0.0f, 1.0f),  vec3(0.0f, 0.0f, 1.0f),
        vec3(-1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-1.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f),  //comuns
        vec3(-1.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f),
        vec3(1.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f),
        vec3(1.0f, 0.0f, 1.0f),  vec3(0.0f, 0.0f, 1.0f),     //comuns

        //ajouter les 4 triangles :
        vec3(1.0f, 0.0f, 1.0f),  vec3(1.0f, 0.0f, 1.0f),
        vec3(-1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f),
        vec3(0.0f, 1.0f, 0.0f),  vec3(1.0f, 0.0f, 1.0f),    //sommet

        vec3(-1.0f, 0.0f, 1.0f),  vec3(0.0f, 1.0f, 1.0f),
        vec3(-1.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 1.0f),
        vec3(0.0f, 1.0f, 0.0f),   vec3(0.0f, 1.0f, 1.0f),    //sommet

        vec3(-1.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),  vec3(1.0f, 0.0f, 0.0f),     //sommet

        vec3(1.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f),
        vec3(1.0f, 0.0f, 1.0f),  vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 1.0f, 0.0f)   //sommet
        };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(STRVertex) , (void*)offsetof(STRVertex,position) );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(STRVertex) , (void*)offsetof(STRVertex,couleur) );
    glEnableVertexAttribArray(1);
    

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but we'll do so to keep things a bit more organized
    glBindVertexArray(0);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position);
    glfwSetScrollCallback(window, scroll);
    while (!glfwWindowShouldClose(window))
    {

glfwGetCursorPos(window, &Xpos, &Ypos);
std::cout << "La position du curseur : " << Xpos << " , " << Ypos << std::endl;

//if ( glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
//std::cout << " Bouton gauche de la souris appuyé " << std::endl;
mat4 MVP = Projection * View * Model;
GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");

        processInput(window);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 54);

        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
