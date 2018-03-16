#include "Font.h"

CharPos::CharPos() {
	startU = 0;
	startV = 0;
	endU = 0;
	endV = 0;
}
CharPos::CharPos(float _startX, float _startY, float _endX, float _endY) {
	startU = _startX;
	startV = _startX;
	endU = _startX;
	endV = _startX;
}


Font::Font() {

}
Font::Font(std::string _fileName) : fileName(_fileName) {

}

const CharPos Font::GetCharPos(char c) {
	if (charMap.find(c) != charMap.end())
		return charMap[c];
	else
		return InvalidCharPos();
}