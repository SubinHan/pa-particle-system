#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include "Util/ImageReader.h"


ImageReader::ImageReader(std::string path)
{
	int width, height, bpp;
	this->_image = stbi_load(path.c_str(), &width, &height, &bpp, 0);
	this->_width = width;
	this->_height = height;
	this->_bpp = bpp;
}

ImageReader::ImageReader(int width, int height, int bpp)
{
	this->_image = new uint8_t[width * height * bpp];

	std::fill(_image, _image + width * height * bpp, ImageReader::DEFAULT_INTENSITY);

	this->_width = width;
	this->_height = height;
	this->_bpp = bpp;
}

ImageReader::~ImageReader()
{
	delete _image;
}

int ImageReader::getWidth() const
{
	return _width;
}

int ImageReader::getHeight() const
{
	return _height;
}

bool ImageReader::is_color() const
{
	return _bpp == 3 || _bpp == 4;
}

int ImageReader::get_offset(int x, int y) const
{
	int offset = (y * _width + x) * _bpp;
	return offset;
}

Pixel ImageReader::getPixelAt(int x, int y) const
{
	int offset = get_offset(x, y);

	Pixel result = {};

	if (is_color())
	{
		result._red = _image[offset + 0];
		result._green = _image[offset + 1];
		result._blue = _image[offset + 2];
		result._intensity = 0.2126f * static_cast<float>(result._red)
			+ 0.7152f * static_cast<float>(result._green)
			+ 0.0722f * static_cast<float>(result._blue);
		return result;
	}

	result._intensity = _image[offset];
	result._red = result._blue = result._green = result._intensity;

	return result;
}

void ImageReader::setPixelAt(int x, int y, Pixel pixel)
{
	int offset = get_offset(x, y);

	if (is_color())
	{
		_image[offset + 0] = pixel._red;
		_image[offset + 1] = pixel._green;
		_image[offset + 2] = pixel._blue;
		return;
	}

	_image[offset] = pixel._intensity;
}

void ImageReader::write(std::string path) const
{
	stbi_write_png(path.c_str(), _width, _height, _bpp, _image, _width * _bpp);
}

uint8_t* ImageReader::getRawData() const
{
	return _image;
}

int ImageReader::getBpp() const
{
	return _bpp;
}