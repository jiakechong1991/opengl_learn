#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
#include <iostream>
 
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
 
// 顶点着色器，GLSL语言
const char *vertexShaderSource = "#version 410 core\n" 
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" 
    "}\0";
// 片元着色器
const char *fragmentShaderSource = "#version 410 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
 
int main()
{
    // 初始化和配置glfw参数
    glfwInit();  // 初始化glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // 设置flfw的参数,告诉他我们使用的是opengl4.1版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 设置 opengl工作在 core profile模式

    // 创建指定宽高的窗口： 宽，高， 窗口名称，NULL，NULL
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window 创建失败" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);  // 将窗口的上下文，设置为当前线程的上下文
    //在window首次初始化 或者 发生变化时，调用该函数，重新设置视口大小
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
 
    // 固定写法：初始化glad(在调用opengl函数之前，必须先初始化glad)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
 
 
    /////////////////////////////////////////////构建 shader program
    //////////////// vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER); // 创建着色(shader)对象, shader类型是GL_VERTEX_SHADER,即顶点shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 把这个着色器源码附加到shader对象。着色器对象，源码字符串数量，源码字符串
    glCompileShader(vertexShader);  // 编译这个shader

    // 检查 shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //////////////// fragment shader：计算像素的颜色
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  //创建fragement shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // 检查 shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ///////////////// link shaders
    int shaderProgram = glCreateProgram(); // shaderProgram[着色器程序对象] 是多个着色器合并之后并最终链接完成的版本
    glAttachShader(shaderProgram, vertexShader); // 将顶点shader对象 附加 shadeProgram
    glAttachShader(shaderProgram, fragmentShader);  // 
    glLinkProgram(shaderProgram);  // link到 shaderProgram上
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog) ;
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // 在把shader对象 link到 shaderProgram后，就可以删除这些shader对象了，因为他们已经被包含在shaderProgram中了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
 
    /////////////////////////////////////////////准备顶点数据
    float vertices[] = {  // opengl的坐标叫做 标准化设备坐标(Normalized Device Coordinates)，这个三维坐标，要在[-1,1]之间，否则不予处理。
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 
 
    unsigned int VBO, VAO;
	//创建VAO对象
    glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//创建VBO对象[顶点缓冲对象]，把顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glGenBuffers(1, &VBO);  //【生成】缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 缓冲【绑定】到GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 顶点数据（cpu内存）复制到缓冲(gpu内存)中
 
	//告诉opengl 如何解释 顶点数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // 顶点数据的解释,并写入到VAO中
    glEnableVertexAttribArray(0);  //启动顶点属性，默认是禁用的
 
    // 解绑VAO
	glBindVertexArray(0); 
	// 解绑VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    

    ///////////////////////////////////// render loop [渲染循环]
    while (!glfwWindowShouldClose(window))  // 检查用户是否 要关闭该窗口
    {
        ///////////////////////// 监控 用户按键，然后 执行对应操作
        processInput(window);
 



        ///////////////////////// render区域
        ////// 使用指定的颜色，清空 window的缓冲区
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // 这是状态设置函数
        // GL_COLOR_BUFFER_BIT：颜色缓冲区，GL_DEPTH_BUFFER_BIT：深度缓冲区 和 GL_STENCIL_BUFFER_BIT：模板缓冲区
        glClear(GL_COLOR_BUFFER_BIT);   // 这属于状态使用函数
 
        // draw our first triangle
        glUseProgram(shaderProgram); // 激活shaderProgram，怎么画
        glBindVertexArray(VAO); // 画什么
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 三角形绘制模式：参数1：  参数2：使用线来绘制，
        glDrawArrays(GL_TRIANGLES, 0, 3); // 开始画：调用当前激活的shader, 顶点数据 开始绘制



        //////////////////////// 检查并调用事件，交换缓冲
        /*
        这个swap buffer是有说法的，我们一般使用双缓冲(double buffer)
        1. 如果使用单缓冲容易造成屏幕图像闪烁，因为绘图是从左到右，从上到下 逐渐绘制的，而非瞬间绘制的。这时 单缓冲就会不断的绘制到屏幕上
        2. 双缓冲是，前缓冲 保存上一帧绘制好的图像，并显示到屏幕上。然后 渲染指令在后缓冲上绘制
        3. 当所有渲染指令(在后缓冲上)运行完毕，就交换(swap)前后缓冲，这样屏幕上就能立刻显示图像
        */
        glfwSwapBuffers(window);  // 将缓冲区的数据，输出到window上
        glfwPollEvents();  //检查是否有 触发事件：键盘输入，鼠标移动，并调用对应的回调函数
    }
 
    // 程序即将退出，释放资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();  // 释放之前分类的内存资源
    
    return 0;
}
 
//键盘按键回调函数  
void processInput(GLFWwindow *window)
{   
    //检测 esc按键是否被按下， 按下：GLFW_PRESS  未按下：GLFW_RELEASE
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  
        glfwSetWindowShouldClose(window, true);  // 主动关闭window窗口， glfwWindowShouldClose将读到true,
}
 
//调整窗口大小回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // 设置视口(viewport)大小 （默认视口是窗口大小）
    // 视口是渲染的输出区域，定义了屏幕空间
    // 我们可以把视口设定成窗口的一个部分，比如只在第一个象限绘制，或者只绘制窗口的一部分
    // 0，0： 视口左下角， width，height：视口右上角
    glViewport(0, 0, width/2, height/2);  
    // std::cout << "我被调用了" << std::endl;
}
 
