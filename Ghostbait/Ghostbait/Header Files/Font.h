#pragma once
#include <string>
#include <unordered_map>

struct CharPos {
	float startU, startV, endU, endV;
	CharPos();
	CharPos(float _startU, float _startV, float _endU, float _endV);
};

class Font {
	std::string fileName;
	std::unordered_map<char, CharPos> charMap;

public:
	Font();
	Font(std::string _fileName);

	inline void SetCharPos(char c, CharPos pos) { charMap[c] = pos; }
	const CharPos GetCharPos(char c);
	
	inline const CharPos InvalidCharPos() const { return CharPos(-1, -1, -1, -1); }
};