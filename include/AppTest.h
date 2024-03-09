#pragma once
#include"Engine.h"

#include"Drawer.h"
#include"ground.h"
#include"cloth_sheet.h"

#include"ray.h"


using namespace std;
class AppTest :public Engine {
public:
	AppTest() {}
	~AppTest() {}
	void before_renderLoop() {
		ground = Ground(1, -0.8);
		ground.setupVAO();
		pureColorShader = Shader(
			"shaders/pure_color.vert",
			"shaders/pure_color.frag"
		);
		cloth = Cloth(20);

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
		cloth.Draw();

		if (renderContex.select_mode && !renderContex.mode_view) {

			Ray ray = renderContex.getRay(view, projection);

			points.push_back(cloth.points[cloth.indice_of_vertex_selected_by_ray(ray)]);

			cout << cloth.indice_of_vertex_selected_by_ray(ray) << endl;


			renderContex.select_mode = false;
		}
		cout << points.size() << endl;
		pureColorShader.setVec3("aColor", 0.0f, 0.0f, 1.0f);
		for (auto point : points) {
			drawer.Draw(point, 10);
			
		}
		//drawer.Draw(points, 10);

	}

public:
	vector<Eigen::Vector3f> points;
	Shader pureColorShader;
	Drawer drawer;
	Ground ground;
	Cloth cloth;

};

