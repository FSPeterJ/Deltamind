//
//#include "NeuralNetwork.h"
//#include "TrainingData.h"
//#include <ctime>
//#include "Console.h"
//#include "RandomEngine.h"
//#include <algorithm>
//
//#include "PythonInterface.h"
//
//using namespace GhostMind;
//void CreateTrainingDataForXor(const char* file, std::size_t inputSize, std::size_t outputSize, std::size_t iterations) {
//	//when setting up training data, scale output so the output values are always within the range of what the activation function is able to make
//	srand((unsigned) time(NULL));
//
//	auto tw = TrainingDataWriter<int, int>(file);
//
//	int x, y;
//
//	tw.SetInputFunction([&]()->std::vector<int> {
//		x = (int) (2.0 * rand() / double(RAND_MAX));
//		y = (int) (2.0 * rand() / double(RAND_MAX));
//		return {x,y};
//	});
//
//	tw.SetTargetFunction([&]()->std::vector<int> { return {x^y}; });
//
//	tw.CreateData(inputSize, outputSize, iterations);
//}
//
//void CreateTrainSaveXorTest() {
//	NeuralNetwork n({2,4,1});
//	CreateTrainingDataForXor(TRAIN_FILE("xorTrainingData"), n.GetInputCount(), n.GetOutputCount(), 20000);
//
//	n.Train(TRAIN_FILE("xorTrainingData"));
//
//	n.Save(GHOSTNET_FILE("xorNeuralNetwork"));
//}
//
//void LoadTestXorTest() {
//	NeuralNetwork n(GHOSTNET_FILE("xorNeuralNetwork"));
//
//	std::vector<double> outputs;
//
//	n.ProcessInput({1,0}, outputs);
//	Console::WriteLine << "1^0 = " << std::floor(outputs[0] + 0.5);
//
//	n.ProcessInput({0,0}, outputs);
//	Console::WriteLine << "0^0 = " << std::floor(outputs[0] + 0.5);
//
//	n.ProcessInput({0,1}, outputs);
//	Console::WriteLine << "0^1 = " << std::floor(outputs[0] + 0.5);
//
//	n.ProcessInput({1,1}, outputs);
//	Console::WriteLine << "1^1 = " << std::floor(outputs[0] + 0.5);
//
//	Console::WriteLine << "Done!";
//}
//
//void CreateTrainingDataForAddition(const char* file, std::size_t inputSize, std::size_t outputSize, std::size_t iterations) {
//	srand((unsigned) time(NULL));
//
//	auto tw = TrainingDataWriter<int, int>(file);
//
//	int x, y;
//
//	std::vector<int> createdNums({0,0,0,0,0,0,0,0,0});
//
//	tw.SetInputFunction([&]()->std::vector<int> {
//		//int minpos = std::distance(createdNums.begin(), std::min_element(createdNums.begin(), createdNums.end()));
//		//int index = 0;
//		//do {
//		x = GhostRand::RandomIntClosedMax(1, 5);
//		y = GhostRand::RandomIntClosedMax(1, 5);
//		//	index = x + y - 2;
//		//} while(index != minpos);
//
//		//++createdNums[index];
//
//		return {x,y};
//	});
//
//	int nums[9] = {0,0,0,0,0,0,0,0,0};
//	//int total[9] = {0,0,0,0,0,0,0,0,0};
//	int prevIndex = 0;
//	tw.SetTargetFunction([&]()->std::vector<int> {
//		nums[prevIndex] = 0;
//		prevIndex = x + y - 2;
//		//	++total[prevIndex];
//		nums[prevIndex] = 1;
//		return std::vector<int>(std::begin(nums), std::end(nums));
//	});
//
//	tw.CreateData(inputSize, outputSize, iterations);
//
//	//Console::Write << "Total: ";
//	//for(int i = 0; i < 9; ++i) {
//	//	Console::Write << total[i] << " ";
//	//}
//	//Console::WriteLine << "";
//
//}
//
//void randomlayers(std::vector<std::size_t>&layerNeuronCounts) {
//	layerNeuronCounts.clear();
//
//	Neuron::trainingRate = GhostRand::RandomDoubleClosedMax(0.0, 1.0);
//	Neuron::momentum = GhostRand::RandomDoubleClosedMax(0.0, 2.0);
//
//	int layers = GhostRand::RandomIntClosedMax(1, 5);
//
//	for(int i = 0; i < layers; ++i) {
//		int randMin = GhostRand::RandomIntClosedMax(1, 10);
//		int randMax = GhostRand::RandomIntClosedMax(11, 15);
//
//		int n = GhostRand::RandomIntClosedMax(randMin, randMax);
//
//		layerNeuronCounts.push_back(n);
//	}
//	layerNeuronCounts.insert(layerNeuronCounts.begin(), {2});
//	layerNeuronCounts.insert(layerNeuronCounts.end(), {9});
//}
//
//void CreateTrainSaveAddTest() {
//
//	Console::WriteLine << "Creating Network...";
//	NeuralNetwork n({2,9,9,9});
//
//	Console::WriteLine << "Creating Training Data...";
//	CreateTrainingDataForAddition("addTrainingData.train", n.GetInputCount(), n.GetOutputCount(), 100000);
//
//	Console::WriteLine << "Training...";
//	n.Train("addTrainingData.train");
//
//	Console::WriteLine << "Saving...";
//	n.Save("addNeuralNetwork.ghostnet");
//
//	Console::WriteLine << "Done";
//}
//
//void LoadTestAddTest() {
//	NeuralNetwork n("addNeuralNetwork.ghostnet");
//
//	std::vector<double> outputs;
//	ptrdiff_t pos;
//
//	double accuracy = 0.0;
//	int correct = 0;
//	int total = 0;
//
//	for(int lhs = 1; lhs < 6; ++lhs) {
//		for(int rhs = 1; rhs < 6; ++rhs) {
//			n.ProcessInput({double(lhs),double(rhs)}, outputs);
//			pos = std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end())) + 2;
//			Console::WriteLine << lhs << " + " << rhs << " = " << pos;
//			if(pos == (lhs + rhs)) { ++correct; }
//			++total;
//		}
//	}
//
//	accuracy = (double) correct / (double) total;
//	Console::WriteLine << "I got " << correct << " right out of " << total << ". That's " << accuracy << "%";
//
//	Console::WriteLine << "Done!";
//}
//
//struct NetworkResult {
//	double acc = 0.0;
//	double mom, rate;
//	std::vector<std::size_t> structure;
//};
//NetworkResult GetBestStructure() {
//
//	std::vector<std::size_t> layerNeuronCounts;
//	randomlayers(layerNeuronCounts);
//
//	NeuralNetwork n(layerNeuronCounts);
//
//	CreateTrainingDataForAddition("addTrainingData.train", n.GetInputCount(), n.GetOutputCount(), 50000);
//
//	n.Train("addTrainingData.train");
//
//	//n.Save("addNeuralNetwork.ghostnet");
//
//	//===========================
//
//	std::vector<double> outputs;
//	ptrdiff_t pos;
//
//	double accuracy = 0.0;
//	int correct = 0;
//	int total = 0;
//
//	for(int lhs = 1; lhs < 6; ++lhs) {
//		for(int rhs = 1; rhs < 6; ++rhs) {
//			n.ProcessInput({double(lhs),double(rhs)}, outputs);
//			pos = std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end())) + 2;
//			//	Console::WriteLine << lhs << " + " << rhs << " = " << pos;
//			if(pos == (lhs + rhs)) { ++correct; }
//			++total;
//		}
//	}
//
//	accuracy = (double) correct / (double) total;
//	Console::WriteLine << "I got " << correct << " right out of " << total << ". That's " << accuracy << "%";
//	Console::Write << "I used a structure of ";
//	for(size_t i = 0; i < layerNeuronCounts.size(); ++i) {
//		Console::Write << layerNeuronCounts[i] << " ";
//	}
//	Console::WriteLine << "";
//
//	NetworkResult res;
//	res.acc = accuracy;
//	res.structure = layerNeuronCounts;
//	res.mom = Neuron::momentum;
//	res.rate = Neuron::trainingRate;
//
//	Console::WriteLine << "Eta: " << res.rate << "  Alpha: " << res.mom;
//
//
//	return res;
//}
//
//void findBestNet() {
//	NetworkResult bestNet;
//	int c = 0;
//	while(bestNet.acc < 0.75) {
//		Console::WriteLine << "Network " << ++c << ":";
//		NetworkResult net = GetBestStructure();
//		if(net.acc < bestNet.acc) {
//			Console::WriteLine << "New best!";
//			bestNet = net;
//		}
//	}
//
//
//	Console::WriteLine << "The best net had an accuracy of " << bestNet.acc;
//	Console::Write << "Its structure was : ";
//	for(size_t i = 0; i < bestNet.structure.size(); ++i) {
//		Console::Write << bestNet.structure[i] << " ";
//	}
//	Console::WriteLine << "";
//
//}
//#include <cassert>
//int nettest() {
//	//Omiracron
//
//	//CreateTrainSaveXorTest();
//	//LoadTestXorTest();
//
//	//CreateTrainSaveAddTest();
//	//LoadTestAddTest();
//
//	PythonInterface python;
//	python.ExecuteFile("Test", "HelloWorld");
//	python.ExecuteFile("Test", "TensorFlow");
//
//	//python.ExecuteFile("zhou", "A3C_RNN");
//
//	return 0;
//}
