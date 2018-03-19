#pragma once

struct _object;
typedef _object PyObject;
#include <unordered_map>

class PythonInterface {
	PyObject *pName, *pModule, *pDict, *pArgs;

	std::unordered_map<const char*, PyObject*> defs;

	template <class T, class... Params>
	void SetParam(T f, Params... params) { int x[] = {(f(params), 0)...}; }

	template <typename T>
	PyObject* ParseArg(T param);

public:
	PythonInterface();
	PythonInterface(const char* file);
	~PythonInterface();

	void OpenFile(const char* fileName);

	void RunCode(const char* code);

	template<typename ReturnType, typename... Params>
	ReturnType ExecuteFunction(const char* function, Params... params);

	bool ExecuteFile(const char* _fileName);
	bool ExecuteFile(const char* folderName, const char* _fileName);
};
