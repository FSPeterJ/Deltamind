#pragma once
#include "fbxsdk.h"
#include <fstream>
#include "structures.h"

FbxManager* create_and_import(const char* fbx_file_path, FbxScene*& lScene);
simple_mesh make_friendly_mesh(FbxMesh * mesh, std::vector<influence_set>& influences);
simple_material make_friendly_material(FbxSurfaceMaterial * mat);
std::vector<fbx_joint> generateNodeList(FbxPose* pose);
std::vector<output_joint> convertFbxJointsToOutput(std::vector<fbx_joint>& joints);
std::vector<output_joint> evaluateTransformsAtTime(std::vector<fbx_joint>& joints, FbxTime time);
std::vector<influence_set> extractInfluences(FbxPose* pose, int count, std::vector<fbx_joint>& joints);
void createAnimationClip(animClip& clip, FbxAnimStack* animStack, std::vector<fbx_joint>& joints);
void export_animClip_to_binary(const char* output_file_path, animClip& clip);
void export_mesh_to_binary(const char* output_file_path, simple_mesh mesh);
void export_material_to_binary(const char* output_file_path, simple_material mat);
void export_bindpose_to_binary(const char* output_file_path, std::vector<output_joint>& joints);
void export_clusterdata_to_binary(const char* output_file_path, simple_mesh mesh);
void export_scene_to_binary(const char* output_file_path, extracted_scene& scene);