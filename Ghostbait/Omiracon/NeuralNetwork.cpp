#include "stdafx.h"
#include "NeuralNetwork.h"
#include "TrainingData.h"
#include <cassert>
#include "BinIO.h"
#include "Console.h"

namespace GhostMind {
	void NeuralNetwork::FeedForward(const std::vector<double>& inputValues) {
		assert(inputValues.size() == network[0].size() - 1); //-1 for bias neuron

		for(std::size_t i = 0; i < inputValues.size(); ++i) {
			network[0][i].SetOutputValue(inputValues[i]);
		}

		//propagate forward
		for(std::size_t layerNum = 1; layerNum < network.size(); ++layerNum) {
			Layer &prevLayer = network[layerNum - 1]; //bias neuron
			for(unsigned n = 0; n < network[layerNum].size() - 1; ++n) {
				network[layerNum][n].FeedForward(prevLayer);
			}
		}
	}

	void NeuralNetwork::BackPropagate(const std::vector<double>& targetValues) {
		Layer &outputLayer = network.back();

		error = 0.0;
		//can experiment with different error calculations
		//calculates error from rms of output neuron errors, not including bias
		for(std::size_t n = 0; n < outputLayer.size() - 1; ++n) {
			double delta = targetValues[n] - outputLayer[n].GetOutputValue();
			error += delta * delta;
		}

		error /= outputLayer.size() - 1;
		error = sqrt(error); //rms

		CalculateMeanError(error);

		//no bias
		for(std::size_t n = 0; n < outputLayer.size() - 1; ++n) {
			outputLayer[n].CalculateOutputGradient(targetValues[n]);
		}

		//right most hidden layer
		for(std::size_t layerNum = network.size() - 2; layerNum > 0; --layerNum) {
			Layer &hiddenLayer = network[layerNum];
			Layer &nextLayer = network[layerNum + 1];

			for(std::size_t n = 0; n < hiddenLayer.size(); ++n) {
				hiddenLayer[n].CalculateHiddenGradient(nextLayer);
			}
		}

		//no input
		for(std::size_t layerNum = network.size() - 1; layerNum > 0; --layerNum) {
			Layer&layer = network[layerNum];
			Layer &prevLayer = network[layerNum - 1];

			for(std::size_t n = 0; n < layer.size() - 1; ++n) {
				layer[n].UpdateInputWeights(prevLayer);
			}
		}
	}

	void NeuralNetwork::GetOutput(std::vector<double>& outputValues) {
		outputValues.clear();

		for(std::size_t n = 0; n < network.back().size() - 1; ++n) {
			outputValues.push_back(network.back()[n].GetOutputValue());
		}
	}

	NeuralNetwork::NeuralNetwork(const std::vector<std::size_t> &netStructure) : structure(netStructure) {
		std::size_t layerSize = netStructure.size();
		for(std::size_t layerNum = 0; layerNum < layerSize; ++layerNum) {
			network.push_back(Layer());

			std::size_t numOutputs = layerNum == netStructure.size() - 1 ? 0 : netStructure[layerNum + 1]; //if output layer, no outputs

																										   //I'm using <= to add a bias neuron at each layer
			for(std::size_t n = 0; n <= netStructure[layerNum]; ++n) {
				network.back().push_back(Neuron(numOutputs, n));
			}
			network.back().back().SetOutputValue(1.0); //bias //-1
		}
	}

	NeuralNetwork::NeuralNetwork(const char* file) {
		Load(file);
	}

	void NeuralNetwork::Save(const char * file) {
		Writer::CreateStream(file);

		Writer::Write(structure.size());
		for(std::size_t s = 0; s < structure.size(); ++s) { Writer::Write(structure[s]); }

		Writer::Write(network.size());
		for(std::size_t layerNum = 0; layerNum < network.size(); ++layerNum) {
			Layer& layer = network[layerNum];

			for(std::size_t n = 0; n <= structure[layerNum]; ++n) { layer[n].SaveNeuron(); }
		}
		Writer::Write(error);
		Writer::CloseStream();
	}

	void NeuralNetwork::ProcessInput(const std::vector<double>& inputValues, std::vector<double>& output) {
		FeedForward(inputValues);
		GetOutput(output);
	}

	void NeuralNetwork::Train(const char * trainDataFile) {
		//when setting up training data, scale output so the output values are always within the range of what the activation function is able to make
		auto trainData = TrainingDataReader<int, int>(trainDataFile);

		std::size_t trainingPass = 0;
		std::size_t iterations = trainData.GetIterations();

		std::vector<double> inputs, outputs, targets;

		while(trainingPass < iterations) {
			trainData.GetInputs(inputs);
			ProcessInput(inputs, outputs);
			trainData.GetTargets(targets);
			BackPropagate(targets);
			++trainingPass;
		}
	}

	void NeuralNetwork::Load(const char* file) {
		Reader::CreateStream(file);

		std::size_t structureSize = Reader::ReadInt();
		structure.resize(structureSize);
		for(std::size_t s = 0; s < structureSize; ++s) {
			structure[s] = Reader::ReadInt();
		}

		std::size_t layersSize = Reader::ReadInt();
		network.resize(layersSize);

		for(std::size_t layerNum = 0; layerNum < layersSize; ++layerNum) {
			Layer myLayer = Layer();

			std::size_t numOutputs = layerNum == structureSize - 1 ? 0 : structure[layerNum + 1];
			myLayer.resize(structure[layerNum] + 1);

			for(std::size_t n = 0; n <= structure[layerNum]; ++n) {
				Neuron myNeuron = Neuron();
				myNeuron.LoadNeuron(numOutputs, n);
				myLayer[n] = myNeuron;
			}
			network[layerNum] = myLayer;
		}

		error = Reader::ReadDouble();
	}
}
