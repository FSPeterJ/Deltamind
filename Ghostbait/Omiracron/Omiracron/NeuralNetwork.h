#pragma once
#include <vector>
#include "Neuron.h"

namespace GhostMind {
#define GHOSTNET_FILE(name) name ## ".ghostnet"
#define TRAIN_FILE(name) name ## ".ghosttrain"

	typedef std::vector<Neuron> Layer;

	class NeuralNetworkDiagnositics {
		double recentAverageError = 0.0;
		double recentAverageSmoothingFactor = 0.0;
	public:
		void CalculateMeanError(const double error) {
			recentAverageError = (recentAverageError * recentAverageSmoothingFactor + error) / (recentAverageSmoothingFactor + 1.0);
		}

		double GetRecentAverageError(void) const { return recentAverageError; }
	};

	class NeuralNetwork: public NeuralNetworkDiagnositics {
		std::vector<std::size_t> structure;
		std::vector<Layer> network;
		double error = 1.0;

		/// <summary>
		/// Feeds input values through the network.
		/// </summary>
		/// <param name="inputValues">The input values.</param>
		void FeedForward(const std::vector<double> &inputValues);

		/// <summary>
		/// Backs the propagate.
		/// </summary>
		/// <param name="targetValues">The target values.</param>
		void BackPropagate(const std::vector<double>& targetValues);

		/// <summary>
		/// Gets the output.
		/// </summary>
		/// <param name="outputValues">The output values to be filled out.</param>
		void GetOutput(std::vector<double>& outputValues);

		/// <summary>
		/// Loads a neural network and populates this instance from a specified file.
		/// </summary>
		/// <param name="file">The file to load.</param>
		void Load(const char* file);
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="NeuralNetwork"/> class.
		/// </summary>
		/// <param name="structure">The number of neurons for each layer.</param>
		NeuralNetwork(const std::vector<std::size_t>&);

		/// <summary>
		/// Initializes a new instance of the <see cref="NeuralNetwork"/> class.
		/// </summary>
		/// <param name="file">The file to load the neural network from.</param>
		NeuralNetwork(const char* file);

		std::size_t GetInputCount() const { return structure.front(); }

		std::size_t GetOutputCount() const { return structure.back(); }

		/// <summary>
		/// Saves this instance to the specified file.
		/// </summary>
		/// <param name="file">The file to save to.</param>
		void Save(const char* file);

		/// <summary>
		/// Processes the input. Given input values will fill the output vector.
		/// </summary>
		/// <param name="inputValues">The input values.</param>
		/// <param name="output">The output vector to be filled.</param>
		void ProcessInput(const std::vector<double> &inputValues, std::vector<double>& output);

		/// <summary>
		/// Trains this instance off the specified train data file.
		/// </summary>
		/// <param name="trainDataFile">The train data file.</param>
		void Train(const char* trainDataFile);
	};
}
