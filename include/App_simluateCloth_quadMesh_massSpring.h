#pragma once
#include"Engine.h"
#include"cloth_sheet.h"

#include"util_simulation.h"

#include"ground.h"
#include <thread>

class AppClothQuadMassSpring : public Engine {
public: // tempt variable for simualting
	Eigen::VectorXf force;
	Eigen::SparseMatrixf stiffness;

	Eigen::MatrixXi E;

	Eigen::VectorXf l0; //original length of all edges in the mesh

	Eigen::VectorXf q;
	Eigen::VectorXf qdot;
	Eigen::SparseMatrixf M;
	Eigen::SparseMatrixf P; //fixed point constraints 
	Eigen::VectorXf x0; //fixed point constraints 

	float m = 1.0f;


	float COR = 0.2;// Coefficient of Restitution
	float dt = 1e-2; //time step

	float k = 1e4;

	float ground_y = -0.9;
	float ground_size = 1.0;



public: // objects and shaders

	Cloth cloth;
	Ground ground;


	Shader pureColorShader;
public:
	AppClothQuadMassSpring() {

	}
	~AppClothQuadMassSpring() {}
	void before_renderLoop() override {



		ground = Ground(ground_size, ground_y);
		ground.setupVAO();

		pureColorShader = Shader(
			"shaders/pure_color.vert", 
			"shaders/pure_color.frag"
		);

		cloth = Cloth(30);

		q.resize(cloth.num_nodes * 3);
		for (int i = 0;i < q.rows() / 3;i++) {
			q.segment<3>(3 * i) = cloth.points[i];
		}

		E = cloth.E;

		l0.resize(E.rows());
		for (int i = 0;i < l0.rows();i++) {
			l0(i) = (q.segment<3>(3 * E(i, 0)) - q.segment<3>(3 * E(i, 1))).norm();
		}

		qdot.resize(q.rows());
		qdot.setZero();

		// mass_matrix_particles(M, q, m);
		M.resize(q.rows(), q.rows());
		M.setIdentity();
		M *= m;

		// setup constraint matrix
		fixed_point_constraints(P, q.rows(), cloth.fixed_point_indices); // produce matrix P

		x0 = q - P.transpose() * P * q; //vector x0 contains position of all fixed nodes, zero for everything else

		// correct M, q and qdot so they are the right size
		q = P * q;
		qdot = P * qdot;
		M = P * M * P.transpose();

		//std::thread simulation_thread(simulate);
		std::thread simulation_thread(&AppClothQuadMassSpring::simulate, this);
		simulation_thread.detach();

	


	};

	void render() override {

		pureColorShader.use();

		// setup shader
		pureColorShader.setMat4("model", model);
		pureColorShader.setMat4("view", view);
		pureColorShader.setMat4("projection", projection);

		// draw ground
		pureColorShader.setVec3("aColor", 0.502, 0.502, 0.502);
		ground.draw();

		// update cloth vertices
		cloth.updateVertices(P.transpose() * q + x0);

		// draw cloth
		pureColorShader.setVec3("aColor", 1.0f, 0.5f, 0.2f);
		cloth.Draw(3.0f);

		ImGui::Begin("My name is window, ImGUI window");
		ImGui::Text("Hello there adventurer!");
		ImGui::End();
	};

	void simulate_restart_handle() override {
		qdot.setZero();

		Eigen::VectorXf qq;
		qq.resize(cloth.points.size() * 3);
		for (int i = 0;i < qq.rows() / 3;i++) {
			qq.segment<3>(3 * i) = cloth.initial_points[i];
		}
		q = P * qq;

	}
	
	// using linear implicit euler method
	void simlate_update() override {

		// compute forces
		assemble_forces(force, P.transpose() * q + x0, E, l0, m, k);

		force = P * force;

		// compute stiffness
		assemble_stiffness(stiffness, P.transpose() * q + x0, E, l0, k);
		stiffness = P * stiffness * P.transpose();

		// compute and update qdot
		Eigen::VectorXf b;
		Eigen::SparseMatrixf A;

		A = (M - dt * dt * stiffness);
		b = M * qdot + dt * force;
		Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
		solver.compute(A);
		qdot = solver.solve(b);

		// compute and update q
		q = q + dt * qdot;

		// collidision detection and handling
		for (int i = 0; i < q.rows(); i += 3) {

			float x = q[i];
			float y = q[i + 1];
			float z = q[i + 2];

			// 检查顶点是否低于地面高度
			if (y < ground_y) {
				// 将顶点位置设置为地面高度
				q[i + 1] = ground_y;

				// 反转速度方向
				qdot[i + 1] = -qdot[i + 1] * COR;
			}
		}
	}
};





