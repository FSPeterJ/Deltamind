#pragma once

#ifdef OMIRACRON_EXPORTS  
#define OMIRACRON_API __declspec(dllexport)   
#else  
#define OMIRACRON_API __declspec(dllimport)   
#endif  

namespace Omiracron {
	// This class is exported from the Omiracron.dll  
	class Functions {
	public:
		static OMIRACRON_API void Print(const char* text);
		
		static OMIRACRON_API void Beep();

		static OMIRACRON_API double Multiply(double a, double b);
	};
}
