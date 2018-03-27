#include "Bitmap.h"
#include "RandomEngine.h"

#define clamp(a,n,x) (a > x ? x : a < n ? n : a)
#define trim_ext(str) do { size_t ext = str.rfind(".");if(ext != std::string::npos) { str = str.substr(0, ext); } }while(0)

class PixelData {
	unsigned char color[3];
	size_t x, y;
public:
	unsigned char& r() { return color[2]; }
	unsigned char& g() { return color[1]; }
	unsigned char& b() { return color[0]; }
	PixelData(unsigned char r, unsigned char g, unsigned char b, size_t _x, size_t _y) : color{b,g,r}, x(_x), y(_y) {}
};

ImageData::~ImageData() { for(size_t i = 0; i < pixels.size(); ++i) { delete pixels[i]; } }

void Bitmap::Load(const char* fileName, ImageData** image) {
	std::ifstream read;
	read.open(fileName, std::ios_base::binary);
	unsigned char file[14];
	unsigned char info[40];
	read.read((char*) file, sizeof(file));
	read.read((char*) info, sizeof(info));

	size_t w = (*image)->w;
	size_t h = (*image)->h;
	//for now I have to pass it in since the ways I try to get the w and h dont work
	//(*image)->w = w;
	//(*image)->h = h;
	(*image)->name = const_cast<char*>(fileName);

	unsigned char pad[3] = {0,0,0};
	unsigned char pixel[3];
	size_t padSize = (4 - (w * 3) % 4) % 4;

	(*image)->pixels.resize(w*h);
	size_t i = 0;
	for(size_t y = 0; y<h; ++y) {
		for(size_t x = 0; x<w; ++x) {
			read.read((char*) pixel, 3);
			(*image)->pixels[i++] = (new PixelData(pixel[2], pixel[1], pixel[0], x, y));
		}
		read.read((char*) pad, padSize);
	}
	read.close();
}

void Bitmap::Write(ImageData* image, std::function<void(const size_t X, const size_t Y, int& r, int& g, int& b)> funct) {
	_byte pad[3] = {0,0,0};
	int red, green, blue;
	_byte pixel[3];

	size_t i = 0;
	for(size_t y = 0, h = image->h; y < h; ++y) {
		for(size_t x = 0, w= image->w; x < w; ++x) {
			auto data = image->pixels[i++];
			red = data->r();
			green = data->g();
			blue = data->b();
			funct(x, y, red, green, blue);

			pixel[0] = clamp(blue, 0, 255);
			pixel[1] = clamp(green, 0, 255);
			pixel[2] = clamp(red, 0, 255);

			stream.write((char*) pixel, 3);
		}
		stream.write((char*) pad, padSize);
	}

}

Bitmap::Bitmap(const char* fileName, IMAGE_TYPE t, const size_t _width, const size_t _height)
	: sizeAll(sizeData + sizeof(file) + sizeof(info)),
	sizeData(int(width * height * 3 + height * padSize)),
	padSize((4 - (width * 3) % 4) % 4),
	width(_width), height(_height){

	std::string name(fileName);
	trim_ext(name);

	switch(t) {
	default: case BMP: name.append(".bmp"); break;
	case PNG: name.append(".png"); break;
	case JPG: name.append(".jpg"); break;
	case JPEG: name.append(".jpeg"); break;
	case ICO: name.append(".ico"); break;
	}

	stream.open(name.c_str(), std::ios_base::binary);

	file[2] = (_byte) (sizeAll);
	file[3] = (_byte) (sizeAll >> 8);
	file[4] = (_byte) (sizeAll >> 16);
	file[5] = (_byte) (sizeAll >> 24);

	info[4] = (_byte) (width);
	info[5] = (_byte) (width >> 8);
	info[6] = (_byte) (width >> 16);
	info[7] = (_byte) (width >> 24);

	info[8] = (_byte) (height);
	info[9] = (_byte) (height >> 8);
	info[10] = (_byte) (height >> 16);
	info[11] = (_byte) (height >> 24);

	info[20] = (_byte) (sizeData);
	info[21] = (_byte) (sizeData >> 8);
	info[22] = (_byte) (sizeData >> 16);
	info[23] = (_byte) (sizeData >> 24);

	stream.write((char*) file, sizeof(file));
	stream.write((char*) info, sizeof(info));
}

void Bitmap::Write(std::function<void(const size_t X, const size_t Y, int& r, int& g, int& b)> funct) {
	_byte pad[3] = {0,0,0};
	int red, green, blue;
	_byte pixel[3];

	for(size_t y = 0; y<height; y++) {
		for(size_t x = 0; x<width; x++) {
			red = green = blue = 0;
			funct(x, y, red, green, blue);
			
			pixel[0] = clamp(blue, 0, 255);
			pixel[1] = clamp(green, 0, 255);
			pixel[2] = clamp(red, 0, 255);

			stream.write((char*) pixel, 3);
		}
		stream.write((char*) pad, padSize);
	}
}

int Bitmap::RandomColorByte() {
	//lround(255.0 *static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	return lround(255.0 * Omiracon::Random::RandomNumber(0.0, 1.0));
}

Bitmap::~Bitmap() {
	stream.close();
}
