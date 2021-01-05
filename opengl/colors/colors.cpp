#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

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

// 声明一个全局vec3变量来表示光源在场景的世界空间坐标中的位置：
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("1.colors.vs", "1.colors.fs");

    // 不希望灯的颜色在接下来的教程中因光照计算的结果而受到影响，而是希望它能够与其它的计算分离。
    // 我们希望灯一直保持明亮，不受其它颜色变化的影响（这样它才更像是一个真实的光源）。
    // 为了实现这个目标，我们需要为灯的绘制创建另外的一套着色器，从而能保证它能够在其它光照着色
    // 器发生改变的时候不受影响。
    Shader lightCubeShader("1.light_cube.vs", "1.light_cube.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
    };

    // 我们还要创建一个表示灯（光源）的立方体，所以我们还要为这个灯创建一个专门的VAO。当然我们也可以让
    // 这个灯和其它物体使用同一个VAO，简单地对它的model（模型）矩阵做一些变换就好了，然而接下来的教程
    // 中我们会频繁地对顶点数据和属性指针做出修改，我们并不想让这些修改影响到灯（我们只关心灯的顶点位置），
    // 因此我们有必要为灯创建一个新的VAO。
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    // 只需要绑定VBO不用再次设置VBO的数据，因为箱子的VBO数据中已经包含了正确的立方体顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // 设置灯立方体的顶点属性（对我们的灯来说仅仅只有位置数据）
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // 设定uniform
        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        // 绿色的光源:
        // lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        // lightingShader.setVec3("lightColor",  0.0f, 1.0f, 0.0f);
        // 深橄榄绿色(Dark olive-green)的光源:
        // lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        // lightingShader.setVec3("lightColor",  0.33f, 0.42f, 0.18f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        // 把灯位移到这里，然后将它缩小一点，让它不那么明显：
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        // 绘制灯立方体
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
    camera.ProcessMouseScroll(yoffset);
}


// 颜色可以数字化的由红色(Red)、绿色(Green)和蓝色(Blue)三个分量组成，它们通常被缩写为RGB。
// 仅仅用这三个值就可以组合出任意一种颜色。
// glm::vec3 coral(1.0f, 0.5f, 0.31f); // 定义颜色向量珊瑚红

// 我们在现实生活中看到某一物体的颜色并不是这个物体真正拥有的颜色，而是它所反射的(Reflected)颜色。
// 换句话说，那些不能被物体所吸收(Absorb)的颜色（被拒绝的颜色）就是我们能够感知到的物体的颜色。
// 例如，太阳光能被看见的白光其实是由许多不同的颜色组合而成的。如果我们将白光照在一个蓝色的玩具上，
// 这个蓝色的玩具会吸收白光中除了蓝色以外的所有子颜色，不被吸收的蓝色光被反射到我们的眼中。白色的阳
// 光实际上是所有可见颜色的集合，物体吸收了其中的大部分颜色。它仅反射了代表物体颜色的部分，被反射颜
// 色的组合就是我们所感知到的颜色。

// 这些颜色反射的定律被直接地运用在图形领域。当我们在OpenGL中创建一个光源时，我们希望给光源一个颜色。
// 我们也将光源设置为白色。当我们把光源的颜色与物体的颜色值相乘，所得到的就是这个物体所反射的颜色（也
// 就是我们所感知到的颜色）。
// glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
// glm::vec3 toyColor(1.0f, 0.5f, 0.31f);
// glm::vec3 result = lightColor * toyColor; // = (1.0f, 0.5f, 0.31f);
// 使用绿色的光源，并没有红色和蓝色的光让我们的玩具来吸收或反射。吸收了光线中一半的绿色值，但仍然也反
// 射了一半的绿色值。玩具现在看上去是深绿色(Dark-greenish)的。
// glm::vec3 lightColor(0.0f, 1.0f, 0.0f);
// glm::vec3 toyColor(1.0f, 0.5f, 0.31f);
// glm::vec3 result = lightColor * toyColor; // = (0.0f, 0.5f, 0.0f);
// 深橄榄绿色(Dark olive-green)的光源：
// glm::vec3 lightColor(0.33f, 0.42f, 0.18f);
// glm::vec3 toyColor(1.0f, 0.5f, 0.31f);
// glm::vec3 result = lightColor * toyColor; // = (0.33f, 0.21f, 0.06f);



