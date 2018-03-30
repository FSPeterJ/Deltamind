#pragma once
#include "Traits.h"

namespace Omiracon {
	namespace Genetics {
		struct Evolvable {
			Evolvable();
			~Evolvable();

			void HandleMutationIteration(const MutationType& mutation, const size_t& curGen);

			Traits traits;
			Performance performance;
		private:
			friend class Evolver;
			int generation = -1;
		};
	}
}
