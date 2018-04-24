#include "CheatCode.h"
#include "Console.h"

CheatCode::CheatCode(CodePreset preset, std::function<void(void)> func) {
	SetCode(preset);
	complete = func;
}
CheatCode::CheatCode(std::vector<Control>& _code, std::function<void(void)> func) {
	SetCode(_code);
	complete = func;
}

const bool CheatCode::IsValidInput(const Control input) {
	for (int i = 0; i < (int)validInputs.size(); ++i) {
		if (validInputs[i] == input)
			return true;
	}
	return false;
}
const bool CheatCode::CheckNewInput(const Control input) {
	if (code.size() < 4 || !IsValidInput(input)) return false;
	
	if (code[position + 1] == input) {
		position += 1;
		if (position >= (int)code.size() - 1) {
			position = -1;
			active = !active;
			complete();
		}
		return true;
	}
	position = -1;
	return false;
}
void CheatCode::SetCode(std::vector<Control>& _code) {
	code.clear();
	for (size_t i = 0; i < _code.size(); ++i) {
		code.push_back(_code[i]);
	}
}
void CheatCode::SetCode(CodePreset preset) {
	code.clear();
	switch (preset) {
		case Konami:
			code = { Control::rightItem0, Control::rightItem0,
				Control::rightItem3, Control::rightItem3,
				Control::rightItem1, Control::rightItem2,
				Control::rightItem1, Control::rightItem2,
				Control::rightCyclePrefab, Control::rightAttack };
		break;
	}
}