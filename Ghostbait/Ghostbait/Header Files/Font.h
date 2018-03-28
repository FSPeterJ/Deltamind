#pragma once
#include <string>
#include <unordered_map>

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11Device;
struct ID3D11DeviceContext;

struct CharPos {
	float startU, endU, size;
	CharPos();
	CharPos(float _startU, float _endU, float _size);
};

class Font {
	std::unordered_map<char, CharPos> charMap;
	ID3D11Texture2D* tex;
	ID3D11ShaderResourceView* srv;
	

	bool LoadIndexFile(std::string _fileName, float width);
	bool LoadTexture(std::string _fileName, ID3D11Device* device, ID3D11DeviceContext* context);
public:
	Font();
	Font(std::string _fileName, std::string _texturePath, ID3D11Device* device, ID3D11DeviceContext* context);
	~Font();

	inline void SetCharPos(char c, CharPos pos) { charMap[c] = pos; }
	const CharPos GetCharPos(char c);
	
	float GetTextureHeight();
	inline const CharPos InvalidCharPos() const { return CharPos(-1, -1, -1); }

	inline ID3D11ShaderResourceView* GetShaderResourceView() { return srv; }
};