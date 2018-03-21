#include "CodeSequence.h"
#include "Console.h"

const bool CodeSequence::IsValidInput(const Control input) {
	for (int i = 0; i < (int)validInputs.size(); ++i) {
		if (validInputs[i] == input)
			return true;
	}
	return false;
}

const bool CodeSequence::CheckNewInput(const Control input) {
	if (code.size() < 4 || !IsValidInput(input)) return false;
	
	if (code[position + 1] == input) {
		position += 1;
		if (position >= code.size() - 1) {
			position = -1;
			complete();
		}
		return true;
	}
	position = -1;
	return false;
}
void CodeSequence::SetCode(std::vector<Control>& _code) {
	code.clear();
	for (int i = 0; i < _code.size(); ++i) {
		code.push_back(_code[i]);
	}
}
void CodeSequence::SetCode(CodePreset preset) {
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