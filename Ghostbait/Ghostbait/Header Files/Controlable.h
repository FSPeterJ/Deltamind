#pragma once
#include <functional>		//can probably be moved to header if nested classes move
#include <unordered_map>	//can probably be moved to header if nested classes move
#include "MessageStructs.h"	//can probably be moved to header if nested classes move

//This becomes some sort of component add-on?
class Controlable {
#pragma region Handler
	//dont touch dis
	class InputReceivedEvent {
		//seriously dont
		using ReceiveEvent = std::function<void(EventMessageBase*)>;
		struct InputReceiver: std::unordered_map<Control, float> {
			//its for your own good
			void OnInputReceived(EventMessageBase* e) {
				InputMessage* inputMsg = (InputMessage*) e;
				this->operator[](inputMsg->GetControl()) = inputMsg->GetAmount();
			}
		}*receiver;
		ReceiveEvent ReceiveInput;
	public:
		InputReceiver * const GetState() const { return this->receiver; }
		void SetState(const Control key, const float amount) { this->receiver->operator[](key) = amount; }
		ReceiveEvent const ReceiveInputEvent() const { return ReceiveInput; }
		InputReceivedEvent() : receiver(new InputReceiver()), ReceiveInput([receiver = receiver](EventMessageBase* e) mutable { receiver->OnInputReceived(e); }) {}
		virtual ~InputReceivedEvent() {
			delete receiver;
		}
	}inputReceivedEvent;
#pragma endregion
protected:
	/// <summary>
	/// Gets the amount the specified key is down.
	/// </summary>
	/// <param name="key">The key.</param>
	/// <returns>The amount the specified key is down.</returns>
	inline float Amount(Control key) { return (*inputReceivedEvent.GetState())[key]; }

	/// <summary>
	/// Determines whether the specified key is currently down.
	/// </summary>
	/// <param name="key">The key.</param>
	/// <returns>true if the key is down, false otherwise</returns>
	inline bool KeyIsDown(Control key) { return Amount(key) > 0; }

	inline void ResetKey(Control key) { inputReceivedEvent.SetState(key, 0); }
public:
	/// <summary>
	/// Subscribes children to receive message input events.
	/// </summary>
	Controlable();
	virtual ~Controlable();
};
