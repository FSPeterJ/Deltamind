#pragma once
#include "NeuralNetwork.h"
#include <vector>
#include <cassert>
#include "RandomEngine.h"


namespace Omiracon {
	namespace Neural {
		using namespace ActivationFunctions;
		struct Variable;

		template<typename T>
		struct Tensor {
			T* _data;

			std::size_t rows = 0, cols = 0;

			Tensor() {}

			Tensor(std::vector<T> nd) {
				rows = 1;
				cols = nd.size();
				_data = new T[cols];
				memcpy(_data, &nd[0], cols);
			}

			~Tensor() {
				delete[] _data;
			}

			Tensor(const std::size_t r, const std::size_t c) {
				_data = new T[r*c];
				rows = r;
				cols = c;
			}

			//Returns the sum of all elements in the tensor.
			T sum() const {
				T result = 0;
				for(int i = 0, len = rows * cols; i < len; ++i) {
					result += _data[i];
				}
				return result;
			}

			//Returns the sum of each row of the input tensor in the given dimension dim.
			//If keepdim is True, the output tensor is of the same size as input except in the dimension dim where it is of size 1.
			//Otherwise, dim is squeezed(see torch.squeeze()), resulting in the output tensor having 1 fewer dimension than input.
			//dim(int) – the dimension to reduce
			//keepdim(bool) – whether the output tensor has dim retained or not
			Tensor sumRow() const {
				Tensor result(rows, 1);

				for(int r = 0; r < rows; ++r) {
					for(int c = 0, i = r * cols; c < cols; ++c) {
						result[r] += _data[i + c];
					}
				}

				return result;
			}

			//Returns the mean value of all elements in the input tensor.
			T mean() const {
				return sum() / T((rows * cols));
			}

			//Takes the power of each element in input with exponent and returns a tensor with the result.
			//exponent can be either a single float number or a Tensor with the same number of elements as input.
			//When exponent is a scalar value, the operation applied is :
			//outi = xexponenti
			//When exponent is a tensor, the operation applied is :
			//outi = xexponentii
			//When exponent is a tensor, the shapes of input and exponent must be broadcastable.
			//Parameters:
			//exponent(float) – the exponent value
			Tensor square() const {
				Tensor result(rows, cols);
				for(int i = 0, len = rows * cols; i < len; ++i) {
					result[i] = _data[i] * _data[i];
				}
				return result;
			}

			Tensor log() const {
				Tensor result(rows, cols);
				for(int i = 0, len = rows * cols; i < len; ++i) {
					result[i] = log(_data[i]);
				}
				return result;
			}

			T multinomial() {
				double r = RandomDoubleClosedMax(0.0, 1.0);
				std::size_t index = 0;
				std::size_t length = rows * cols;

				while(r >= 0.0 && index < length) {
					r -= _data[index];
					++index;
				}

				return _data[index - 1];
			}

			//Gathers values along an axis specified by dim.
			//For a 3 - D tensor the output is specified by :
			//
			//out[i][j][k] = input[index[i][j][k]][j][k]  # if dim == 0
			//out[i][j][k] = input[i][index[i][j][k]][k]  # if dim == 1
			//out[i][j][k] = input[i][j][index[i][j][k]]  # if dim == 2
			//If input is an n - dimensional tensor with size(x0, x1..., xi−1, xi, xi + 1, ..., xn−1) and dim = i, then index must be an n - dimensional tensor with size(x0, x1, ..., xi−1, y, xi + 1, ..., xn−1) where y≥1 and out will have the same size as index.
			//
			//Parameters:
			//dim(int) – the axis along which to index
			//index(LongTensor) – the indices of elements to gather
			//out(Tensor, optional) – the destination tensor
			Tensor gather(Variable index) const {
				Tensor result(rows, cols);

				for(int r = 0, j; r < rows; ++r) {
					for(int c = 0, i = r * cols; c < cols ++c) {
						j = index[i + c];
						result[i + c] = _data[i + j];
					}
				}

				return result;
			}

			Tensor squash() {
				row = (rows*cols);
				cols = 1;
			}

