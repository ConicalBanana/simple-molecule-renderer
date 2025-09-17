while (!glfwWindowShouldClose(window))
{
    // 清除缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 第一遍：渲染轮廓
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT); // 只渲染背面（轮廓）
    
    outlineShader.use();
    // 设置轮廓着色器的uniform
    outlineShader.setMat4("model", model);
    outlineShader.setMat4("view", view);
    outlineShader.setMat4("projection", projection);
    outlineShader.setFloat("outlineSize", 0.05f); // 轮廓大小
    
    // 绘制模型
    model.Draw();
    
    // 第二遍：渲染三渲二效果
    glCullFace(GL_BACK); // 只渲染正面
    
    toonShader.use();
    // 设置三渲二着色器的uniform
    toonShader.setMat4("model", model);
    toonShader.setMat4("view", view);
    toonShader.setMat4("projection", projection);
    toonShader.setVec3("lightPos", lightPos);
    toonShader.setVec3("viewPos", camera.Position);
    toonShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    toonShader.setVec3("objectColor", 0.8f, 0.3f, 0.3f); // 物体颜色
    
    // 绘制模型
    model.Draw();
    
    // 交换缓冲区并轮询事件
    glfwSwapBuffers(window);
    glfwPollEvents();
}