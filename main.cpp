#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "objload.hpp"
using namespace glm;

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

    "layout(location = 0) in vec3 inPosition;\n"
    "layout(location = 1) in vec3 inColor;\n"
    "layout(location = 2) in vec2 inTexCoord;\n"
    "layout(location = 3) in int inFaceIndex;\n"

    "out vec3 color;\n"
    "out vec2 texCoord;\n"
    "flat out int indexFace;\n"

    "uniform mat4 MVP;\n"

    "void main(){\n"
    "gl_Position = MVP * vec4(inPosition, 1.0);\n"

    "color = inColor;\n"
    "texCoord = inTexCoord;\n"
    "indexFace = inFaceIndex;\n"
    "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"

    "in vec3 color;\n"
    "in vec2 texCoord;\n"
    "flat in int indexFace;\n"

    "out vec4 outColor;\n"

    "uniform sampler2D OurTexture;\n"

    "void main()\n"
    "{\n"
    "vec2 combinedTexCoord = texCoord + vec2(float(indexFace), 0.0);\n"
    "outColor = texture(OurTexture, combinedTexCoord);\n"
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

struct STRVertex
{
    vec3 position;
    vec3 color;
    vec2 texCoord;
    int indexFace;
};

GLuint textures[5]; 

void InitTexture(void)
{
    char data[128 * 128 * 3];
    FILE* f = fopen("textures/damier.raw", "rb");
    if (f)
    {
        fread(data, 128 * 128 * 3, 1, f);
        fclose(f);
        glGenTextures(1, &textures[0]);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }


    // Load texture for face 0
    FILE* f0 = fopen("textures/1.raw", "rb");
    fread(data, 128 * 128 * 3, 1, f0);
    fclose(f0);
    glGenTextures(1, &textures[1]);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Load texture for face 1
    FILE* f1 = fopen("textures/2.raw", "rb");
    fread(data, 128 * 128 * 3, 1, f1);
    fclose(f1);
    glGenTextures(1, &textures[2]);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Load texture for face 2
    FILE* f2 = fopen("textures/3.raw", "rb");
    fread(data, 128 * 128 * 3, 1, f2);
    fclose(f2);
    glGenTextures(1, &textures[3]);
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Load texture for face 2
    FILE* f3 = fopen("textures/4.raw", "rb");
    fread(data, 128 * 128 * 3, 1, f3);
    fclose(f3);
    glGenTextures(1, &textures[4]);
    glBindTexture(GL_TEXTURE_2D, textures[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Load texture for face 2
    FILE* f4 = fopen("textures/5.raw", "rb");
    fread(data, 128 * 128 * 3, 1, f4);
    fclose(f4);
    glGenTextures(1, &textures[5]);
    glBindTexture(GL_TEXTURE_2D, textures[5]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
};

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

    Model = translate(Model, vec3(-1.0f, 0.0f, 0.0f));
    Model = scale(Model, vec3(2.5f, 1.5f, 1.0f));
    mat4 MVP = Projection * View * Model;
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    STRVertex vertices[] = {
        // CarrÃ©
        {vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), 0}, 
        {vec3(-1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), 0},
        {vec3(-1.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), 0},
        

        {vec3(-1.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), 0},
        {vec3(1.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), 0},
        {vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), 0},

        // Triangle 1
        {vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f),1},
        {vec3(-1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f),1},
        {vec3(-1.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f),1},

        // Triangle 2
        {vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f), 2},
        {vec3(1.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), 2},
        {vec3(-1.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f), 2},

        // Triangle 3
        {vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f),3},
        {vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f),3},
        {vec3(-1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f),3},

        // Triangle 4
        {vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), 4},
        {vec3(1.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), 4},
        {vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), 4},
    };

    unsigned int vertex_index []={0, 1, 2,
                                2, 3, 1,
                                4, 5, 2,
                                4, 3, 2,
                                4, 0, 1,
                                4, 3, 1};


    GLuint VAO;
    GLuint VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribut 0 : position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(STRVertex), (void*)offsetof(STRVertex, position));
    glEnableVertexAttribArray(0);

    // Attribut 2 : coordonnÃ©es de texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(STRVertex), (void*)offsetof(STRVertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load the texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    std::vector<vec3> position; 
    std::vector<vec2> texture; 
    std::vector<vec3> normals;

    // render loop
    // -----------
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position);
    glfwSetScrollCallback(window, scroll);
    float radius = 5.0f;
    float cameraSpeed = 1.0f;
    InitTexture();
    GLuint TextureID = glGetUniformLocation(shaderProgram, "OurTexture");
    while (!glfwWindowShouldClose(window))
    {

        glfwGetCursorPos(window, &Xpos, &Ypos);
        // std::cout << "La position du curseur : " << Xpos << " , " << Ypos << std::endl;
        // if ( glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        // std::cout << " Bouton gauche de la souris appuyÃƒÂ© " << std::endl;
        //  Trackball camera
        float cameraX = radius * sin(glfwGetTime() * cameraSpeed);
        float cameraZ = radius * cos(glfwGetTime() * cameraSpeed);
        View = lookAt(vec3(cameraX, 0.0, cameraZ), vec3(0, 0, 0), vec3(0, 1, 0));
        mat4 MVP = Projection * View * Model;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // draw our first triangle
        glUseProgram(shaderProgram);
        //glBindVertexArray(VAO);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(TextureID, 0);

        for (int i = 0; i < 6; ++i) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[vertices[i * 3].indexFace]);
            glUniform1i(TextureID, 0);

            glDrawArrays(GL_TRIANGLES, i * 3, 3);
            
        }

        glUniform1i(TextureID, 0);
        glDrawArrays(GL_TRIANGLES, 0, 18); // Assuming 18 vertices for the entire geometry


        glDrawArrays(GL_TRIANGLES, 0, 18);
        //glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, (void*)0);

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