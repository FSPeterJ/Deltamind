#pragma once

#define TESTING 0

#ifdef OMIRACRON_EXPORTS  
#if TESTING
#define OMIRACRON_API
#else
#define OMIRACRON_API __declspec(dllexport)   
#endif
#else  
#define OMIRACRON_API __declspec(dllimport)   
#endif  

namespace Omiracron {
	// This class is exported from the Omiracron.dll  
	class Functions {
	public:
		static OMIRACRON_API void GiveState(
			float distToPlayer, float distToCore, float playerHealth,
			float coreHealth, float myHealth, int turretsInRange,
			int enemiesInRange, float turretDamages, int myType);

		static OMIRACRON_API int GetAction();

		static OMIRACRON_API void Print(const char* text);
		
		static OMIRACRON_API void Beep();

		static OMIRACRON_API double Multiply(double a, double b);
	};
}
