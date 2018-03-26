#include <cassert>
#include <functional>
#include "TrainingData.h"
#include "BinaryFileIO.h"
using namespace Common;

namespace Omiracon {
	namespace Neural {
		template<typename I, typename O>
		TrainingDataReader<I, O>::TrainingDataReader(const char* fileName) {
			Reader::CreateStream(fileName);
			inputSize = Reader::ReadInt();
			outputSize = Reader::ReadInt();
			iterations = Reader::ReadInt();
		}

		template<typename I, typename O>
		TrainingDataReader<I, O>::~TrainingDataReader() {
			Reader::CloseStream();
		}

		template<typename I, typename O>
		void TrainingDataReader<I, O>::GetInputs(std::vector<double>& inputValues) const {
			inputValues.clear();

			for(std::size_t n = 0; n < inputSize; ++n) {
				inputValues.push_back(double(Reader::Read<I>()));
			}
		}

		template<typename I, typename O>
		void TrainingDataReader<I, O>::GetTargets(std::vector<double>& targetValues) const {
			targetValues.clear();

			for(std::size_t n = 0; n < outputSize; ++n) {
				targetValues.push_back(double(Reader::Read<O>()));
			}
		}

		template<typename I, typename O>
		TrainingDataWriter<I, O>::TrainingDataWriter(const char *fileName) {
			Writer::CreateStream(fileName);
		}

		template<typename I, typename O>
		TrainingDataWriter<I, O>::~TrainingDataWriter() {
			Writer::CloseStream();
		}

		template<typename I, typename O>
		void TrainingDataWriter<I, O>::SetInputFunction(std::function<std::vector<I>(void)> f) {
			inputFunct = f;
		}

		template<typename I, typename O>
		void TrainingDataWriter<I, O>::SetTargetFunction(std::function<std::vector<O>(void)> f) {
			targetFunct = f;
		}

		template<typename I, typename O>
		void TrainingDataWriter<I, O>::CreateData(std::size_t inputSize, std::size_t outputSize, std::size_t iterations) const {
			assert(inputFunct != nullptr);
			assert(targetFunct != nullptr);

			assert(inputSize > 0);
			assert(outputSize > 0);
			assert(iterations > 0);

			Writer::Write(inputSize);
			Writer::Write(outputSize);
			Writer::Write(iterations);

			for(std::size_t i = 0; i < iterations; ++i) {

				std::vector<I> inputs = inputFunct();
				assert(inputs.size() == inputSize);

				for(auto& inputData : inputs) {
					Writer::Write<I>(inputData);
				}

				std::vector<O> targets = targetFunct();
				assert(targets.size() == outputSize);

				for(auto& targetData : targets) {
					Writer::Write<O>(targetData);
				}
			}
		}
	}
}

template class Omiracon::Neural::TrainingDataWriter<int, int>;
template class Omiracon::Neural::TrainingDataReader<int, int>;
template class Omiracon::Neural::TrainingDataWriter<double, double>;
template class Omiracon::Neural::TrainingDataReader<double, double>;
