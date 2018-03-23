#include "AnimationManager.h"
#include <fstream>
#include <DirectXMath.h> 
#include "AnimatorStructs.h"


Animation * AnimationManager::LoadAnimation(const char * _animationFilePath, const char* _bindposeFilePath) {
	Animation* toPush = new Animation;
	std::ifstream reader;
	reader.open(_animationFilePath, std::ios_base::binary);
	if(reader.is_open()) {
		reader.read((char*)&toPush->duration, sizeof(toPush->duration));
		int length;
		reader.read((char*)&length, sizeof(length));
		char name[512];
		for(int i = 0; i < length; ++i) {
			keyframe temp;
			reader.read((char*)&temp.endTime, sizeof(temp.endTime));
			int jointNum;
			reader.read((char*)&jointNum, sizeof(jointNum));
			//TEMPORARY CODE - REMOVE LATER
			std::vector<std::vector<int>> ChildRecord_holyshit;
			//END
			for(int index = 0; index < jointNum; ++index) {
				animJoint joint;
				reader.read((char*)&joint.parent_index, sizeof(joint.parent_index));
				for(int almostThere = 0; almostThere < 4; ++almostThere) {
					reader.read((char*)&joint.transform.m[almostThere], sizeof(joint.transform.m[almostThere]));

					//TEMPORARY CODE - REMOVE LATER
					//LONGER LOAD TIMES

				}
				//TEMPORARY CODE - REMOVE LATER

				if(joint.parent_index > -1) {
					if(joint.parent_index >= (int)ChildRecord_holyshit.size()) {
						ChildRecord_holyshit.resize(joint.parent_index +1);
					}
					ChildRecord_holyshit[joint.parent_index].push_back(index);
				}

				//END
				int nameLen;
				reader.read((char*)&nameLen, sizeof(nameLen));
				reader.read(name, nameLen);
				joint.name = name;
				temp.joints.push_back(joint);
			}
			//TEMPORARY CODE - REMOVE LATER

			for(size_t index = 0; index < ChildRecord_holyshit.size(); ++index) {
				if(!ChildRecord_holyshit[index].empty()) {

					temp.joints[index].child_index = new int[ChildRecord_holyshit[index].size()];
					temp.joints[index].child_count = (int)ChildRecord_holyshit[index].size();
					memcpy(temp.joints[index].child_index, &ChildRecord_holyshit[index][0], ChildRecord_holyshit[index].size() * sizeof(int));
				}
			}
			//END
			toPush->keyframes.push_back(temp);
		}
	}
	reader.close();
	toPush->animID = (unsigned int)animations.size();
	toPush->bPose = LoadBindpose(_bindposeFilePath);
	animations.push_back(toPush);
	animNames[std::string(_animationFilePath)] = toPush;
	return animations[animations.size() - 1];
}

bindpose * AnimationManager::LoadBindpose(const char * _bindposeFilePath) {
	{
		bindpose* temp = bindposeNames[std::string(_bindposeFilePath)];
		if(temp)
			return temp;
	}
	bindpose* toPush = new bindpose;
	std::ifstream reader;
	reader.open(_bindposeFilePath, std::ios_base::binary);
	int len;
	char name[512];
	reader.read((char*)&len, sizeof(len));
	for(int i = 0; i < len; ++i) {
		animJoint alright;
		reader.read((char*)&alright.parent_index, sizeof(alright.parent_index));
		for(int almostThere = 0; almostThere < 4; ++almostThere)
			reader.read((char*)&alright.transform.m[almostThere], sizeof(alright.transform.m[almostThere]));

		XMStoreFloat4x4(&alright.transform, XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&alright.transform)), XMLoadFloat4x4(&alright.transform)));
		int nameLen;
		reader.read((char*)&nameLen, sizeof(nameLen));
		reader.read(name, nameLen);
		alright.name = name;
		toPush->joints.push_back(alright);
	}
	reader.close();
	bindPoses.push_back(toPush);
	bindposeNames[std::string(_bindposeFilePath)] = toPush;
	return bindPoses[bindPoses.size() - 1];
}

AnimationManager::AnimationManager() {}

AnimationManager::~AnimationManager() {}

void AnimationManager::Destroy() {
	for (size_t i = 0; i < animations.size(); ++i) {
		for (size_t j = 0; j < animations[i]->keyframes.size(); ++j) {
			for (size_t k = 0; k < animations[i]->keyframes[j].joints.size(); ++k) {
				delete[] animations[i]->keyframes[j].joints[k].child_index;
			}
		}
		delete animations[i];
	}
	for (size_t i = 0; i < bindPoses.size(); ++i)
		delete bindPoses[i];
}

unsigned int AnimationManager::AddElement(const char * _animationFilePath, const char * _bindposeFilePath) {
	return LoadAnimation(_animationFilePath, _bindposeFilePath)->animID;
}

Animation * AnimationManager::GetElement(const unsigned int _id) {
	for(size_t i = 0; i < animations.size(); ++i) {
		if(animations[i]->animID == _id)
			return animations[i];
	}
	return nullptr;
}

Animation * AnimationManager::GetReferenceAnimation(const char * _FilePath, const char * _bindposeFilePath) {
	Animation* toReturn = animNames[_FilePath];
	if(!toReturn) {
		toReturn = LoadAnimation(_FilePath, _bindposeFilePath);
	}
	return toReturn;
}

void AnimationManager::ResetComponent(ComponentBase * reset) {
	//TODO: deactivate reset's memory
}
