#pragma once
#include <vector>
#include "MessageStructs.h"
#include <functional>

/*
std::vector<Control> code = { Control::rightItem0, Control::rightItem0,
Control::rightItem3, Control::rightItem3,
Control::rightItem1, Control::rightItem2,
Control::rightItem1, Control::rightItem2,
Control::rightCyclePrefab, Control::rightAttack };
*/

class CodeSequence {
	std::vector<Control> code;
	std::vector<Control> validInputs = { Control::rightItem0, Control::leftItem0,
		Control::rightItem1, Control::leftItem1,
		Control::rightItem2, Control::leftItem2,
		Control::rightItem3, Control::leftItem3,
		Control::rightCyclePrefab, Control::leftCyclePrefab,
		Control::rightAttack, Control::leftAttack };
	int position = -1;
	std::function<void(void)> complete;
	unsigned minimumLength = 4;

	const bool IsValidInput(const Control input);

public:
	enum CodePreset {
		Konami,
	};

	const bool CheckNewInput(const Control input);
	void SetCode(std::vector<Control>& _code);
	void SetCode(CodePreset preset);

	inline void SetCompletionFunction(std::function<void(void)> func) { complete = func; }
};