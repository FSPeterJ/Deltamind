#pragma once
#include "GameObject.h"

class Monitor : public GameObject {
	GameObject* screen = nullptr;
	std::string font = "Assets/Fonts/defaultFont.png";
	bool positioned = false; //TODO: Temprary until the object has its position on awake

	std::string curWave;
	std::string totalWaves;
public:
	void Awake(Object* obj) override;
	void Update() override;

	inline void SetScreenFont(const std::string _font) { font = _font; }
	void WriteToScreen(const std::string text, const DirectX::XMFLOAT4 foreground = { 1, 1, 1, 1 }, const DirectX::XMFLOAT4 background = { 0, 0, 0, 1 });
};