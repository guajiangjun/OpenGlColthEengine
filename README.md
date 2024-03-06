其实按照以下方式也实现了线框显示+背面剔除（也就是只显示正面的线框）
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
        
        glDisable(GL_CULL_FACE);
但效果总是有些差强人意，其原因在于这种方式是通过三角面片的顺时针或者逆时针来确定的
这种现象在观察 torus 时尤为明显，当有前后两个面都是顺时针时，那么就会都显示，而并没有体现出正确的遮挡关系
# openGL_Project
