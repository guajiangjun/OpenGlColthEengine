#pragma once
#include"Engine.h"

#include"Drawer.h"
#include"ground.h"
#include"cloth_sheet.h"

#include"ray.h"

#include"mesh_m.h"

using namespace std;
class App_simluateCloth_triMesh_massSpring :public Engine {
public:
	App_simluateCloth_triMesh_massSpring() {}
	~App_simluateCloth_triMesh_massSpring() {}
	void before_renderLoop() {
		ground = Ground(1, -0.8);
		ground.setupVAO();
		pureColorShader = Shader(
			"shaders/pure_color.vert",
			"shaders/pure_color.frag"
		);
		

		mesh = triMesh("E:/Desktop/models/cloth/inspired_mesh.obj");

		


	}
	void render_inLoop() {
		pureColorShader.use();
		pureColorShader.setMat4("model", model);
		pureColorShader.setMat4("view", view);
		pureColorShader.setMat4("projection", projection);

		// draw ground
		pureColorShader.setVec3("aColor", 0.502, 0.502, 0.502);
		ground.draw();

		// draw cloth
		pureColorShader.setVec3("aColor", 1.0f, 0.5f, 0.2f);
		mesh.Draw();

	}

public:

	Drawer drawer;
	Shader pureColorShader;


	Ground ground;	
	triMesh mesh;

};

