#include "stdafx.h"
#include "Neuron.h"
#include "BinIO.h"
#include "Console.h"
#include "RandomEngine.h"

namespace GhostMind {

	Neuron::Eta Neuron::trainingRate = 0.15;
	Neuron::Alpha Neuron::momentum = 0.5;

	double Neuron::RandomWeight(void) {
		return GhostRand::RandomDoubleClosedMax(0.0, 1.0);
		//return rand() / double(RAND_MAX); //[0..1]
	}

	double Neuron::sumDOW(const Layer & nextLayer) const {
		//sum errors that we pass on
		double sumDerivativeNextLayer = 0.0;

		for(std::size_t n = 0; n < nextLayer.size() - 1; ++n) {
			sumDerivativeNextLayer += connections[n].weight * nextLayer[n].gradient;
		}

		return sumDerivativeNextLayer;
	}

	Neuron::Neuron(const std::size_t numOutputs, const std::size_t index) {
		//activationType = (unsigned char)GhostRand::RandomIntClosedMax(0, 3);
		SetActivationFunction(*this, static_cast<ActivationFunctions::ActivationType>(activationType));

		for(std::size_t connection = 0; connection < numOutputs; ++connection) {
			connections.push_back(Connection());
			connections.back().weight = RandomWeight();
		}
		myIndex = index;
	}

	void Neuron::LoadNeuron(std::size_t numOutputs, std::size_t index) {
		activationType = Reader::Read<unsigned char>();
		SetActivationFunction(*this, static_cast<ActivationFunctions::ActivationType>(activationType));

		output = Reader::ReadDouble();
		gradient = Reader::ReadDouble();
		myIndex = index;
		connections.resize(numOutputs);

		for(std::size_t o = 0; o < numOutputs; ++o) {
			Connection c;
			c.weight = Reader::ReadDouble();
			c.deltaWeight = Reader::ReadDouble();
			connections[o] = c;
		}
	}
	void Neuron::SaveNeuron() {
		Writer::Write(activationType);
		Writer::Write(output);
		Writer::Write(gradient);

		for(std::size_t o = 0; o < connections.size(); ++o) {
			Writer::Write(connections[o].weight);
			Writer::Write(connections[o].deltaWeight);
		}
	}

	void Neuron::FeedForward(const Layer & prevLayer) {
		double sumPrevLayer = 0.0;

		for(std::size_t n = 0; n < prevLayer.size(); ++n) {
			sumPrevLayer += prevLayer[n].GetOutputValue() * prevLayer[n].connections[myIndex].weight;
		}

		output = Activation(sumPrevLayer);
	}

	void Neuron::CalculateOutputGradient(const double target) {
		//can experiment with different gradient calculations
		gradient = (target - output) * ActivationDerivative(output);
	}

	void Neuron::CalculateHiddenGradient(const Layer & nextLayer) {
		//doesn't have a target value so we will approx
		gradient = sumDOW(nextLayer) * ActivationDerivative(output);
	}

	void Neuron::UpdateInputWeights(Layer & prevLayer) {
		//includes bias
		for(std::size_t n = 0; n < prevLayer.size(); ++n) {
			Neuron &neuron = prevLayer[n];
			double oldDeltaWeight = neuron.connections[myIndex].deltaWeight;

			double newDeltaWeight = trainingRate * neuron.GetOutputValue() * gradient + momentum * oldDeltaWeight;

			neuron.connections[myIndex].deltaWeight = newDeltaWeight;
			neuron.connections[myIndex].weight += newDeltaWeight;
		}
	}

	void Neuron::SetActivationFunction(Neuron & n, const ActivationFunctions::ActivationType type) {
		n.activationType = static_cast<std::underlying_type_t<ActivationFunctions::ActivationType>>(type);
		switch(type) {
		case ActivationFunctions::ActivationType::Sigmoid:
			n.Activation = ActivationFunctions::Sigmoid;
			n.ActivationDerivative = ActivationFunctions::SigmoidDerivative;
			break;
		case ActivationFunctions::ActivationType::Tanh:
			n.Activation = ActivationFunctions::HyperbolicTangent;
			n.ActivationDerivative = ActivationFunctions::HyperbolicTangentDerivative;
			break;
		case ActivationFunctions::ActivationType::ReLu:
			n.Activation = ActivationFunctions::RectifiedLinearUnit;
			n.ActivationDerivative = ActivationFunctions::RectifiedLinearUnitDerivative;
			break;
		case ActivationFunctions::ActivationType::LeakyReLu:
			n.Activation = ActivationFunctions::LeakyRectifiedLinearUnit;
			n.ActivationDerivative = ActivationFunctions::LeakyRectifiedLinearUnitDerivative;
			break;
		default:// Console::ErrorLine << "Could not find specified activation function. Did you forget to add it to the switch?";
			break;
		}
	}
}
