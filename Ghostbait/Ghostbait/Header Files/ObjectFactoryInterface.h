#pragma once
class ObjFactoryInterface
{
public:
	ObjFactoryInterface() {}
	virtual ~ObjFactoryInterface() {}
	// Override with instructions for the factory on how to assemble the object
	virtual void Assemble() = 0;
	virtual void* GetMesh(int id) = 0;
};