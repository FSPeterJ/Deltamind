#include "Grid.h"
#include "DebugRenderer.h"

#include <assert.h>
#include <DirectXMath.h>

Grid::Grid(unsigned _size_x, unsigned _size_z, float _w, float _d, float _origin_x, float _origin_y) :
	vertices(new DirectX::XMFLOAT3[count]),
	count(((_size_x + 1) * 2) + ((_size_z - 1) * 2)),
	size_x(_size_x),
	size_z(_size_z),
	origin_x(_origin_x),
	origin_y(_origin_y),
	width(_w / _size_x),
	depth(_d / _size_z)
	{}

Grid::~Grid() {
	delete[] vertices;
}

void Grid::GreatGrid() {
	float x_bound = width * (size_x / 2.0f);
	float z_bound = depth * (size_z / 2.0f);

	unsigned i = 0;

	int mult = 1;

	for(float x = -1 * x_bound; x <= x_bound; x += width) {
		DirectX::XMFLOAT3 cur;
		cur.x = x;
		cur.y = 0;
		cur.z = mult * z_bound;
		vertices[i++] = cur;
		if(fabsf(x - x_bound) < FLT_EPSILON) {
			x = -1 * x_bound - width;
			if(mult == -1) break;
			mult = -1;
		}
	}
	z_bound = z_bound - depth;

	for(float z = z_bound; z >= -1 * z_bound; z -= depth) {
		DirectX::XMFLOAT3 cur;
		cur.x = mult * x_bound;
		cur.y = 0;
		cur.z = z;
		vertices[i++] = cur;
		if(fabsf(z - (-1 * z_bound)) < FLT_EPSILON) {
			z = z_bound + depth;
			if(mult == 1) break;
			mult = 1;
		}
	}

	//assert(i == count);
}


void Grid::Draw() {
	DebugRenderer::AddLine(vertices[0], vertices[10], {1,1,1});
	DebugRenderer::AddLine(vertices[11], vertices[21], {1,1,1});

	for(unsigned r = 22; r < 31; ++r) {
		DebugRenderer::AddLine(vertices[r], vertices[r + size_x - 1], {1,1,1});
	}

	for(unsigned c = 0; c <= size_x; ++c) {
		DebugRenderer::AddLine(vertices[c], vertices[c + size_z + 1], {1,1,1});
	}
}
