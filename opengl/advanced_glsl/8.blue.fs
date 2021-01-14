#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}

// 片段着色器变量:
// 在片段着色器中，我们也能访问到一些有趣的变量。GLSL提供给我们两个有趣的输入变量：gl_FragCoord和gl_FrontFacing。

// gl_FragCoord:
// 在讨论深度测试的时候，我们已经见过gl_FragCoord很多次了，因为gl_FragCoord的z分量等于对应片段的深度值。然而，我们也能使用它的x和y分量来实现一些有趣的效果。
// gl_FragCoord的x和y分量是片段的窗口空间(Window-space)坐标，其原点为窗口的左下角。我们已经使用glViewport设定了一个800x600的窗口了，所以片段窗口空间坐标
// 的x分量将在0到800之间，y分量在0到600之间。
// 通过利用片段着色器，我们可以根据片段的窗口坐标，计算出不同的颜色。gl_FragCoord的一个常见用处是用于对比不同片段计算的视觉输出效果，这在技术演示中可以经常看到。
// 比如说，我们能够将屏幕分成两部分，在窗口的左侧渲染一种输出，在窗口的右侧渲染另一种输出。下面这个例子片段着色器会根据窗口坐标输出不同的颜色：
// if(gl_FragCoord.x < 400)
//     FragColor = vec4(1.0, 0.0, 0.0, 1.0);
// else
//     FragColor = vec4(0.0, 1.0, 0.0, 1.0); 

// gl_FrontFacing:
// 在面剔除教程中，我们提到OpenGL能够根据顶点的环绕顺序来决定一个面是正向还是背向面。如果我们不（启用GL_FACE_CULL来）使用面剔除，那么gl_FrontFacing将会告诉
// 我们当前片段是属于正向面的一部分还是背向面的一部分。举例来说，我们能够对正向面计算出不同的颜色。
// if(gl_FrontFacing)
//     FragColor = texture(frontTexture, TexCoords);
// else
//     FragColor = texture(backTexture, TexCoords);
// 注意，如果你开启了面剔除，你就看不到箱子内部的面了，所以现在再使用gl_FrontFacing就没有意义了。

// gl_FragDepth:
// 输入变量gl_FragCoord能让我们读取当前片段的窗口空间坐标，并获取它的深度值，但是它是一个只读(Read-only)变量。我们不能修改片段的窗口空间坐标，但实际上修改片段
// 的深度值还是可能的。GLSL提供给我们一个叫做gl_FragDepth的输出变量，我们可以使用它来在着色器内设置片段的深度值。
// 要想设置深度值，我们直接写入一个0.0到1.0之间的float值到输出变量就可以了：gl_FragDepth = 0.0; // 这个片段现在的深度值为 0.0
// 如果着色器没有写入值到gl_FragDepth，它会自动取用gl_FragCoord.z的值。
// 然而，由我们自己设置深度值有一个很大的缺点，只要我们在片段着色器中对gl_FragDepth进行写入，OpenGL就会（像深度测试小节中讨论的那样）禁用所有的提前深度测试
// (Early Depth Testing)。它被禁用的原因是，OpenGL无法在片段着色器运行之前得知片段将拥有的深度值，因为片段着色器可能会完全修改这个深度值。
// 在写入gl_FragDepth时，你就需要考虑到它所带来的性能影响。然而，从OpenGL 4.2起，我们仍可以对两者进行一定的调和，在片段着色器的顶部使用深度条件(Depth Condition)
// 重新声明gl_FragDepth变量：layout (depth_<condition>) out float gl_FragDepth;
// 下面这个例子中，我们对片段的深度值进行了递增，但仍然也保留了一些提前深度测试：
// #version 420 core // 注意GLSL的版本！
// out vec4 FragColor;
// layout (depth_greater) out float gl_FragDepth;
// void main()
// {             
//     FragColor = vec4(1.0);
//     gl_FragDepth = gl_FragCoord.z + 0.1;
// }  

// 块接口：
// 到目前为止，每当我们希望从顶点着色器向片段着色器发送数据时，我们都声明了几个对应的输入/输出变量。将它们一个一个声明是着色器间发送数据最简单的方式了，但当程序变得更大
// 时，你希望发送的可能就不只是几个变量了，它还可能包括数组和结构体。
// 为了帮助我们管理这些变量，GLSL为我们提供了一个叫做接口块(Interface Block)的东西，来方便我们组合这些变量。接口块的声明和struct的声明有点相像，不同的是，现在根据它
// 是一个输入还是输出块(Block)，使用in或out关键字来定义的。
// out VS_OUT
// {
//     vec2 TexCoords;
// } vs_out;
// vs_out.TexCoords = aTexCoords;
// in VS_OUT
// {
//     vec2 TexCoords;
// } fs_in;
// FragColor = texture(texture, fs_in.TexCoords);
// 只要两个接口块的名字一样，它们对应的输入和输出将会匹配起来。这是帮助你管理代码的又一个有用特性，它在几何着色器这样穿插特定着色器阶段的场景下会很有用。

// Uniform缓冲对象:
// 当使用多于一个的着色器时，尽管大部分的uniform变量都是相同的，我们还是需要不断地设置它们，所以为什么要这么麻烦地重复设置它们呢？
// OpenGL为我们提供了一个叫做Uniform缓冲对象(Uniform Buffer Object)的工具，它允许我们定义一系列在多个着色器中相同的全局Uniform变量。当使用Uniform缓冲对象的时候，
// 我们只需要设置相关的uniform一次。当然，我们仍需要手动设置每个着色器中不同的uniform。并且创建和配置Uniform缓冲对象会有一点繁琐。
// 因为Uniform缓冲对象仍是一个缓冲，我们可以使用glGenBuffers来创建它，将它绑定到GL_UNIFORM_BUFFER缓冲目标，并将所有相关的uniform数据存入缓冲。在Uniform缓冲对象中
// 储存数据是有一些规则的