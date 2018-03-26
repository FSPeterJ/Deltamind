#include "ActivationFunctions.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <numeric>

namespace Omiracon {
	namespace Neural {
		namespace ActivationFunctions {
			double Sigmoid(double x) {
				//o(z) - output range [0.0..1.0]
				//used for predicting probabilities
				return 1.0 / (1.0 + exp(-x));
			}

			double SigmoidDerivative(double x) {
				return x * (1 - x);
			}

			double HyperbolicTangent(double x) {
				//tanh - output range [-1.0..1.0]
				//used for classification
				//ff
				return tanh(x);
			}

			double HyperbolicTangentDerivative(double x) {
				return 1.0 - x * x;
			}

			double RectifiedLinearUnit(double x) {
				//R(z) - output range [0.0..infinity)
				//used for convolutional or deep learning
				return std::max(0.0, x);
			}

			double RectifiedLinearUnitDerivative(double x) {
				//R(z) - output range [0.0..infinity)
				//used for convolutional or deep learning
				// g'(x) when x=0 is technically undefined, but here we'll just say it's 1. The chance of it being 0 is very unlikely so it doesn't matter.
				return (x >= 0) ? 1 : 0;
			}

			double LeakyRectifiedLinearUnit(double x) {
				//Leaky ReLU - output range (-infinity..infinity)
				return std::max(0.01 * x, x);
			}

			double LeakyRectifiedLinearUnitDerivative(double x) {
				//Leaky ReLU - output range (-infinity..infinity)
				// g'(x) when x=0 is technically undefined, but here we'll just say it's 1. The chance of it being 0 is very unlikely so it doesn't matter.
				return (x >= 1) ? 1 : 0.01;
			}

			void Softmax(std::vector<double>& neruons) {
				// output of the neural network without softmax activation function
				double ymax = *std::max_element(std::begin(neruons), std::end(neruons));
				for(int f = 0; f < neruons.size(); ++f) {
					neruons[f] = exp(neruons[f] - ymax);
				}

				double sum = std::accumulate(neruons.begin(), neruons.end(), 0);

				for(int f = 0; f < neruons.size(); ++f) {
					neruons[f] /= sum;
				}
			}
		}
	}
}
