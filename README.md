��ʵ�������·�ʽҲʵ�����߿���ʾ+�����޳���Ҳ����ֻ��ʾ������߿�
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
        
        glDisable(GL_CULL_FACE);
��Ч��������Щ��ǿ���⣬��ԭ���������ַ�ʽ��ͨ��������Ƭ��˳ʱ�������ʱ����ȷ����
���������ڹ۲� torus ʱ��Ϊ���ԣ�����ǰ�������涼��˳ʱ��ʱ����ô�ͻᶼ��ʾ������û�����ֳ���ȷ���ڵ���ϵ
# openGL_Project
