#pragma once
template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

namespace std {
	template<>
	struct hash<HexTile*> { 
		typedef HexTile* argument_type; 
		size_t operator()(const argument_type& data) const noexcept;
	};
}