			//Returns a new tensor with a dimension of size one inserted at the specified position.
			//The returned tensor shares the same underlying data with this tensor.
			//A negative dim value within the range[-input.dim(), input.dim()) can be used and will correspond to unsqueeze() applied at dim = dim + input.dim() + 1
			//int dim - the index at which to insert the singleton dimension
			Tensor transpose() {
				rows ^= cols;
				cols ^= rows;
				rows ^= cols;
			}
		};

		template<typename T>
		Tensor<T> operator*(const Tensor<T>& lhs, const Tensor<T>& rhs) const {
			assert(lhs.rows*lhs.cols == rhs.rows * rhs.cols);

			Tensor<T> result(lhs.rows, lhs.cols);

			for(int i = 0, len = lhs.rows * lhs.cols; i < len; ++i) {
				result._data[i] = lhs._data[i] * rhs._data[i];
			}

			return result;
		}

		template<typename T>
		Tensor<T> operator-(const Tensor<T>& lhs, const Tensor<T>& rhs) const {
			assert(lhs.rows*lhs.cols == rhs.rows * rhs.cols);

			Tensor<T> result(lhs.rows, lhs.cols);

			for(int i = 0, len = lhs.rows * lhs.cols; i < len; ++i) {
				result._data[i] = lhs._data[i] - rhs._data[i];
			}

			return result;
		}

		struct Variable {
			template <typename T>
			Variable(Tensor<T> t) {}

			void transpose() {}
		};

		enum Action {
			Wander,
			AttackTurret,
			AttackPlayer,
			AttackCore,
			Evade,
			COUNT,
		};

		typedef std::vector<float> State;
		struct Environment {
			void Step(Action action, State next_state, float reward, bool done) {}

			State Reset() {}
		};

		struct Optimizer {
			void ZeroGrad(std::vector<float>& parameters) {
				for(auto& p : parameters) {
					//if(p.grad) {
						//p.grad.data.zero_()
						//p.grad = 0;
					//}
				}
			}

