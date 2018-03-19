#include "stdafx.h"
#include "PythonInterface.h"

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <string>

PythonInterface::PythonInterface() {
	//#if not hasattr(sys, 'argv'):
	//#sys.argv = ['']
	wchar_t *argv[] = { const_cast<wchar_t*>(L"") };
	Py_Initialize(),
	RunCode("import sys\nsys.path.append(\".\")"),
	PySys_SetArgv(1, argv);
}
PythonInterface::PythonInterface(const char* file) : PythonInterface() { OpenFile(file); }
PythonInterface::~PythonInterface() { Py_Finalize(); }

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

void PythonInterface::OpenFile(const char* fileName) {
	pName = PyUnicode_DecodeFSDefault(fileName);
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	//pDict = PyModule_GetDict(pModule);
	//if(PyErr_Occurred()) PyErr_Print();
}

void  PythonInterface::RunCode(const char* code) { PyRun_SimpleString(code); }

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

bool PythonInterface::ExecuteFile(const char* _fileName) {
	return ExecuteFile(0, _fileName);
}

bool PythonInterface::ExecuteFile(const char* folderName, const char* _fileName) {
	std::string fuckcpp("Python\\");
	if(folderName) {
		fuckcpp.append(folderName);
		fuckcpp.append("\\");
	}
	fuckcpp.append(_fileName);
	fuckcpp.append(".py");
	char* fileName = const_cast<char*>(fuckcpp.c_str());

	FILE *file = _Py_fopen_obj(Py_BuildValue("s", fileName), "r+");
	if(file) { PyRun_SimpleFileEx(file, fileName, 1); return true; }
	return false;
}
