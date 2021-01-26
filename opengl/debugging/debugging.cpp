#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 默认情况下glGetError只会打印错误数字，如果你不去记忆的话会非常难以理解。通常我们会写一个助手函数来简便地打印出错误字符串以及错误检测函数调用的位置。
GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// 下面是这个就是OpenGL对调试输出所期待的回调函数的原型：
void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // comment this line in a release build! 

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // enable OpenGL debug context if context allows for debug context
    // 检查我们是否成功地初始化了调试上下文，我们可以对OpenGL进行查询：
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
 
    // OpenGL initial state
    Shader shader("debugging.vs", "debugging.fs");

    // configure 3D cube
    unsigned int cubeVAO, cubeVBO;
    float vertices[] = {
         // back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, // bottom-left
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f, // bottom-right         
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f, // top-left
         // front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f,  1.0f, // top-right
         0.5f,  0.5f,  0.5f,  1.0f,  1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, // bottom-left
         // left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f, // top-right
        -0.5f,  0.5f, -0.5f, -1.0f,  1.0f, // top-left
        -0.5f, -0.5f, -0.5f, -0.0f,  1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f, -0.0f,  1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f, -0.0f,  0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f, // top-right
         // right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f, // top-left
         0.5f, -0.5f, -0.5f,  0.0f,  1.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f, // top-right         
         0.5f, -0.5f, -0.5f,  0.0f,  1.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f, // top-left
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, // bottom-left     
         // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f,  1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f,  1.0f, // top-left
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f,  1.0f, // top-right
         // top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f, // top-left
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f, // top-right     
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f  // bottom-left        
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load cube texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    int width, height, nrComponents;
    unsigned char *data = stbi_load("../source/resources/wood.png", &width, &height, &nrComponents, 0);
    if (data)
    {
        glTexImage2D(GL_FRAMEBUFFER, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // set up projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(shader.ID, "tex"), 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        float rotationSpeed = 10.0f;
        float angle = (float)glfwGetTime() * rotationSpeed;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 0.0f, -2.5));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// 图形编程可以带来很多的乐趣，然而如果什么东西渲染错误，或者甚至根本就没有渲染，它同样可以给你带来大量的沮丧感！由于我们大部分时间都在与像素打交道，当出现错误的
// 时候寻找错误的源头可能会非常困难。调试(Debug)这样的视觉错误与往常熟悉的CPU调试不同。我们没有一个可以用来输出文本的控制台，在GLSL代码中也不能设置断点，更没有
// 方法检测GPU的运行状态。

// glGetError():
// 当你不正确使用OpenGL的时候（比如说在绑定之前配置一个缓冲），它会检测到，并在幕后生成一个或多个用户错误标记。我们可以使用一个叫做glGetError的函数查询这些错误
// 标记，他会检测错误标记集，并且在OpenGL确实出错的时候返回一个错误值。GLenum glGetError();
// 当glGetError被调用时，它要么会返回错误标记之一(在OpenGL的函数文档中你可以找到函数在错误时生成的错误代码)，要么返回无错误。glGetError会返回的错误值如下：
// GL_NO_ERROR 	                        0 	    自上次调用glGetError以来没有错误
// GL_INVALID_ENUM 	                    1280 	枚举参数不合法
// GL_INVALID_VALUE 	                1281 	值参数不合法
// GL_INVALID_OPERATION 	            1282 	一个指令的状态对指令的参数不合法
// GL_STACK_OVERFLOW 	                1283 	压栈操作造成栈上溢(Overflow)
// GL_STACK_UNDERFLOW 	                1284 	弹栈操作时栈在最低点（译注：即栈下溢(Underflow)）
// GL_OUT_OF_MEMORY 	                1285 	内存调用操作无法调用（足够的）内存
// GL_INVALID_FRAMEBUFFER_OPERATION 	1286 	读取或写入一个不完整的帧缓冲

// 当一个错误标记被返回的时候，将不会报告其它的错误标记。换句话说，当glGetError被调用的时候，它会清除所有的错误标记。这也就是说如果你在每一帧之后调用glGetError一
// 次，它返回一个错误，但你不能确定这就是唯一的错误，并且错误的来源可能在这一帧的任意地方。
// 注意当OpenGL是分布式(Distributely)运行的时候，如在X11系统上，其它的用户错误代码仍然可以被生成，只要它们有着不同的错误代码。调用glGetError只会重置一个错误代
// 码标记，而不是所有。由于这一点，我们通常会建议在一个循环中调用glGetError。
// glBindTexture(GL_TEXTURE_2D, tex);
// std::cout << glGetError() << std::endl; // 返回 0 (无错误)
// glTexImage2D(GL_TEXTURE_3D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
// std::cout << glGetError() << std::endl; // 返回 1280 (非法枚举)
// glGenTextures(-5, textures);
// std::cout << glGetError() << std::endl; // 返回 1281 (非法值)
// std::cout << glGetError() << std::endl; // 返回 0 (无错误)

// glGetError非常棒的一点就是它能够非常简单地定位错误可能的来源，并且验证OpenGL使用的正确性。比如说你获得了一个黑屏的结果但是不知道什么造成了它：是不是帧缓冲设置
// 错误？是不是我忘记绑定纹理了？通过在代码中各处调用glGetError，你可以非常快速地查明OpenGL错误开始出现的位置，这也就意味着这次调用之前的代码中哪里出错了。

// 如果我们坚持在代码中使用大量glGetError的调用，这就会让我们更加准确地知道哪个glGetError调用返回了错误（记得glGetError显示的错误会发生在该次调用与上次调用之间，
// 如果间隔太大的话需要检查的地方就太多了）。

// 还有一个重要的事情需要知道，GLEW有一个历史悠久的bug，调用glewInit()会设置一个GL_INVALID_ENUM的错误标记，所以第一次调用的glGetError永远会猝不及防地给你返回
// 一个错误代码。如果要修复这个bug，我们建议您在调用glewInit之后立即调用glGetError消除这个标记：
// glewInit();
// glGetError();
// glGetError并不能帮助你很多，因为它返回的信息非常简单，但不可否认它经常能帮助你检查笔误或者快速定位错误来源。总而言之，是一个非常简单但有效的工具。

// 调试输出:
// 虽然没有glGetError普遍，但一个叫做调试输出(Debug Output)的OpenGL拓展是一个非常有用的工具，它在4.3版本之后变为了核心OpenGL的一部分。通过使用调试输出拓展，
// OpenGL自身会直接发送一个比起glGetError更为完善的错误或警告信息给用户。它不仅提供了更多的信息，也能够帮助你使用调试器(Debugger)捕捉错误源头。
// 调试输出自4.3版本变为核心OpenGL的一部分，这也就是说你可以在任意运行OpenGL 4.3及以上版本的机器中找到这一功能。如果OpenGL低于这一版本，你可以可以查询
// ARB_debug_output或者AMD_debug_output拓展来获取它的功能。注意OS X好像不支持调试输出功能

// GLFW中的调试输出:
// 在GLFW中请求一个调试输出非常简单，我们只需要传递一个提醒到GLFW中，告诉它我们需要一个调试输出上下文即可。我们需要在调用glfwCreateWindow之前完成这一请求。
// glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
// 在调试上下文中使用OpenGL会明显更缓慢一点，所以当你在优化或者发布程序之前请将这一GLFW调试请求给注释掉。
// 调试输出工作的方式是这样的，我们首先将一个错误记录函数的回调（类似于GLFW输入的回调）传递给OpenGL，在这个回调函数中我们可以自由地处理OpenGL错误数据，在这里我们将
// 输出一些有用的错误数据到控制台中。

// 回溯调试错误源:
// 使用调试输出另一个很棒的技巧就是你可以很容易找出错误发生的准确行号或者调用。通过在DebugOutput中特定的错误类型上（或者在函数的顶部，如果你不关心类型的话）设置一个
// 断点，调试器将会捕捉到抛出的错误，你可以往上查找调用栈直到找到消息发出的源头。

// 自定义错误输出:
// 除了仅仅是阅读信息，我们也可以使用glDebugMessageInsert将信息推送到调试输出系统：
// glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_MEDIUM, -1, "error message here");
// 总而言之，调试输出（如果你能使用它）对与快速捕捉错误是非常有用的，完全值得你花一点时间来配置，它能够省下你非常多的开发时间。

// 调试着色器输出:
// 对于GLSL来说，我们不能访问像是glGetError这样的函数，也不能通过步进的方式运行着色器代码。如果你得到一个黑屏或者完全错误的视觉效果，通常想要知道着色器代码是否有误会
// 非常困难。是的，我们是有编译错误报告来报告语法错误，但是捕捉语义错误又是一大难题。
// 一个经常使用的技巧就是将着色器程序中所有相关的变量直接发送到片段着色器的输出通道，以评估它们。通过直接输出着色器变量到输出颜色通道，我们通常可以通过观察视觉结果来获取
// 有用的信息。比如说，如果我们想要检查一个模型的法向量是否正确，我们可以把它们（可以是变换过的也可以是没有变换过的）从顶点着色器传递到片段着色器中，在片段着色器中我们会
// 用以下这种方式输出法向量：
// #version 330 core
// out vec4 FragColor;
// in vec3 Normal;
// [...]
// void main()
// {
//     [...]
//     FragColor.rgb = Normal;
//     FragColor.a = 1.0f;
// }
// 通过输出一个（非颜色）变量到这样子的输出颜色通道中，我们可以快速审查变量是否显示着正确的值。举例来说，如果最后的视觉效果完全是黑色的，则很清楚表明法向量没有正确地传递至
// 着色器中。当它们都显示出来的时候，检查它们（大概）正确与否就会变得非常简单。

// OpenGL GLSL参考编译器:
// 每一个驱动都有它自己的怪癖。比如说NVIDIA驱动会更宽容一点，通常会忽略一些限制或者规范，而ATI/AMD驱动则通常会严格执行OpenGL规范。问题是在一台机器上的着色器到另一台机器
// 上可能就由于驱动差异不能正常工作了。
// 不同GPU供应商之间的细微差别，但如果你想要保证你的着色器代码在所有的机器上都能运行，你可以直接对着官方的标准使用OpenGL的GLSL参考编译器（Reference Compiler）来检查。
// 有了这个GLSL语言校验器，你可以很方便的检查你的着色器代码，只需要把着色器文件作为程序的第一个参数即可。注意GLSL语言校验器是通过下列固定的后缀名来决定着色器的类型的：
// .vert：顶点着色器(Vertex Shader)
// .frag：片段着色器(Fragment Shader)
// .geom：几何着色器(Geometry Shader)
// .tesc：细分控制着色器(Tessellation Control Shader)
// .tese：细分计算着色器(Tessellation Evaluation Shader)
// .comp：计算着色器(Compute Shader)
// 它不会显示AMD，NVidia，以及Intel的GLSL编译器之间的细微差别，也不能保证你的着色器完全没有Bug，但它至少能够帮你对着直接的GLSL规范进行检查。

// 帧缓冲输出:
// 你的调试工具箱中另外一个技巧就是在OpenGL程序中一块特定区域显示帧缓冲的内容。你可能会比较频繁地使用帧缓冲，但由于帧缓冲的魔法通常在幕后进行，有时候想要知道出什么问题会非常
// 困难。在你的程序中显示帧缓冲的内容是一个很有用的技巧，帮助你快速检查错误。
// 注意，这里讨论的帧缓冲显示内容（附件）仅能在纹理附件上使用，而不能应用于渲染缓冲对象。
// 通过使用一个非常简单，只显示纹理的着色器，我们可以写一个助手函数快速在屏幕右上角显示任何纹理。
// 顶点着色器
// #version 330 core
// layout (location = 0) in vec2 position;
// layout (location = 1) in vec2 texCoords;
// out vec2 TexCoords;
// void main()
// {
//     gl_Position = vec4(position, 0.0f, 1.0f);
//     TexCoords = texCoords;
// }
// 片段着色器
// #version 330 core
// out vec4 FragColor;
// in  vec2 TexCoords;
// uniform sampler2D fboAttachment;
// void main()
// {
//     FragColor = texture(fboAttachment, TexCoords);
// }
// 代码
// void DisplayFramebufferTexture(GLuint textureID)
// {
//     if(!notInitialized)
//     {
//         // 在屏幕右上角，使用NDC顶点坐标初始化着色器和VAO
//         [...]
//     }
//     glActiveTexture(GL_TEXTURE0);   
//     glUseProgram(shaderDisplayFBOOutput);
//         glBindTexture(GL_TEXTURE_2D, textureID);
//         glBindVertexArray(vaoDebugTexturedRect);
//             glDrawArrays(GL_TRIANGLES, 0, 6);
//         glBindVertexArray(0);
//     glUseProgram(0);
// }
// int main()
// {
//     [...]
//     while (!glfwWindowShouldClose(window))
//     {
//         [...]
//         DisplayFramebufferTexture(fboAttachment0);
//         glfwSwapBuffers(window);
//     }
// }

// 外部调试软件:
// gDebugger是一个非常易用的跨平台OpenGL程序调试工具。gDebugger会在你运行的OpenGL程序边上，提供OpenGL状态的详细概况。可以随时暂停程序来检查当前状态，纹理内容以及缓冲使用。
// RenderDoc是另外一个很棒的（完全开源的）独立调试工具。和gDebugger类似，你只需要设置捕捉的程序以及工作目录就行了。你的程序会正常运行，当你想要检查一个特定的帧的时候，
// 你只需要让RenderDoc在程序当前状态下捕捉一个或多个帧即可。在捕捉的帧当中，你可以观察管线状态，所有OpenGL指令，缓冲储存，以及使用的纹理。
// CodeXL是由AMD开发的一款GPU调试工具，它有独立版本也有Visual Studio插件版本。CodeXL可以给你非常多的信息，对于图形程序的性能测试也非常有用。CodeXL在NVidia与Intel
// 的显卡上也能运行，不过会不支持OpenCL调试。
// NVIDIA流行的Nsight GPU调试工具并不是一个独立程序，而是一个Visual Studio IDE或者Eclipse IDE的插件。Nsight插件对图形开发者来说非常容易使用，因为它给出了GPU用量，
// 逐帧GPNsight是一款非常有用的工具，在我看来比上述其它工具都有更好的表现，但它仍有一个非常重要的缺点，它只能在NVIDIA的显卡上工作。如果你正在使用一款NVIDIA的显卡（并且
// 使用Visual Studio），Nsight是非常值得一试的。
