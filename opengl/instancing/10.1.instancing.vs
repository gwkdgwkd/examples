#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset; // 将偏移量uniform数组设置为一个实例化数组。我们需要在顶点着色器中再添加一个顶点属性：

out vec3 fColor;

void main()
{
    fColor = aColor;
    // 我们不再使用gl_InstanceID，现在不需要索引一个uniform数组就能够直接使用offset属性了。
    // 因为实例化数组和position与color变量一样，都是顶点属性，我们还需要将它的内容存在顶点缓冲对象中，并且配置它的属性指针。
    gl_Position = vec4(aPos + aOffset, 0.0, 1.0);

    // 为了更有趣一点，我们也可以使用gl_InstanceID，从右上到左下逐渐缩小四边形：
    // vec2 pos = aPos * (gl_InstanceID / 100.0);
    // gl_Position = vec4(pos + aOffset, 0.0, 1.0);
}