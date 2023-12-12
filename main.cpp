#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "objload.hpp"
using namespace glm;
using namespace std;

using namespace glm;
using namespace std;

#define _CRT_SECURE_NO_WARNINGS

// Very, VERY simple OBJ loader.

bool loadOBJ(
    const char *path,
    std::vector<glm::vec3> &out_vertices,
    std::vector<glm::vec2> &out_uvs,
    std::vector<glm::vec3> &out_normals)
{
    printf("Loading OBJ file %s...\n", path);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Impossible to open the file !\n");
        getchar();
        return false;
    }

    while (1)
    {

        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF
        if (strcmp(lineHeader, "v") == 0)
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uv.y = -uv.y;
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9)
            {
                printf("File can't be read \n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else
        {
            // Probably a comment
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

    cout << vertexIndices.size() << std::endl;

    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        glm::vec3 normal = temp_normals[normalIndex - 1];

        // Put the attributes in buffers
        out_vertices.push_back(vertex); // std::cout<<vertex.x<< " " << vertex.y<< " " << vertex.z << "       ";

        out_uvs.push_back(uv); // std::cout<< uv.x<<" " << uv.y << "       ";

        out_normals.push_back(normal); // std::cout<<normal.x<<" " <<normal.y<<" "<<normal.z<<std::endl;
    }

    return true;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

vector<vec3> posion;
vector<vec2> texture;
vector<vec3> normals;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource =
    "#version 330 core\n"

    "layout(location = 0) in vec3 VertexPosition;\n"
    "layout(location = 1) in vec3 VertexColor;\n"
    "uniform mat4 MVP;\n"
    "out vec3 Vcolor;\n"
    "void main(){\n"
    "gl_Position = MVP * vec4(VertexPosition, 1);\n"
    "Vcolor = VertexColor;\n"
    "}\0";

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
mat4 View = lookAt(vec3(5, 4, 7), vec3(-1, 0, 0), vec3(2, 3, 2));
static double Xpos = 0.0, Ypos = 0.0;
static void cursor_position(GLFWwindow *window, double x, double y)
{

    double diffX;
    diffX = Xpos - x;
    double diffY;
    diffY = y - Ypos;
    Model = rotate(Model, radians(float(diffX)), vec3(0.0f, 0.0f, 1.0f));
    Model = rotate(Model, radians(float(diffY)), vec3(1.0f, 0.0f, 0.0f));
}
int fov = 45;
void scroll(GLFWwindow *window, double xoffset, double yoffset)
{
    // Adjust the field of view based on the scroll offset
    fov -= yoffset * 5.0; // You can adjust the sensitivity here

    // Clamp the field of view to reasonable values
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;

    // Update the projection matrix with the new field of view
    Projection = perspective(radians(static_cast<float>(fov)), 4.0f / 3.0f, 0.1f, 100.0f);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
        switch (key)
        {
        case GLFW_KEY_R:
            glClearColor(1.0f, 0.0f, 0.0f, 0.5f);
            break;
        case GLFW_KEY_V:
            glClearColor(0.0f, 1.0f, 0.0f, 0.5f);
            break;
        case GLFW_KEY_B:
            glClearColor(0.0f, 0.0f, 1.0f, 0.5f);
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        default:
            glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
        }
    else if (action == GLFW_RELEASE)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    }
}
int main()
{
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

    Model = translate(Model, vec3(-1.0f, 0.0f, 0.0f));
    Model = scale(Model, vec3(2.5f, 1.5f, 1.0f));
    mat4 MVP = Projection * View * Model;
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");

    loadOBJ("torus.obj", posion, texture, normals);

    unsigned int VBO, VAO;
    unsigned int VBONormal;
    // unsigned int  VBOTexture;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, posion.size() * sizeof(vec3), &posion[0], GL_STATIC_DRAW);

    // Correction de noms de fonctions OpenGL et indices de vertex attribs
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBONormal);
    glBindBuffer(GL_ARRAY_BUFFER, VBONormal);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(1);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but we'll do so to keep things a bit more organized
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    processInput(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position);
    glfwSetScrollCallback(window, scroll);
    // Declare global variables for camera
    vec3 camPos = vec3(0.0f, 0.0f, 3.0f);
    vec3 camDirection = vec3(0.0f, 0.0f, -1.0f);
    vec3 camUp = vec3(0.0f, 1.0f, 0.0f);
    vec3 camRight = vec3(1.0f, 0.0f, 0.0f);
    float camSpeed = 0.1f; // Adjust the speed as needed
    while (!glfwWindowShouldClose(window))
    {
        glfwGetCursorPos(window, &Xpos, &Ypos);
        // std::cout << "La position du curseur : " << Xpos << " , " << Ypos << std::endl;
        // if ( glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        // std::cout << " Bouton gauche de la souris appuyÃ© " << std::endl;
        //  Trackball camera
        // FreeFly camera movement
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            std::cout << "E key pressed!" << std::endl;
            camPos += camSpeed * camDirection;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            std::cout << "S key pressed!" << std::endl;
            camPos -= camSpeed * camDirection;
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            camPos -= camSpeed * camRight;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camPos += camSpeed * camRight;

        // Update the View matrix
        View = lookAt(camPos, camPos + camDirection, camUp);

        // Update the MVP matrix
        mat4 MVP = Projection * View * Model;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, posion.size());
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
