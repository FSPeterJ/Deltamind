#include "Evolvable.h"

namespace Omiracon {
	namespace Genetics {
		Evolvable::Evolvable() : traits(Traits()), performance(Performance()) {}
		Evolvable::~Evolvable() {}

		void Evolvable::HandleMutationIteration(const MutationType& mutation, const size_t& curGen) {
			traits.Mutate(mutation);
			generation = (int)curGen;
			performance.Reset();
		}
	}
}
