#include"App_simluateCloth_quadMesh_massSpring.h"
#include"App_simluateCloth_triMesh_massSpring.h"
#include"AppTest.h"


int main()
{
	Engine* app;
	//app = new AppTest();
	app = new AppClothQuadMassSpring();
	//app = new App_simluateCloth_triMesh_massSpring();

	//app = new Engine();
	
	
	app->launch();
	

	return 0;
}

