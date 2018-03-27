#pragma once

namespace Omiracon {
	namespace Neural {
		typedef double(*ActivationFunction)(double);
		namespace ActivationFunctions {
			enum class ActivationType: unsigned char {
				Sigmoid = 0,
				Tanh,
				ReLu,
				LeakyReLu,
			};

			double Sigmoid(double x);
			double SigmoidDerivative(double x);
			double HyperbolicTangent(double x);
			double HyperbolicTangentDerivative(double x);
			double RectifiedLinearUnit(double x);
			double RectifiedLinearUnitDerivative(double x);
			double LeakyRectifiedLinearUnit(double x);
			double LeakyRectifiedLinearUnitDerivative(double x);

			//	void Softmax(std::vector<double>& neuronOutputs);
		}
	}
}
