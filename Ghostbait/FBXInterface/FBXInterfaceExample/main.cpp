#include <iostream>
#include "../FBXInterface/FBXInterface.h"

int main()
{
	
	//std::cout << get_scene_poly_count("ViveController.fbx") << '\n';
	std::cout << get_mesh_from_scene("Assets/skyball.fbx", "Skyball.mesh") << '\n';
	//std::cout << get_material_from_scene("Assets/Teddy_run.fbx", "temp.bin");
	//std::cout << get_animdata_from_scene("Assets/Teddy_Run.fbx", "temp.bin") << '\n';
	//std::cout << get_clusterdata_from_scene("ViveController.fbx", "temp.bin") << '\n';
	//std::cout << get_bindpose_from_scene("ViveController.fbx", "temp.bin") << '\n';
	//std::cout << write_scene_to_file("TestScene.fbx", "d.temp");
	system("pause");
	return 0;
}
