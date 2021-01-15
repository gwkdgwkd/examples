#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

    // build and compile shaders
    // -------------------------
    Shader shader("10.1.instancing.vs", "10.1.instancing.fs");

    // generate a list of 100 quad locations/translation-vectors
    // ---------------------------------------------------------
    glm::vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    // store instance data in an array buffer
    // --------------------------------------
    // translations数组存到一个新的缓冲对象中：
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // 每个四边形由2个三角形所组成，一共有6个顶点。每个顶点包含一个2D的标准化设备坐标位置向量和一个颜色向量。 
    // 下面就是这个例子使用的顶点数据，为了大量填充屏幕，每个三角形都很小：
    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    // also set instance data
    // 设置它的顶点属性指针，并启用顶点属性：
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 这个函数告诉了OpenGL该什么时候更新顶点属性的内容至新一组数据。它的第一个参数是需要的顶点属性，第二个参数是属性除数(Attribute Divisor)。
    // 默认情况下，属性除数是0，告诉OpenGL我们需要在顶点着色器的每次迭代时更新顶点属性。
    // 将它设置为1时，我们告诉OpenGL我们希望在渲染一个新实例的时候更新顶点属性。
    // 而设置为2时，我们希望每2个实例更新一次属性，以此类推。
    // 我们将属性除数设置为1，是在告诉OpenGL，处于位置值2的顶点属性是一个实例化数组。
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw 100 instanced quads
        shader.use();
        glBindVertexArray(quadVAO);
        // glDrawArraysInstanced的参数和glDrawArrays完全一样，除了最后多了个参数用来设置需要绘制的实例数量。
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100); // 100 triangles of 6 vertices each
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// 实例化:
// 假设你有一个绘制了很多模型的场景，而大部分的模型包含的是同一组顶点数据，只不过进行的是不同的世界空间变换。想象一个充满草的场景：每根草都是一个包含几个三角形的小模型。
// 可能会需要绘制很多根草，最终在每帧中可能会需要渲染上千或者上万根草。因为每一根草仅仅是由几个三角形构成，渲染几乎是瞬间完成的，但上千个渲染函数调用却会极大地影响性能。
// 与绘制顶点本身相比，使用glDrawArrays或glDrawElements函数告诉GPU去绘制你的顶点数据会消耗更多的性能，因为OpenGL在绘制顶点数据之前需要做很多准备工作（比如告诉GPU
// 该从哪个缓冲读取数据，从哪寻找顶点属性，而且这些都是在相对缓慢的CPU到GPU总线(CPU to GPU Bus)上进行的）。所以，即便渲染顶点非常快，命令GPU去渲染却未必。
// 如果我们能够将数据一次性发送给GPU，然后使用一个绘制函数让OpenGL利用这些数据绘制多个物体，就会更方便了。这就是实例化(Instancing)。
// 实例化这项技术能够让我们使用一个渲染调用来绘制多个物体，来节省每次绘制物体时CPU -> GPU的通信，它只需要一次即可。如果想使用实例化渲染，我们只需要将glDrawArrays和
// glDrawElements的渲染调用分别改为glDrawArraysInstanced和glDrawElementsInstanced就可以了。这些渲染函数的实例化版本需要一个额外的参数，叫做实例数量
// (Instance Count)，它能够设置我们需要渲染的实例个数。这样我们只需要将必须的数据发送到GPU一次，然后使用一次函数调用告诉GPU它应该如何绘制这些实例。GPU将会直接渲染这
// 些实例，而不用不断地与CPU进行通信
// 这个函数本身并没有什么用。渲染同一个物体一千次对我们并没有什么用处，每个物体都是完全相同的，而且还在同一个位置。我们只能看见一个物体！处于这个原因，GLSL在顶点着色器中
// 嵌入了另一个内建变量，gl_InstanceID。在使用实例化渲染调用时，gl_InstanceID会从0开始，在每个实例被渲染时递增1。
// 因为每个实例都有唯一的ID，我们可以建立一个数组，将ID与位置值对应起来，将每个实例放置在世界的不同位置。

// 在合适的环境下，实例化渲染能够大大增加显卡的渲染能力。正是出于这个原因，实例化渲染通常会用于渲染草、植被、粒子，以及上面这样的场景，基本上只要场景中有很多重复的形状，
// 都能够使用实例化渲染来提高性能。

// 实例化数组:
// 使用顶点属性时，顶点着色器的每次运行都会让GLSL获取新一组适用于当前顶点的属性。而当我们将顶点属性定义为一个实例化数组时，顶点着色器就只需要对每个实例，而不是每个顶点，更新
// 顶点属性的内容了。这允许我们对逐顶点的数据使用普通的顶点属性，而对逐实例的数据使用实例化数组。

// 如果不用实例化数组：
// 定义了一个叫做offsets的数组，它包含100个偏移向量。在顶点着色器中，我们会使用gl_InstanceID来索引offsets数组，获取每个实例的偏移向量。如果我们要实例化绘制100个四边形，
// 仅使用这个顶点着色器我们就能得到100个位于不同位置的四边形。当前，我们仍要设置这些偏移位置，我们会在进入渲染循环之前使用一个嵌套for循环计算：
// glm::vec2 translations[100];
// int index = 0;
// float offset = 0.1f;
// for(int y = -10; y < 10; y += 2)
// {
//     for(int x = -10; x < 10; x += 2)
//     {
//         glm::vec2 translation;
//         translation.x = (float)x / 10.0f + offset;
//         translation.y = (float)y / 10.0f + offset;
//         translations[index++] = translation;
//     }
// }
// 还需要将数据转移到顶点着色器的uniform数组中：
// 在这一段代码中，我们将for循环的计数器i转换为一个string，我们可以用它来动态创建位置值的字符串，用于uniform位置值的索引。
// 接下来，我们会对offsets uniform数组中的每一项设置对应的位移向量。
// shader.use();
// for(unsigned int i = 0; i < 100; i++)
// {
//     stringstream ss;
//     string index;
//     ss << i; 
//     index = ss.str(); 
//     shader.setVec2(("offsets[" + index + "]").c_str(), translations[i]);
// }