			void Step() {}
		};
		class NaturalActorCriticNetwork: public NeuralNetwork {
			//	Environment environment;
			//	Optimizer optimizer;
			//
			//	//Clips gradient norm of an iterable of parameters.
				//The norm is computed over all gradients together, as if they were
				//concatenated into a single vector. Gradients are modified in place.
				//Arguments:
				//parameters(Iterable[Variable]) : an iterable of Variables that will have gradients normalized
				//max_norm(float or int) : max norm of the gradients
				//norm_type(float or int) : type of the used p-norm. Can be 'inf' for infinity norm.
				//Returns:
				//Total norm of the parameters(viewed as a single vector).
				//parameters = list(filter(lambda p : p.grad is not None, parameters))
			//	float ClipGradNorm(std::vector<float>& parameters, float max_norm, float norm_type = 2) {
			//		max_norm = float(max_norm);
			//		norm_type = float(norm_type);
			//
			//		float total_norm = 0;
			//		float param_norm = 0;
			//
			//		float clip_coef = 0;
			//
			//		if(norm_type == float('inf')) {
			//			//total_norm = max(p.grad.data.abs().max() for p in parameters)
			//		} else {
			//			total_norm = 0;
			//			for(auto &p : parameters) {
			//				//param_norm = p.grad.data.norm(norm_type)
			//				total_norm += pow(param_norm, norm_type);
			//			}
			//			total_norm = pow(total_norm, (1.0 / norm_type));
			//		}
			//		clip_coef = max_norm / (total_norm + 1e-6);
			//
			//		if(clip_coef < 1) {
			//			for(auto &p : parameters) {
			//				//p.grad.data.mul_(clip_coef)
			//			}
			//		}
			//
			//		return total_norm;
			//	}
			//
			//	void EvaluateActions(Tensor<float>& actionProbs, Tensor<float>& stateValues, Variable x) {
			//		Softmax(actionProbs);
			//		stateValues = self.critic(x);
			//	}
			//
			//	//actor
			//	void getActionProbabilities(Tensor<float>& actionProbs, Variable x) {
			//		Softmax(actionProbs);
			//	}
			//
			//	//critic
			//	Tensor<float> GetStateValue(Variable x) {
			//		return self.critic(x);
			//	}
			//
			//	double discount = 0.95; //gamma
			//	double learningRate = 3e-3;
			//
			//	std::size_t steps = 5;
			//
			//	std::size_t inputs = 6;//enemies near me, turrets near me, distannce to player, distance to core, player health, core health, avg turrent dmg, my enemy type
			//	std::size_t actionCount = Action::COUNT;
			//
			//	std::vector<Action> actions;
			//	std::vector<State> states;
			//	std::vector<float> rewards;
			//	std::vector<bool> dones;
			//
			//	State state;
			//
			//	float next_return;
			//	float this_return;
			//
			//	int finished_games = 0;
			//
			//	void GatherData() {
			//		Tensor<Action> action_probs;
			//		for(int i = 0; i < steps; ++i) {
			//			Variable s = Variable(Tensor<float>(state));
			//
			//			getActionProbabilities(action_probs, s);
			//			Action action = action_probs.multinomial();
			//
			//			State next_state;
			//			float reward = 0;
			//			bool done = false;
			//			environment.Step(action, next_state, reward, &done);
			//
			//			states.push_back(state);
			//			actions.push_back(action);
			//			rewards.push_back(reward);
			//			dones.push_back(done);
			//
			//			if(done) {
			//				state = environment.Reset();
			//				finished_games += 1;
			//			} else {
			//				state = next_state;
			//			}
			//		}
			//
			//		Reflect(states, actions, rewards, dones);
			//	}
			//
			//	Tensor<float> CalculateActualStateValues() {
			//		std::vector<float> R;
			//		std::reverse(rewards.begin(), rewards.end());
			//
			//		//If we happen to end the set on a terminal state, set next return to zero
			//		if(dones.back() == true) {
			//			next_return = 0;
			//		}
			//		//If not terminal state, bootstrap v(s) using our critic
			//		//TODO: don't need to estimate again, just take from last value of v(s) estimates
			//		else {
			//			Tensor<float> s = Tensor<float>(states.back());
			//			next_return = GetStateValue(Variable(s))._data[0];
			//		}
			//
			//		//Backup from last state to calculate "true" returns for each state in the set
			//		R.push_back(next_return);
			//		std::reverse(dones.begin(), dones.end());
			//
			//		for(std::size_t r = 0; r < rewards.size(); ++r) {
			//			if(!dones[r]) {
			//				this_return = rewards[r] + next_return * discount;
			//			} else {
			//				this_return = 0;
			//			}
			//			R.push_back(this_return);
			//			next_return = this_return;
			//		}
			//
			//		std::reverse(R.begin(), R.end());
			//
			//		Variable state_values_true = Variable(Tensor<float>(R));
			//		return state_values_true.transpose();
			//	}
			//
			//	void Reflect() {
			//		//Calculating the ground truth "labels" as described above
			//		Tensor<float> state_values_true = CalculateActualStateValues();
			//
			//		Tensor<float> action_probs;
			//		Tensor<float> state_values_est;
			//		Tensor<float> action_log_probs;
			//
			//		Variable s = Variable(states);
			//		EvaluateActions(action_probs, state_values_est, s);
			//
			//		action_log_probs = action_probs.log();
			//
			//		Variable a = Variable(Tensor<Action>(actions).squash());
			//		Tensor<float> chosen_action_log_probs = action_log_probs.gather(a);
			//
			//		//This is also the TD error
			//		Tensor<float> advantages = state_values_true - state_values_est;
			//
			//		float entropy = (action_probs * action_log_probs).sumRow().mean();
			//		float action_gain = (chosen_action_log_probs * advantages).mean();
			//		float value_loss = advantages.square().mean();
			//		float total_loss = value_loss - action_gain - 0.0001 * entropy;
			//
			//		optimizer.ZeroGrad();
			//		total_loss.backward();
			//		ClipGradNorm(model.parameters(), 0.5);
			//		optimizer.Step();
			//	}
			//
			//	void Train() {
			//		rewards.clear();
			//		dones.clear();
			//		states.clear();
			//		actions.clear();
			//
			//		GatherData();
			//	}
			//
			//	void Update() {
			//		if(finished_games == 5) {
			//			state = environment.Reset();
			//			finished_games = 0;
			//		}
			//
			//		Train();
			//	}
		};
	}
}
