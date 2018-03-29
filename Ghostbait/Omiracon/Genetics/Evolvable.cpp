#include "Evolvable.h"

namespace Omiracon {
	namespace Genetics {
		Evolvable::Evolvable() : traits(Traits()), performance(Performance()) {}
		Evolvable::~Evolvable() {
			//delete traits; delete measure;
		}
	}
}
