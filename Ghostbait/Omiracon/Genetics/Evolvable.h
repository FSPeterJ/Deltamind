#pragma once
#include "Traits.h"

namespace Omiracon {
	namespace Genetics {
		struct Evolvable {
			Traits traits;
			Performance performance;
			Evolvable();
			~Evolvable();
		private:
			friend class Evolver;
			int generation = -1;
			//void CreateSimulatedResults(void) {
			//	float timeLasted = Random::RandomNumber(1.0f, 60.0f)*traits[SPEED];
			//	float damageDelt = Random::RandomNumber(0.0f, 100.0f)*traits[STRENGTH];
			//	float damageRecv = Random::RandomNumber(0.0f, 100.0f)*(1.0f - traits[DEFENSE]);
			//	float nodesTrav = Random::RandomNumber(0.0f, 500.0f)*(1.0f - traits[INTELLIGENCE]);
			//
			//	measure.timeLasted = timeLasted + timeLasted * (traits[SPEED] + traits[ENERGY] * 0.5f + traits[COORDINATION] * 0.25f + traits[BALANCE] * 0.125f);
			//	measure.damageDelt = damageDelt + damageDelt * (traits[STRENGTH] + traits[POWER] * 0.5f + traits[ACCURACY] * 0.25f + traits[LUCK] * 0.125f);
			//	measure.damageReceived = damageRecv - damageRecv * (traits[DEFENSE] + traits[ENDURANCE] * 0.5f + traits[STAMINA] * 0.25f + traits[RESISTANCE] * 0.125f);
			//	measure.nodesTraversed = nodesTrav - nodesTrav * (traits[INTELLIGENCE] + traits[WISDOM] * 0.5f + traits[EVASION] * 0.25f + traits[DEXTERITY] * 0.125f);
			//
			//	measure.damageReceived = measure.damageReceived < 0 ? 0 : measure.damageReceived;
			//	measure.nodesTraversed = measure.nodesTraversed < 0 ? 0 : measure.nodesTraversed;
			//}
		};
	}
}
