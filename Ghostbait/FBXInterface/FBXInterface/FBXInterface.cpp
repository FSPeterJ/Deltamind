#include "utility.h"
#include "FBXInterface.h"

int get_scene_poly_count(const char* fbx_file_path) {
	int result = -1;
	// Scene pointer, set by call to create_and_import
	FbxScene* scene = nullptr;
	// Create the FbxManager and import the scene from file
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);
	// Check if manager creation failed
	if(sdk_manager == nullptr)
		return result;
	//If the scene was imported...
	if(scene != nullptr) {
		//No errors to report, so start polygon count at 0
		result = 0;
		// Get the count of geometry objects in the scene
		int geo_count = scene->GetGeometryCount();
		for(int i = 0; i < geo_count; ++i) {
			//Get geometry number 'i'
			FbxGeometry* geo = scene->GetGeometry(i);
			// If it's not a mesh, skip it
			// Geometries might be some other type like nurbs
			if(geo->GetAttributeType() != FbxNodeAttribute::eMesh)
				continue;
			// Found a mesh, add its polygon count to the result
			FbxMesh* mesh = (FbxMesh*)geo;
			result += mesh->GetPolygonCount();
		}
	}
	//Destroy the manager
	sdk_manager->Destroy();
	//Return the polygon count for the scene
	return result;
}

int get_mesh_from_scene(const char * fbx_file_path, const char * output_file_path, const char * name) {
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if(!sdk_manager || !scene) return -1;

	int geoCount = scene->GetGeometryCount();
	for(int i = 0; i < geoCount; ++i) {
		FbxGeometry* geo = scene->GetGeometry(i);

		if(geo->GetAttributeType() != FbxNodeAttribute::eMesh)
			continue;

		FbxMesh* mesh = (FbxMesh*)geo;
		if(name != nullptr) {
			if(strcmp(name, mesh->GetName())) {
				FbxPose* thePose = nullptr;
				for(int i = 0; i < scene->GetPoseCount(); ++i) {
					thePose = scene->GetPose(i);
					if(thePose != nullptr)
						break;
				}
				std::vector<fbx_joint> joints = generateNodeList(thePose);
				std::vector<influence_set> influences = extractInfluences(thePose, ((FbxMesh*)geo)->GetControlPointsCount(), joints);
				simple_mesh gotIt = make_friendly_mesh(mesh, influences);
				export_clusterdata_to_binary(output_file_path, gotIt);
				break;
			}
		}
		else {
			FbxPose* thePose = nullptr;;
			for(int i = 0; i < scene->GetPoseCount(); ++i) {
				thePose = scene->GetPose(i);
				if(thePose != nullptr)
					break;
			}
			std::vector<influence_set> influences;
			if(thePose != nullptr) {
				std::vector<fbx_joint> joints = generateNodeList(thePose);
				influences = extractInfluences(thePose, ((FbxMesh*)geo)->GetControlPointsCount(), joints);
			}
			simple_mesh gotIt = make_friendly_mesh(mesh, influences);
			export_clusterdata_to_binary(output_file_path, gotIt);
			break;
		}
	}
	sdk_manager->Destroy();
	return 0;
}

int get_material_from_scene(const char * fbx_file_path, const char * output_file_path, const char * name) {
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if(!sdk_manager || !scene) return -1;

	int geoCount = scene->GetGeometryCount();
	for(int i = 0; i < geoCount; ++i) {
		FbxGeometry* geo = scene->GetGeometry(i);

		if(geo->GetAttributeType() != FbxNodeAttribute::eMesh)
			continue;

		FbxNode* node = geo->GetNode(0);
		if(node) {
			if(name != nullptr) {
				FbxMesh* mesh = (FbxMesh*)geo;
				if(strcmp(name, mesh->GetName())) {
					if(!node->GetMaterial(0)) {
						sdk_manager->Destroy();
						return -1;
					}
					simple_material gotIt = make_friendly_material(node->GetMaterial(0));
					if(gotIt.diffusePath[0] == '\0' && gotIt.specPath[0] == '\0' && gotIt.emissivePath[0] == '\0')
						return -1;
					export_material_to_binary(output_file_path, gotIt);
					break;
				}
			}
			else {
				if(!node->GetMaterial(0)) {
					sdk_manager->Destroy();
					return -1;
				}
				simple_material gotIt = make_friendly_material(node->GetMaterial(0));
				if(gotIt.diffusePath[0] == '\0' && gotIt.specPath[0] == '\0' && gotIt.emissivePath[0] == '\0')
					return -1;
				export_material_to_binary(output_file_path, gotIt);
				break;
			}
		}
	}
	sdk_manager->Destroy();
	return 0;
}

