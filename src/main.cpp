#include"App_simluateCloth_quadMesh_massSpring.h"
#include"App_simluateCloth_triMesh_massSpring.h"
#include"App_RayCast_MousePicking.h"






int main()
{
	Engine* app;
	
	//app = new Engine();
	//app = new App_RayCast_MousePicking();
	//app = new AppClothQuadMassSpring();
	app = new App_simluateCloth_triMesh_massSpring();

	
  


	app->launch();



	

	return 0;
}

