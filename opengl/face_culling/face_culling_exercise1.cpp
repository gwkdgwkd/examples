float vertices[] = {
    // back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right    
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right              
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left                
    // front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right        
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left        
    // left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left       
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    // right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right      
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right          
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
    // bottom face          
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left        
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    // top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right                 
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left  
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // top-left              
};

/* Also make sure to add a call to OpenGL to specify that triangles defined by a clockwise ordering 
   are now 'front-facing' triangles so the cube is rendered as normal:
   glFrontFace(GL_CW);
*/

// 一个3D立方体，从任意方向最多能同时看到几个面？ 3个。如果我们能够以某种方式丢弃这几个看不见的面，我们能省下超过50%的片段着色器执行数！
// 如果我们想象任何一个闭合形状，它的每一个面都有两侧，每一侧要么面向用户，要么背对用户。如果我们能够只绘制面向观察者的面呢？
// 这正是面剔除(Face Culling)所做的。OpenGL能够检查所有面向(Front Facing)观察者的面，并渲染它们，而丢弃那些背向(Back Facing)的面，节省我们很多的片段着
// 色器调用（它们的开销很大！）。但我们仍要告诉OpenGL哪些面是正向面(Front Face)，哪些面是背向面(Back Face)。
// OpenGL使用了一个很聪明的技巧，分析顶点数据的环绕顺序(Winding Order)。

// 环绕顺序:
// 当我们定义一组三角形顶点时，我们会以特定的环绕顺序来定义它们，可能是顺时针(Clockwise)的，也可能是逆时针(Counter-clockwise)的。
// 每组组成三角形图元的三个顶点就包含了一个环绕顺序。OpenGL在渲染图元的时候将使用这个信息来决定一个三角形是一个正向三角形还是背向三角形。默认情况下，逆时针顶点
// 所定义的三角形将会被处理为正向三角形。
// 当你定义顶点顺序的时候，你应该想象对应的三角形是面向你的，所以你定义的三角形从正面看去应该是逆时针的。这样定义顶点很棒的一点是，实际的环绕顺序是在光栅化阶段进
// 行的，也就是顶点着色器运行之后。这些顶点就是从观察者视角所见的了。
// 观察者所面向的所有三角形顶点就是我们所指定的正确环绕顺序了，而立方体另一面的三角形顶点则是以相反的环绕顺序所渲染的。这样的结果就是，我们所面向的三角形将会是正
// 向三角形，而背面的三角形则是背向三角形。
// 在顶点数据中，我们定义的是两个逆时针顺序的三角形。然而，从观察者的方面看，后面的三角形是顺时针的，如果我们仍以1、2、3的顺序以观察者当面的视野看的话。即使我们
// 以逆时针顺序定义后面的三角形，它现在还是变为顺时针。它正是我们打算剔除（丢弃）的不可见的面！

// 面剔除:
// OpenGL的面剔除选项了，它默认是禁用状态的。
// 要想启用面剔除，我们只需要启用OpenGL的GL_CULL_FACE选项：glEnable(GL_CULL_FACE);
// 所有背向面都将被丢弃（尝试飞进立方体内部，看看所有的内面是不是都被丢弃了）。目前我们在渲染片段的时候能够节省50%以上的性能，但注意这只对像立方体这样的封闭形状有效。
// 当我们想要绘制草时，我们必须要再次禁用面剔除，因为它们的正向面和背向面都应该是可见的。
// OpenGL允许我们改变需要剔除的面的类型。如果我们只想剔除正向面而不是背向面会怎么样？我们可以调用glCullFace来定义这一行为：glCullFace(GL_FRONT);
// glCullFace函数有三个可用的选项：
// GL_BACK：只剔除背向面。(glCullFace的初始值是GL_BACK)
// GL_FRONT：只剔除正向面。
// GL_FRONT_AND_BACK：剔除正向面和背向面。
// 除了需要剔除的面之外，可以通过调用glFrontFace，告诉OpenGL我们希望将顺时针的面（而不是逆时针的面）定义为正向面：
// glFrontFace(GL_CCW); 默认值是GL_CCW，它代表的是逆时针的环绕顺序，另一个选项是GL_CW，它（显然）代表的是顺时针顺序。
// 告诉OpenGL现在顺时针顺序代表的是正向面，这样的结果是只有背向面被渲染了。
// glEnable(GL_CULL_FACE);
// glCullFace(GL_BACK);
// glFrontFace(GL_CW);
// 可以仍使用默认的逆时针环绕顺序，但剔除正向面，来达到相同的效果：
// glEnable(GL_CULL_FACE);
// glCullFace(GL_FRONT);
// 面剔除是一个提高OpenGL程序性能的很棒的工具。但你需要记住哪些物体能够从面剔除中获益，而哪些物体不应该被剔除。