#pragma once

#include <string>

/**
 * @brief
 * The struct Pixel represents a pixel of color images.
 * If the image is grayscale, then red, green, blue contains intensity value.
 * intensity = 0.2126 * red + 0.7152 * green + 0.0722 * blue
*/
struct Pixel {
	uint8_t _red;
	uint8_t _green;
	uint8_t _blue;
	uint8_t _intensity;

	Pixel(uint8_t red, uint8_t green, uint8_t blue) : _red(red), _green(green), _blue(blue)
	{
		_intensity = 0.2126 * red + 0.7152 * green + 0.0722 * blue;
	}
	Pixel(uint8_t intensity = 0) : Pixel(intensity, intensity, intensity) {}

};

class ImageReader
{
public:
	static constexpr int DEFAULT_INTENSITY = 0;

private:
	uint8_t* _image;
	int _width;
	int _height;
	int _bpp;

public:
	ImageReader() = delete;
	ImageReader(const ImageReader& copy) : _width(copy._width), _height(copy._height), _bpp(copy._bpp)
	{
		_image = new uint8_t[_width * _height * _bpp];
		memcpy(_image, copy._image, sizeof(uint8_t) * _width * _height * _bpp);
	}
	ImageReader(std::string path);
	ImageReader(int width, int height, int bpp = 3);
	~ImageReader();
	int getWidth() const;
	int getHeight() const;
	Pixel getPixelAt(int x, int y) const;
	void setPixelAt(int x, int y, Pixel pixel);
	void write(std::string path) const;
	uint8_t* getRawData() const;
	int getBpp() const;

	ImageReader& operator=(const ImageReader& ref)
	{
		delete _image;
		_width = ref._width;
		_height = ref._height;
		_bpp = ref._bpp;

		_image = new uint8_t[static_cast<size_t>(_width) * _height * _bpp];
		memcpy(_image, ref._image, sizeof(uint8_t) * _width * _height * _bpp);

		return *this;
	}

private:
	int get_offset(int x, int y) const;
	bool is_color() const;
};