int get_animdata_from_scene(const char * fbx_file_path, const char * output_file_path) {
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if(!sdk_manager || !scene || scene->GetPoseCount() <= 0) return -1;
	FbxPose* thePose = scene->GetPose(0);
	std::vector<fbx_joint> joints = generateNodeList(thePose);
	//std::vector<output_joint> output = convertFbxJointsToOutput(joints);
	animClip toWrite;
	createAnimationClip(toWrite, scene->GetCurrentAnimationStack(), joints);
	export_animClip_to_binary(output_file_path, toWrite);
	sdk_manager->Destroy();
	return 0;
}

int get_bindpose_from_scene(const char * fbx_file_path, const char * output_file_path) {
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if(!sdk_manager || !scene)
		return -1;

	if(scene->GetPoseCount() <= 0) {
		sdk_manager->Destroy();
		return -1;
	}

	FbxPose* thePose = scene->GetPose(0);
	std::vector<fbx_joint> joints = generateNodeList(thePose);
	std::vector<output_joint> output = convertFbxJointsToOutput(joints);
	export_bindpose_to_binary(output_file_path, output);
	sdk_manager->Destroy();
	return 0;
}

int write_scene_to_file(const char * fbx_file_path, const char * output_file_path) {
	//IMPORTANT: This function generates temporary files for use with the C# app.  These files are not final and get cleaned up by the app when not in use.
	//In other words you're stuck with using the app buddy
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if(!sdk_manager || !scene)
		return -1;

	int geoCount = scene->GetGeometryCount();

	extracted_scene toWrite;

	for(int i = 0; i < geoCount; ++i) {
		FbxGeometry* geo = scene->GetGeometry(i);
		FbxNode* node = geo->GetNode();
		std::string name(node->GetName());
		size_t index = name.find("__");
		name = name.substr(0, index);
		name.append(".ghost");
		toWrite.prefabs[name].name = name;
		FbxAMatrix m = node->EvaluateGlobalTransform();
		float4x4 toPush;
		//Rapid matrix conversion
		auto src = &m[0][0];
		auto dst = (float*)&toPush;
		for(int c = 0; c < 16; ++c) {
			dst[c] = (float)src[c];
		}

		toPush.m13 *= -1.0f;
		toPush.m23 *= -1.0f;

		toPush.m31 *= -1.0f;
		toPush.m32 *= -1.0f;

		toPush.m43 *= -1.0f;
		toWrite.prefabs[name].matrices.push_back(toPush);
	}
	export_scene_to_binary(output_file_path, toWrite);
	return 0;
}

