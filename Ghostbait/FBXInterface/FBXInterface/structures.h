#pragma once
#include <vector>
#include <array>
#include <map>

#define MAX_INFLUENCES 4

struct simple_vert
{
	float pos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float norm[3] = { 0.0f, 0.0f, 0.0f };
	float uv[2] = { 0.0f, 0.0f };
	int indices[4] = { 0,0,0,0 };
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct simple_mesh
{
	unsigned int vertCount = 0;
	unsigned int indexCount = 0;
	std::vector<simple_vert> verts;
	std::vector<int> indices;
};

struct simple_material
{
	const char* diffusePath;
	unsigned int diffusePathLength = 0;
	const char* specPath;
	unsigned int specPathLength = 0;
	const char* emissivePath;
	unsigned int emissivePathLength = 0;
	float diffuseFactor;
	float specFactor;
	float emissiveFactor;
};

struct fbx_joint
{
	FbxNode* node;
	int parent_index;
	const char* name;
};

struct output_joint
{
	float global_xform[16];
	int parent_index;
	const char* name;
	unsigned int nameLen;
};

struct keyframe
{
	double time;
	std::vector<output_joint> joints;
};

struct animClip
{
	double duration;
	std::vector<keyframe> keyframes;
};

struct influence
{
	int index = 0;
	float weight = 0;
};

struct influence_set
{
	std::array<influence, MAX_INFLUENCES> influences;
};

struct prefab
{
	std::string name;
	std::vector<std::array<float, 16>> matrices;
};

struct extracted_scene
{
	std::map<std::string, prefab> prefabs;
};