#pragma once
#include <iosfwd>
#include <vector>
#include <functional>
namespace Omiracon {
	namespace Neural {
		template<typename InputType = double, typename TargetType = double>
		class TrainingDataReader {
			//std::ifstream trainingDataFile;
			std::size_t inputSize, outputSize, iterations;
		public:
			TrainingDataReader(const char*);
			~TrainingDataReader();

			void GetInputs(std::vector<double>&) const;

			void GetTargets(std::vector<double>&) const;

			std::size_t GetIterations() const { return iterations; }
		};

		template<typename InputType = double, typename TargetType = double>
		class TrainingDataWriter {
			//std::ofstream trainingDataFile;

			std::function<std::vector<InputType>(void)> inputFunct;
			std::function<std::vector<TargetType>(void)> targetFunct;
		public:
			/// <summary>
			/// Initializes a new instance of the <see cref="TrainingDataWriter"/> class.
			/// </summary>
			/// <param name="">The path of the file to write the training data to.</param>
			TrainingDataWriter(const char*);

			~TrainingDataWriter();

			/// <summary>
			/// Sets the input function.
			/// </summary>
			/// <param name="">The function that shall be used when generating training input data.</param>
			void SetInputFunction(std::function<std::vector<InputType>(void)>);

			/// <summary>
			/// Sets the target function.
			/// </summary>
			/// <param name="">The function that shall be used when generating training target data.</param>
			void SetTargetFunction(std::function<std::vector<TargetType>(void)>);

			/// <summary>
			/// Generates and writes the data.
			/// </summary>
			/// <param name="inputSize">Number of the input neurons.</param>
			/// <param name="outputSize">Number of the output nerons.</param>
			/// <param name="iterations">The number of iterations to write.</param>
			void CreateData(std::size_t inputSize, std::size_t outputSize, std::size_t iterations) const;
		};
	}
}
