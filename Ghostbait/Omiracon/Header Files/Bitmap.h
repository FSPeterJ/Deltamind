#pragma once
#include <fstream>
#include <functional>

//Signature to use with the Write function.
#define BMP() [](const size_t x, const size_t y, int& red, int& green, int& blue)

//Signature to use with the Write function when access to outside variables are needed.
#define BMP_r() [&](const size_t x, const size_t y, int& red, int& green, int& blue)

enum IMAGE_TYPE {
	BMP,
	PNG,
	JPG,
	JPEG,
	ICO
};

class PixelData;
#include <vector>
struct ImageData {
	char* name;
	std::vector<PixelData*> pixels;
	size_t w, h;
	ImageData(size_t _w, size_t _h) : w(_w), h(_h){}
	~ImageData();
};

/// <summary>
/// Usage when creating a new Bitmap:<para />Bitmap bmp("myBitmap", BMP, 256, 256); bmp.Write( BMP() { blue = 0; } );<para /><para>&#160;</para>
/// Usage when modifying an existing Bitmap:<para />ImageData* d = new ImageData(512, 512); Bitmap::Load("existingImage.bmp", &amp;d);<para />
/// Bitmap bmp("existingMod", BMP, 512, 512); bmp.Write(d, BMP() { red = 0; }); delete d;
/// </summary>
class Bitmap {
	typedef unsigned char _byte;

	std::ofstream stream;
	unsigned char file[14] = {
		'B','M', // magic
		0,0,0,0, // size in bytes
		0,0, // app data
		0,0, // app data
		40 + 14,0,0,0 // start of data offset
	};
	unsigned char info[40] = {
		40,0,0,0, // info hd size
		0,0,0,0, // width
		0,0,0,0, // heigth
		1,0, // number color planes
		24,0, // bits per pixel
		0,0,0,0, // compression is none
		0,0,0,0, // image bits size
		0x13,0x0B,0,0, // horz resoluition in pixel / m
		0x13,0x0B,0,0, // vert resolutions (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
		0,0,0,0, // #colors in pallete
		0,0,0,0, // #important colors
	};

	const size_t width, height;
	const int padSize;
	const int sizeData;
	const int sizeAll;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="Bitmap"/> class.
	/// </summary>
	/// <param name="fileName">Name of the file.</param>
	/// <param name="">The image type.</param>
	/// <param name="width">The width.</param>
	/// <param name="height">The height.</param>
	Bitmap(const char* fileName, IMAGE_TYPE, const size_t width, const size_t height);

	/// <summary>
	/// Loads the specified image into ImageData.
	/// </summary>
	/// <param name="fileName">Name of the file.</param>
	/// <param name="data">Gets filled with the data of the image.</param>
	static void Load(const char* fileName, ImageData**);

	/// <summary>
	/// Use this function to modify a loaded bitmap image.
	/// </summary>
	/// <param name="data">The loaded image data.</param>
	void Write(ImageData*, std::function<void(const size_t, const size_t, int&, int&, int&)>);

	/// <summary>
	/// Use this function to create bitmaps.
	/// Usage: Write(BMP() { red = 255; green = x ^ y; blue = x * x; });
	/// </summary>
	void Write(std::function<void(const size_t, const size_t, int&, int&, int&)>);
	
	/// <summary>
	/// Generates a random color byte.
	/// </summary>
	/// <returns>int.</returns>
	static int RandomColorByte();

	~Bitmap();
};
