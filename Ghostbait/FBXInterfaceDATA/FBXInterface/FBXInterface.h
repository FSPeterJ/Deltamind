#pragma once
#ifdef FBXINTERFACE_EXPORTS
#define FBXLIBRARY_API __declspec(dllexport)
#else
#define FBXLIBRARY_API __declspec(dllimport)
#endif

extern "C" FBXLIBRARY_API int get_scene_poly_count(const char* fbx_file_path);

extern "C" FBXLIBRARY_API int get_mesh_from_scene(const char* fbx_file_path, const char* output_file_path, const char* name = nullptr);

extern "C" FBXLIBRARY_API int get_material_from_scene(const char* fbx_file_path, const char* output_file_path, const char* name = nullptr);

extern "C" FBXLIBRARY_API int get_animdata_from_scene(const char* fbx_file_path, const char* output_file_path);

extern "C" FBXLIBRARY_API int get_clusterdata_from_scene(const char* fbx_file_path, const char* output_file_path);

extern "C" FBXLIBRARY_API int get_bindpose_from_scene(const char* fbx_file_path, const char* output_file_path);

extern "C" FBXLIBRARY_API int write_scene_to_file(const char* fbx_file_path, const char* output_file_path);