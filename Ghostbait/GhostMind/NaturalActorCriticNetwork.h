#pragma once
#include "NeuralNetwork.h"
#include <vector>
using namespace ActivationFunctions;

namespace GhostMind {
	class NaturalActorCriticNetwork: public NeuralNetwork {

		template<typename T>
		struct Tensor {
			std::vector<std::vector<T>> data;

			Tensor(std::vector<T> nd) {

			}

			Tensor view(int a, int b) {

			}

			void clear() {

			}

			//Returns a new tensor with a dimension of size one inserted at the specified position.
			//The returned tensor shares the same underlying data with this tensor.
			//A negative dim value within the range[-input.dim(), input.dim()) can be used and will correspond to unsqueeze() applied at dim = dim + input.dim() + 1
			//int dim - the index at which to insert the singleton dimension 
			Tensor unsqueeze(int dim) {

			}

			std::vector<T> back() {
				return data.back();
			}


		};


		struct Variable {
			template <typename T>
			Variable(Tensor<T> t) {

			}

			Variable unsqueeze(int dim) {

			}
		};

		enum Action {
			Wander,
			AttackTurret,
			AttackPlayer,
			AttackCore,
			Evade,
			COUNT,
		};

		void EvaluateActions(std::vector<double>& actionProbs, std::vector<double>& stateValues, Ghostrix x) {
			Softmax(actionProbs);
			stateValues = self.critic(x);
		}

		//actor
		void getActionProbabilities(std::vector<double>& actionProbs, Ghostrix x) {
			Softmax(actionProbs);
		}

		//critic
		Tensor<float> GetStateValue(Variable x) {
			return self.critic(x);
		}

		double discount = 0.95; //gamma
		double learningRate = 3e-3;

		std::size_t steps = 5;

		std::size_t inputs = 6;//enemies near me, turrets near me, distannce to player, distance to core, player health, core health
		std::size_t actionCount = Action::COUNT;

		 std::vector<Action> actions;
		 Tensor<float> states;
		 std::vector<Action> rewards;
		 std::vector<Action> dones;

		float next_return;
		float this_return;

		int finished_games = 0;

		Action Multinomial(std::vector<double>& action_probs) {
			return action_probs.multinomial().data[0][0];
		}

		void GatherData() {
			std::vector<double> action_probs;
			for(int i = 0; i < steps; ++i) {
				Ghostrix s = Variable(torch.from_numpy(state).float().unsqueeze(0));

				getActionProbabilities(action_probs, s);
				Action action = Multinomial(action_probs);
				next_state, reward, done, _ = env.step(action);

				states.push_back(state);
				actions.push_back(action);
				rewards.push_back(reward);
				dones.push_back(done);

				if(done) {
					state = env.reset();
					finished_games += 1;
				} else {
					state = next_state;
				}
			}

			Reflect(states, actions, rewards, dones);
		}

		Variable CalculateActualStateValues( ) {
			std::vector<float> R;
			std::reverse(rewards.begin(), rewards.end());

			//If we happen to end the set on a terminal state, set next return to zero
			if(dones.back() == true) {
				next_return = 0;
			}

			//If not terminal state, bootstrap v(s) using our critic
			//TODO: don't need to estimate again, just take from last value of v(s) estimates
			else {
				Tensor<float> s = Tensor<float>(states.back()).unsqueeze(0);
				next_return = GetStateValue(Variable(s)).data[0][0];
			}

			//Backup from last state to calculate "true" returns for each state in the set
			R.push_back(next_return);
			std::reverse(dones.begin(), dones.end());

			for(std::size_t r = 0; r < rewards.size(); ++r) {
				if(!dones[r]) {
					this_return = rewards[r] + next_return * discount;
				} else {
					this_return = 0;
					R.push_back(this_return);
					next_return = this_return;
				}
			}

			std::reverse(R.begin(), R.end());

			return Variable(Tensor<float>(R)).unsqueeze(1);
		}

		void Reflect() {
			//Calculating the ground truth "labels" as described above
			std::vector<double> state_values_true = CalculateActualStateValues();

			std::vector<double> action_probs;
			std::vector<double> state_values_est;
			std::vector<double> action_log_probs;

			Variable s = Variable(torch.FloatTensor(states));
			EvaluateActions(action_probs, state_values_est, s);

			action_log_probs.resize(action_probs.size());
			for(int i = 0; i < action_probs.size(); ++i) {
				action_log_probs[i] = log(action_probs[i]);
			}

			Variable a = Variable(Tensor<long>(actions).view(-1, 1));
			std::vector<float> chosen_action_log_probs = action_log_probs.gather(1, a);

			//This is also the TD error
			std::vector<float> advantages;
			advantages.resize(state_values_true.size());
			for(int i = 0; i < state_values_true.size(); ++i) {
				advantages[i] = state_values_true[i] - state_values_est[i];
			}

			float entropySum = 0;
			for(int i = 1; i < action_probs.size(); ++i) {
				entropySum += action_probs[i] * action_log_probs[i];
			}

			float action_gainSum = 0;
			for(int i = 0; i < chosen_action_log_probs.size(); ++i) {
				action_gainSum += chosen_action_log_probs[i] * advantages[i];
			}

			float value_lossSum = 0;
			for(int i = 0; i < advantages.size(); ++i) {
				value_lossSum += advantages[i] * advantages[i];
			}

			float entropy = entropySum / action_probs.size() - 1;
			float action_gain = action_gainSum / chosen_action_log_probs.size();
			float value_loss = value_lossSum / advantages.size();
			float total_loss = value_loss - action_gain - 0.0001*entropy;

			optimizer.zero_grad();
			total_loss.backward();
			nn.utils.clip_grad_norm(model.parameters(), 0.5);
			optimizer.step();
		}

		void Train() {
			rewards.clear();
			dones.clear();
			states.clear();
			actions.clear();

			GatherData();

		}

		void Update() {
			if(finished_games == 5) {
				state = env.reset();
				finished_games = 0;
			}

			Train();
		}
	};
}
