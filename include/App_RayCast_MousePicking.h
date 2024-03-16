#pragma once
#include"Engine.h"

#include"Drawer.h"
#include"ground.h"
#include"cloth_sheet.h"

#include"ray.h"


using namespace std;
class App_RayCast_MousePicking :public Engine {
public:
	App_RayCast_MousePicking() {}
	~App_RayCast_MousePicking() {}
	void before_renderLoop() override {
		ground = Ground(1, -0.8);
		ground.setupVAO();
		cloth = Cloth(20);

	}
	void render() override {


		// draw ground
		defaultPureColorShader.use();
		defaultPureColorShader.setVec3("aColor", 0.502, 0.502, 0.502);
		ground.draw();

		// draw cloth
		defaultPureColorShader.setVec3("aColor", 1.0f, 0.5f, 0.2f);
		cloth.Draw();

		if (renderContex.select_mode && !renderContex.mode_view) {

			Ray ray = renderContex.getRay(view, projection);

			points.push_back(cloth.points[cloth.indice_of_vertex_selected_by_ray(ray)]);

			cout << cloth.indice_of_vertex_selected_by_ray(ray) << endl;


			renderContex.select_mode = false;
		}
		defaultPureColorShader.setVec3("aColor", 0.0f, 0.0f, 1.0f);
		for (auto point : points) {
			drawer.Draw(point, 10);
			
		}
	}

public:
	vector<Eigen::Vector3f> points;
	
	Drawer drawer;
	Ground ground;
	Cloth cloth;

};