FbxManager* create_and_import(const char* fbx_file_path, FbxScene*& lScene) {
	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();
	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
	// Use the first argument as the filename for the importer.
	if(!lImporter->Initialize(fbx_file_path, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		return nullptr;
	}
	// Create a new scene so that it can be populated by the imported file.
	lScene = FbxScene::Create(lSdkManager, "imported_scene");
	// Import the contents of the file into the scene.
	lImporter->Import(lScene);
	lImporter->Destroy();
	return lSdkManager;
}

simple_mesh make_friendly_mesh(FbxMesh * mesh, std::vector<influence_set>& influences) {
	simple_mesh ret;
	int iCount = mesh->GetPolygonVertexCount();
	int pCount = mesh->GetPolygonCount();

	FbxVector4* positions = mesh->GetControlPoints();
	FbxGeometryElementNormal* normals = mesh->GetElementNormal(0);
	FbxGeometryElementUV * UVs = mesh->GetElementUV(0);
	std::vector<simple_vert> verts;
	std::vector<int> indices;
	bool globalControlPoints = true;

	bool hasNormal = mesh->GetElementNormalCount() > 0;
	bool hasUV = mesh->GetElementUVCount() > 0;

	FbxGeometryElement::EMappingMode normalMode = FbxGeometryElement::eNone;
	FbxGeometryElement::EMappingMode UVMode = FbxGeometryElement::eNone;

	if(hasNormal) {
		normalMode = normals->GetMappingMode();
		if(normalMode == FbxGeometryElement::eNone)
			hasNormal = false;
		if(hasNormal && normalMode != FbxGeometryElement::eByControlPoint)
			globalControlPoints = false;
	}

	if(hasUV) {
		UVMode = UVs->GetMappingMode();
		if(UVMode == FbxGeometryElement::eNone)
			hasUV = false;
		if(hasUV && UVMode != FbxGeometryElement::eByControlPoint)
			globalControlPoints = false;
	}

	int vCount = mesh->GetControlPointsCount();
	if(!globalControlPoints)
		vCount = pCount * 3;

	FbxStringList UVNames;
	mesh->GetUVSetNames(UVNames);
	const char * UVName = NULL;
	if(hasUV && UVNames.GetCount() > 0)
		UVName = UVNames[0];

	if(globalControlPoints) {
		for(int i = 0; i < vCount; ++i) {
			simple_vert toPush;
			FbxVector4 currPos = positions[i];
			toPush.pos[0] = (float)currPos[0];
			toPush.pos[1] = (float)currPos[1];
			toPush.pos[2] = -(float)currPos[2];
			toPush.pos[3] = 1.0f;

			if(hasNormal) {
				int index = i;
				if(normals->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					index = normals->GetIndexArray().GetAt(index);

				FbxVector4 currNorm = normals->GetDirectArray().GetAt(index);
				toPush.norm[0] = (float)currNorm[0];
				toPush.norm[1] = (float)currNorm[1];
				toPush.norm[2] = -(float)currNorm[2];
			}

			if(hasUV) {
				int index = i;
				if(UVs->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					index = UVs->GetIndexArray().GetAt(index);

				FbxVector2 currUV = UVs->GetDirectArray().GetAt(index);
				toPush.uv[0] = (float)currUV[0];
				toPush.uv[1] = 1.0f - (float)currUV[1];
			}
			verts.push_back(toPush);
		}
	}
	int vertexPlace = 0;
	for(int i = 0; i < pCount; ++i) {
		for(int j = 0; j < 3; ++j) {
			int index = mesh->GetPolygonVertex(i, j);
			if(globalControlPoints)
				indices.push_back(index);
			else {
				indices.push_back(vertexPlace);
				simple_vert toPush;
				FbxVector4 currPos = positions[index];
				toPush.pos[0] = (float)currPos[0];
				toPush.pos[1] = (float)currPos[1];
				toPush.pos[2] = -(float)currPos[2];
				toPush.pos[3] = 1.0f;

				if(hasNormal) {
					FbxVector4 currNorm;
					mesh->GetPolygonVertexNormal(i, j, currNorm);
					toPush.norm[0] = (float)currNorm[0];
					toPush.norm[1] = (float)currNorm[1];
					toPush.norm[2] = -(float)currNorm[2];
				}

				if(hasUV) {
					bool unmappedUV;
					FbxVector2 currUV;
					mesh->GetPolygonVertexUV(i, j, UVName, currUV, unmappedUV);
					toPush.uv[0] = (float)currUV[0];
					toPush.uv[1] = 1.0f - (float)currUV[1];
				}

				if(influences.size() > 0) {
					for(int infIndex = 0; infIndex < 4; ++infIndex) {
						toPush.indices[infIndex] = influences[index].influences[infIndex].index;
						toPush.weights[infIndex] = influences[index].influences[infIndex].weight;
					}
				}
				verts.push_back(toPush);
			}
			++vertexPlace;
		}
	}
	//Where the magic happens
	ret.indexCount = (unsigned int)indices.size();
	ret.vertCount = vCount;
	ret.verts = verts;
	ret.indices = indices;
	return ret;
}

simple_material make_friendly_material(FbxSurfaceMaterial * mat) {
	simple_material ret;
	bool isPhong = mat->Is<FbxSurfacePhong>();
	if(isPhong) {
		FbxFileTexture* Dtexture = ((FbxSurfacePhong*)mat)->Diffuse.GetSrcObject<FbxFileTexture>();
		FbxFileTexture* Stexture = ((FbxSurfacePhong*)mat)->Specular.GetSrcObject<FbxFileTexture>();
		FbxFileTexture* Etexture = ((FbxSurfacePhong*)mat)->Emissive.GetSrcObject<FbxFileTexture>();
		if(Dtexture) {
			ret.diffusePath = Dtexture->GetRelativeFileName();
		}
		else {
			ret.diffusePath = "";
		}
		ret.diffusePathLength = (unsigned int)strlen(ret.diffusePath) + 1;
		if(Stexture) {
			ret.specPath = Stexture->GetRelativeFileName();
		}
		else {
			ret.specPath = "";
		}
		ret.specPathLength = (unsigned int)strlen(ret.specPath) + 1;
		if(Etexture) {
			ret.emissivePath = Etexture->GetRelativeFileName();
		}
		else {
			ret.emissivePath = "";
		}
		ret.emissivePathLength = (unsigned int)strlen(ret.emissivePath) + 1;
		ret.diffuseFactor = (float)((FbxSurfacePhong*)mat)->DiffuseFactor.Get();
		ret.specFactor = (float)((FbxSurfacePhong*)mat)->SpecularFactor.Get();
		ret.emissiveFactor = (float)((FbxSurfacePhong*)mat)->EmissiveFactor.Get();
	}
	else {
		FbxFileTexture* Dtexture = ((FbxSurfaceLambert*)mat)->Diffuse.GetSrcObject<FbxFileTexture>();
		FbxFileTexture* Etexture = ((FbxSurfaceLambert*)mat)->Emissive.GetSrcObject<FbxFileTexture>();
		if(Dtexture) {
			ret.diffusePath = Dtexture->GetRelativeFileName();
		}
		else
			ret.diffusePath = "";
		ret.diffusePathLength = (unsigned int)strlen(ret.diffusePath) + 1;
		ret.specPath = "";
		ret.specPathLength = 0;
		ret.specFactor = 0;
		if(Etexture) {
			ret.emissivePath = Etexture->GetRelativeFileName();
		}
		else
			ret.emissivePath = "";
		ret.emissivePathLength = (unsigned int)strlen(ret.emissivePath) + 1;
		ret.diffuseFactor = (float)((FbxSurfaceLambert*)mat)->DiffuseFactor.Get();
		ret.emissiveFactor = (float)((FbxSurfaceLambert*)mat)->EmissiveFactor.Get();
	}

	return ret;
}

std::vector<fbx_joint> generateNodeList(FbxPose * pose) {
	FbxSkeleton* skeleton = nullptr;
	for(int i = 0; i < pose->GetCount(); ++i) {
		skeleton = pose->GetNode(i)->GetSkeleton();
		if(skeleton && skeleton->IsSkeletonRoot())
			break;
	}
	if(skeleton == nullptr || !skeleton->IsSkeletonRoot())
		return std::vector<fbx_joint>();

	std::vector<fbx_joint> ret;
	fbx_joint temp;
	temp.node = skeleton->GetNode();
	temp.parent_index = -1;
	temp.name = temp.node->GetName();
	ret.push_back(temp);
	for(size_t i = 0; i < ret.size(); ++i) {
		for(size_t child = 0; child < ret[i].node->GetChildCount(); ++child) {
			temp.node = ret[i].node->GetChild((int)child);
			temp.parent_index = (int)i;
			temp.name = temp.node->GetName();
			ret.push_back(temp);
		}
	}
	return ret;
}

std::vector<output_joint> convertFbxJointsToOutput(std::vector<fbx_joint>& joints) {
	std::vector<output_joint> ret;
	for(size_t i = 0; i < joints.size(); ++i) {
		output_joint temp;
		FbxAMatrix globalxform = joints[i].node->EvaluateGlobalTransform();
		const auto src = &globalxform[0][0];
		const auto dst = (float*)&temp.global_xform;
		for(int c = 0; c < 16; ++c) {
			dst[c] = (float)src[c];
		}
		//temp.global_xform[0] *= -1.0f;
		//temp.global_xform[1] *= -1.0f;

		temp.global_xform.m13 *= -1.0f;
		temp.global_xform.m23 *= -1.0f;

		//temp.global_xform[4] *= -1.0f;
		//temp.global_xform[5] *= -1.0f;
		temp.global_xform.m31 *= -1.0f;
		temp.global_xform.m32 *= -1.0f;
		//temp.global_xform[10] *= -1.0f;
		temp.global_xform.m43 *= -1.0f;
		temp.parent_index = joints[i].parent_index;
		temp.name = joints[i].name;
		temp.nameLen = (unsigned int)(strlen(temp.name) + 1);
		ret.push_back(temp);
	}
	return ret;
}

std::vector<output_joint> evaluateTransformsAtTime(std::vector<fbx_joint>& joints, FbxTime time) {
	std::vector<output_joint> ret;
	for(size_t i = 0; i < joints.size(); ++i) {
		output_joint temp;
		FbxAMatrix globalxform = joints[i].node->EvaluateGlobalTransform(time);
		//Rapid matrix conversion
		const auto src = &globalxform[0][0];
		const auto dst = (float*)&temp.global_xform;
		for(int c = 0; c < 16; ++c) {
			dst[c] = (float)src[c];
		}

		//temp.global_xform[0] *= -1.0f;
		//temp.global_xform[1] *= -1.0f;
		temp.global_xform.m13 *= -1.0f;
		temp.global_xform.m23 *= -1.0f;

		//temp.global_xform[4] *= -1.0f;
		//temp.global_xform[5] *= -1.0f;
		temp.global_xform.m31 *= -1.0f;
		temp.global_xform.m32 *= -1.0f;
		//temp.global_xform[10] *= -1.0f;
		temp.global_xform.m43 *= -1.0f;
		temp.parent_index = joints[i].parent_index;
		temp.name = joints[i].name;
		temp.nameLen = (unsigned int)(strlen(temp.name) + 1);
		ret.push_back(temp);
	}
	return ret;
}

std::vector<influence_set> extractInfluences(FbxPose * pose, int count, std::vector<fbx_joint>& joints) {
	std::vector<influence_set> ret;
	ret.resize(count);

	int poseCount = pose->GetCount();
	FbxMesh* mesh = nullptr;
	for(int i = 0; i < poseCount; ++i) {
		FbxNode* node = pose->GetNode(i);
		mesh = node->GetMesh();
		if(mesh)
			break;
	}
	if(!mesh)
		return std::vector<influence_set>();

	poseCount = mesh->GetDeformerCount();
	FbxDeformer* skin = nullptr;
	for(int i = 0; i < poseCount; ++i) {
		skin = mesh->GetDeformer(i);
		if(skin->Is<FbxSkin>())
			break;
	}

	if(!skin || !skin->Is<FbxSkin>())
		return std::vector<influence_set>();

	poseCount = ((FbxSkin*)skin)->GetClusterCount();
	for(int i = 0; i < poseCount; ++i) {
		FbxCluster* cluster = ((FbxSkin*)skin)->GetCluster(i);
		FbxNode* node = cluster->GetLink();
		int index;
		for(index = 0; index < joints.size(); ++index) {
			if(node == joints[index].node)
				break;
		}
		influence inf;
		inf.index = index;
		double* weights = cluster->GetControlPointWeights();
		int* indices = cluster->GetControlPointIndices();
		for(index = 0; index < cluster->GetControlPointIndicesCount(); ++index) {
			float weight = (float)weights[index];
			inf.weight = weight;

			int lowestIndex = 4;
			float lowestWeight = inf.weight;
			for(int infIndex = 0; infIndex < 4; ++infIndex) {
				if(ret[indices[index]].influences[infIndex].weight < lowestWeight) {
					lowestIndex = infIndex;
					lowestWeight = ret[indices[index]].influences[infIndex].weight;
				}
			}
			if(lowestIndex != 4)
				ret[indices[index]].influences[lowestIndex] = inf;
		}
	}
	return ret;
}

int get_clusterdata_from_scene(const char * fbx_file_path, const char * output_file_path) {
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if(sdk_manager == nullptr)
		return -1;

	int geoCount = scene->GetGeometryCount();
	FbxGeometry* geo = nullptr;
	for(int i = 0; i < geoCount; ++i) {
		geo = scene->GetGeometry(i);

		if(geo->GetAttributeType() == FbxNodeAttribute::eMesh)
			break;
	}
	if(scene != nullptr && scene->GetPoseCount() > 0) {
		FbxPose* thePose = scene->GetPose(0);
		std::vector<fbx_joint> joints = generateNodeList(thePose);
		std::vector<influence_set> influences = extractInfluences(thePose, ((FbxMesh*)geo)->GetControlPointsCount(), joints);

		//export_clusterdata_to_binary(output_file_path, influences);
	}
	sdk_manager->Destroy();
	return 0;
}

void createAnimationClip(animClip & clip, FbxAnimStack * animStack, std::vector<fbx_joint>& joints) {
	FbxTime time = animStack->GetLocalTimeSpan().GetDuration();
	clip.duration = time.GetSecondDouble();
	FbxLongLong frames = time.GetFrameCount(FbxTime::EMode::eFrames24);
	for(int i = 1; i <= frames; ++i) {
		keyframe toPush;
		time.SetFrame(i, FbxTime::EMode::eFrames24);
		toPush.time = time.GetSecondDouble();
		toPush.joints = evaluateTransformsAtTime(joints, time);
		clip.keyframes.push_back(toPush);
	}
}

void export_animClip_to_binary(const char * output_file_path, animClip & clip) {
	std::ofstream writer;
	writer.open(output_file_path, std::ios_base::binary | std::ios_base::out);
	writer.write((char*)&clip.duration, sizeof(clip.duration));
	int numFrames = (int)clip.keyframes.size();
	writer.write((char*)&numFrames, sizeof(numFrames));

	for(size_t i = 0; i < numFrames; ++i) {
		keyframe kframe = clip.keyframes[i];
		writer.write((char*)&kframe.time, sizeof(kframe.time));
		int len = (int)kframe.joints.size();
		writer.write((char*)&len, sizeof(len));
		for(size_t index = 0; index < len; ++index) {

			output_joint joint = kframe.joints[index];
			writer.write((char*)&joint.parent_index, sizeof(joint.parent_index));
			const auto dst = (float*)&joint.global_xform;
			for(int almostThere = 0; almostThere < 16; ++almostThere) {
				writer.write((char*)&dst[almostThere], sizeof(float));
			}
			writer.write((char*)&joint.nameLen, sizeof(joint.nameLen));
			writer.write(joint.name, joint.nameLen);
		}
	}
	writer.close();
}

void export_mesh_to_binary(const char * output_file_path, simple_mesh mesh) {
	std::fstream writer;
	writer.open(output_file_path, std::ios_base::binary | std::ios_base::out);
	writer.write((char*)&mesh.vertCount, sizeof(mesh.vertCount));
	for(unsigned int i = 0; i < mesh.vertCount; ++i) {
		simple_vert tempVert = mesh.verts[i];
		writer.write((char*)&tempVert.pos[0], sizeof(tempVert.pos[0]));
		writer.write((char*)&tempVert.pos[1], sizeof(tempVert.pos[1]));
		writer.write((char*)&tempVert.pos[2], sizeof(tempVert.pos[2]));
		writer.write((char*)&tempVert.pos[3], sizeof(tempVert.pos[3]));
		writer.write((char*)&tempVert.norm[0], sizeof(tempVert.norm[0]));
		writer.write((char*)&tempVert.norm[1], sizeof(tempVert.norm[1]));
		writer.write((char*)&tempVert.norm[2], sizeof(tempVert.norm[2]));
		writer.write((char*)&tempVert.uv[0], sizeof(tempVert.uv[0]));
		writer.write((char*)&tempVert.uv[1], sizeof(tempVert.uv[1]));
	}
	writer.write((char*)&mesh.indexCount, sizeof(mesh.indexCount));
	for(unsigned int i = 0; i < mesh.indexCount; ++i) {
		unsigned int index = i;
		if(index % 3 == 1)
			index = index + 1;
		else if(index % 3 == 2)
			index = index - 1;
		writer.write((char*)&mesh.indices[index], sizeof(mesh.indices[index]));
	}
	writer.close();
}

void export_material_to_binary(const char * output_file_path, simple_material mat) {
	std::fstream writer;
	writer.open(output_file_path, std::ios_base::binary | std::ios_base::out);
	writer.write((char*)&mat.diffusePathLength, sizeof(mat.emissivePathLength));
	writer.write(mat.diffusePath, mat.diffusePathLength);
	writer.write((char*)&mat.specPathLength, sizeof(mat.specPathLength));
	writer.write(mat.specPath, mat.specPathLength);
	writer.write((char*)&mat.emissivePathLength, sizeof(mat.emissivePathLength));
	writer.write(mat.emissivePath, mat.emissivePathLength);
	writer.write((char*)&mat.diffuseFactor, sizeof(mat.diffuseFactor));
	writer.write((char*)&mat.specFactor, sizeof(mat.specFactor));
	writer.write((char*)&mat.emissiveFactor, sizeof(mat.emissiveFactor));
	writer.close();
}

void export_bindpose_to_binary(const char * output_file_path, std::vector<output_joint>& joints) {
	std::fstream writer;
	writer.open(output_file_path, std::ios_base::binary | std::ios_base::out);
	int len = (int)joints.size();
	writer.write((char*)&len, sizeof(len));
	for(int i = 0; i < len; ++i) {
		output_joint joint = joints[i];

		writer.write((char*)&joint.parent_index, sizeof(joint.parent_index));
		const auto dst = (float*)&joint.global_xform;
		for(int almostThere = 0; almostThere < 16; ++almostThere) {
			writer.write((char*)&dst[almostThere], sizeof(float));
		}
		writer.write((char*)&joint.nameLen, sizeof(joint.nameLen));
		writer.write(joint.name, joint.nameLen);
		/*writer.write((char*)&joints[i].parent_index, sizeof(joints[i].parent_index));
		for (int index = 0; index < 16; ++index)
			writer.write((char*)&joints[i].global_xform[index], sizeof(joints[i].global_xform[index]));

		writer.write((char*)&joints[i].nameLen, sizeof(joints[i].nameLen));
		writer.write(joints[i].name, joints[i].nameLen);*/
	}
	writer.close();
}

void export_clusterdata_to_binary(const char * output_file_path, simple_mesh mesh) {
	std::fstream writer;
	writer.open(output_file_path, std::ios_base::binary | std::ios_base::out);
	writer.write((char*)&mesh.vertCount, sizeof(mesh.vertCount));
	for(unsigned int i = 0; i < mesh.vertCount; ++i) {
		simple_vert tempVert = mesh.verts[i];
		writer.write((char*)&tempVert.pos[0], sizeof(tempVert.pos[0]));
		writer.write((char*)&tempVert.pos[1], sizeof(tempVert.pos[1]));
		writer.write((char*)&tempVert.pos[2], sizeof(tempVert.pos[2]));
		writer.write((char*)&tempVert.pos[3], sizeof(tempVert.pos[3]));
		writer.write((char*)&tempVert.norm[0], sizeof(tempVert.norm[0]));
		writer.write((char*)&tempVert.norm[1], sizeof(tempVert.norm[1]));
		writer.write((char*)&tempVert.norm[2], sizeof(tempVert.norm[2]));
		writer.write((char*)&tempVert.uv[0], sizeof(tempVert.uv[0]));
		writer.write((char*)&tempVert.uv[1], sizeof(tempVert.uv[1]));
		float sum = tempVert.weights[0] + tempVert.weights[1] + tempVert.weights[2] + tempVert.weights[3];
		tempVert.weights[0] /= sum;
		tempVert.weights[1] /= sum;
		tempVert.weights[2] /= sum;
		tempVert.weights[3] /= sum;

		for(int index = 0; index < 4; ++index) {
			writer.write((char*)&tempVert.indices[index], sizeof(tempVert.indices[index]));
			writer.write((char*)&tempVert.weights[index], sizeof(tempVert.weights[index]));
		}
	}
	writer.write((char*)&mesh.indexCount, sizeof(mesh.indexCount));
	for(unsigned int i = 0; i < mesh.indexCount; ++i) {
		unsigned int index = i;
		if(index % 3 == 1)
			index = index + 1;
		else if(index % 3 == 2)
			index = index - 1;
		writer.write((char*)&mesh.indices[index], sizeof(mesh.indices[index]));
	}



	writer.close();
}

void export_scene_to_binary(const char * output_file_path, extracted_scene & scene) {
	std::fstream writer;
	writer.open(output_file_path, std::ios_base::binary | std::ios_base::out);
	auto iter = scene.prefabs.begin();
	int size = (int)scene.prefabs.size();
	writer.write((char*)&size, sizeof(size));
	for(; iter != scene.prefabs.end(); ++iter) {
		size = (int)iter->first.length() + 1;
		writer.write((char*)&size, sizeof(size));
		writer.write((char*)iter->first.c_str(), size);
		size = (int)iter->second.matrices.size();
		writer.write((char*)&size, sizeof(size));
		for(int i = 0; i < size; ++i) {
			writer.write((char*)&iter->second.matrices[i], sizeof(float) * 16);
		}
	}
}
