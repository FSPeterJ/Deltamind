#pragma once

namespace DirectX {
	struct XMFLOAT3;
}
class Grid {
private:
	DirectX::XMFLOAT3* vertices;

	float origin_x, origin_y;
	unsigned size_x, size_z;

	//float off_x, off_y;

	//int bound_x, bound_y;

	float width, depth;

	unsigned count;


	Grid() {}
public:
	Grid(unsigned _size_x, unsigned _size_z, float _w, float _d, float _origin_x = 0, float _origin_y = 0);
	~Grid();
	
	
	//vertex operator[](unsigned index) {
	//	return vertices[index];
	//}

	void GreatGrid();

	void Draw();

	unsigned get_vertex_count() {
		return count;
	}

	//vertex* get_vertices() {
	//	return vertices;
	//}

	//void print() {
	//	for(unsigned i = 0; i < count; ++i) {
	//		vertices[i].print();
    //
	//		if(i%size_z == 0)
	//			cout << endl;
	//	}
	//}

	//void set_vertices(vertex* _vertices) {
	//	for(unsigned i = 0; i < count; ++i) {
	//		vertices[i] = _vertices[i];
	//	}
	//}
};
