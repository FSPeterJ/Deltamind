#pragma once
#include <vector>
#include "ActivationFunctions.h"

namespace Omiracon {
	namespace Neural {
		class Neuron; typedef std::vector<Neuron> Layer;

		class Neuron {
			struct Connection {
				double weight, deltaWeight;
			};

			double output = 0.0;
			double gradient = 0.0;

			std::size_t myIndex = 0;
			unsigned char activationType = 1;

			std::vector<Connection> connections;

			typedef double Eta;
			typedef double Alpha;
			static double RandomWeight(void);

			ActivationFunction Activation;
			ActivationFunction ActivationDerivative;

			double sumDOW(const Layer&nextLayer) const;
		public:

			static Eta trainingRate; //[0.0..1.0] 0.0 is slow - 0.2 is medium - 1.0 is reckless
			static Alpha momentum; //[0.0..n] 0.5 is moderate

			/// <summary>
			/// Initializes an empty instance of the <see cref="Neuron"/> class to be loaded.
			/// </summary>
			Neuron() {}

			/// <summary>
			/// Initializes a new instance of the <see cref="Neuron"/> class.
			/// </summary>
			/// <param name="numOutputs">The number outputs.</param>
			/// <param name="index">The index.</param>
			Neuron(const std::size_t numOutputs, const std::size_t index);

			/// <summary>
			/// Reads a neuron from file into this instance.
			/// </summary>
			/// <param name="numOutputs">The number outputs.</param>
			/// <param name="index">The index.</param>
			void LoadNeuron(std::size_t numOutputs, std::size_t index);

			/// <summary>
			/// Writes out this neuron.
			/// </summary>
			void SaveNeuron();

			/// <summary>
			/// Feeds weights to the next layer.
			/// </summary>
			/// <param name="prevLayer">The previous layer to feed into this instance.</param>
			void FeedForward(const Layer &prevLayer);

			inline double GetOutputValue(void) const { return output; }

			inline void SetOutputValue(const double val) { output = val; }

			/// <summary>
			/// Calculates the output gradient.
			/// </summary>
			/// <param name="target">The target.</param>
			void CalculateOutputGradient(const double target);

			/// <summary>
			/// Calculates the hidden gradient.
			/// </summary>
			/// <param name="nextLayer">The next layer.</param>
			void CalculateHiddenGradient(const Layer &nextLayer);

			/// <summary>
			/// Updates the input weights.
			/// </summary>
			/// <param name="prevLayer">The previous layer.</param>
			void UpdateInputWeights(Layer &prevLayer);

			/// <summary>
			/// Sets the activation function for the specified neuron.
			/// </summary>
			/// <param name="n">The neuron.</param>
			/// <param name="type">The activation type.</param>
			static void SetActivationFunction(Neuron& n, const ActivationFunctions::ActivationType type);
		};
	}
}
