#include "stdafx.h"
#include "PythonInterface.h"


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


void PythonInterface::OpenFile(const char* fileName) {
	pName = PyUnicode_DecodeFSDefault(fileName);
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	//pDict = PyModule_GetDict(pModule);
	//if(PyErr_Occurred()) PyErr_Print();
}

void  PythonInterface::RunCode(const char* code) { PyRun_SimpleString(code); }

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
