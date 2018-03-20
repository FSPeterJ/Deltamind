#pragma once


#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

//struct _object;
//typedef _object PyObject;
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

	template<typename ReturnType>
	ReturnType ExecuteFunction(const char* function);

	bool ExecuteFile(const char* _fileName);
	bool ExecuteFile(const char* folderName, const char* _fileName);
};


template<typename ReturnType, typename... Params>
ReturnType PythonInterface::ExecuteFunction(const char* function, Params... params) {
	if(!defs.count(function)) {
		defs[function] = PyObject_GetAttrString(pModule, function);
	}

	PyObject *pFunc = defs[function];
	if(!pFunc || !PyCallable_Check(pFunc)) { throw "Function not callable or does not exist."; }

	pArgs = PyTuple_New(sizeof...(params));

	int i = -1;
	SetParam([&](auto param) { PyTuple_SetItem(pArgs, ++i, ParseArg(param)); }, params...);

	PyObject* pResult = PyObject_CallObject(pFunc, pArgs);

	if(pResult) {
		if(typeid(int) == typeid(ReturnType)) {
			return (int) PyLong_AsLong(pResult);
		}
		if(typeid(double) == typeid(ReturnType)) {
			return PyFloat_AsDouble(pResult);
		}
	}
	throw "failed or no type conversion avaliable for return.";
}
template<typename ReturnType>
ReturnType PythonInterface::ExecuteFunction(const char* function) {
	if(!defs.count(function)) {
		defs[function] = PyObject_GetAttrString(pModule, function);
	}

	PyObject *pFunc = defs[function];
	if(!pFunc || !PyCallable_Check(pFunc)) { throw "Function not callable or does not exist."; }

	PyObject* pResult = PyObject_CallObject(pFunc, 0);

	if(pResult) {
		if(typeid(int) == typeid(ReturnType)) {
			return (int) PyLong_AsLong(pResult);
		}
		if(typeid(double) == typeid(ReturnType)) {
			return PyFloat_AsDouble(pResult);
		}
	}
	throw "failed or no type conversion avaliable for return.";
}

template <typename T>
PyObject* PythonInterface::ParseArg(T param) {
	if(typeid(int) == typeid(T)) {
		return PyLong_FromLong((long) param);
	}
	if(typeid(double) == typeid(T)) {
		return PyFloat_FromDouble(param);
	}
	throw "no type conversion avaliable for argument.";
}
