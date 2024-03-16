#pragma once
#include"Engine.h"

#include"Drawer.h"
#include"ground.h"
#include"cloth_sheet.h"

#include"ray.h"

#include"mesh_m.h"






class App_simluateCloth_triMesh_massSpring :public Engine {
public:
   
public:
	App_simluateCloth_triMesh_massSpring() {}
	~App_simluateCloth_triMesh_massSpring() {}
	void before_renderLoop() override {
        models_path = "E:/Desktop/models/";
		
        ground = Ground(1, -0.8);
		ground.setupVAO();

        mesh = triMesh(models_path +"bunny/bunny30k_f.obj");
        
        file_dialog.set_current_path(models_path);

        renderContex.simulating = true;
        
      
	}

	void render() override {
        // ground
        defaultPureColorShader.use();
		ground.draw();

        // mesh
        defaultPhongShader.use();
        
        mesh.update_normals();
		mesh.Draw();

        // imgui
        ImGui::Begin("My name is window, ImGUI window");
        showMainMenu();
        ImGui::Text("Hello there adventurer!");
        ImGui::End();

	}
    void open_file() override {
        
        mesh = triMesh(file_dialog.selected_path);

    }
    void simlate_update() override {
        //定义旋转角度(弧度制)
        float angle = 0.01; // 45度

        // 构造旋转矩阵
        Eigen::Matrix3f R;
        R << std::cos(angle), 0, std::sin(angle),
            0, 1, 0,
            -std::sin(angle), 0, std::cos(angle);

        // 对每个顶点进行旋转
        for (int i = 0; i < mesh.V.cols(); ++i) {
            mesh.V.col(i) = R * mesh.V.col(i);
        }
    }

public:

	Drawer drawer;

	Ground ground;	
	triMesh mesh;

    std::string models_path;

    

};

