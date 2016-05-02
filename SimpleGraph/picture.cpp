#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <limits>

#include "picture.h"

using namespace std;
using namespace juntong_liu_lib;

picture::picture()
	: width(0), height(0), data(nullptr) {
}

picture::picture(int width, int height)
	: width(width), height(height), data((char*)malloc(width*height)) {
	if (data != nullptr);
	memset(data, 0x01, width*height);
}

picture::~picture() {
	free(data);
}

bool picture::drawPixel(const pointInt &p) {
	if (p.first < 0 || p.first >= width || p.second < 0 || p.second >= height)
		return false;
	data[p.first + p.second*width] = 0x00;
	return true;
}

bool picture::drawPoint(const pointInt &p, int thickness) {
	switch (thickness) {
	case 1: 
		drawPixel(p);
		return true;
	case 2: 
		drawPixel(p);
		drawPixel(pointInt(p.first + 1, p.second));
		drawPixel(pointInt(p.first, p.second + 1));
		drawPixel(pointInt(p.first + 1, p.second + 1));
		break;
	case 3:
		drawPixel(p);
		drawPixel(pointInt(p.first + 1, p.second));
		drawPixel(pointInt(p.first - 1, p.second));
		drawPixel(pointInt(p.first, p.second + 1));
		drawPixel(pointInt(p.first, p.second - 1));
		break;
	case 4:
		drawPixel(p);
		drawPixel(pointInt(p.first + 1, p.second));
		drawPixel(pointInt(p.first, p.second + 1));
		drawPixel(pointInt(p.first + 1, p.second + 1));
		drawPixel(pointInt(p.first - 1, p.second));
		drawPixel(pointInt(p.first, p.second - 1));
		drawPixel(pointInt(p.first + 1, p.second - 1));
		drawPixel(pointInt(p.first - 1, p.second + 1));
		drawPixel(pointInt(p.first + 2, p.second));
		drawPixel(pointInt(p.first, p.second + 2));
		drawPixel(pointInt(p.first + 1, p.second + 2));
		drawPixel(pointInt(p.first + 2, p.second + 1));
		break;
	default:
		drawRegion(
			circle(pointFloat(p.first, p.second), thickness), 
			pointFloat(p.first - thickness*1.5, p.second - thickness*1.1), 
			pointFloat(p.first + thickness*1.5, p.second + thickness*1.1));
	}
	return true;
}

bool picture::drawFunc(const function &f, const pointInt &p1, const pointInt &p2, const pointFloat &v1, const pointFloat &v2, int thickness) {
	float size = p1.first > p2.first ? (p1.first - p2.first + 1) : (p2.first - p1.first + 1);
	float interval = (v2.first - v1.first) / (size-1);
	for (int i = 0; i < size; i++) {
		drawPoint(getPosition(pointFloat(v1.first + i*interval, f(v1.first + i*interval)), p1, p2, v1, v2), thickness);
	}
	return true;
}

bool picture::drawLine(pointInt &p1, pointInt &p2, int thickness) {
	float x1 = p1.first;
	float y1 = p1.second;
	float x2 = p2.first;
	float y2 = p2.second;
	if (x2 != x1) {
		float k = (y2 - y1) / (x2 - x1);
		float d = y1 - k*x1;
		for (int i = x1>x2? x2 : x1; i <= x1 || i <= x2; i++) {
			drawPoint(pointInt(i, (int)(k*i + d + 0.5)), thickness);
		}
	}
	if (y1 != y2) {
		float k = (x2 - x1) / (y2 - y1);
		float d = x1 - k*y1;
		for (int i = y1>y2 ? y2 : y1; i <= y1 || i <= y2; i++) {
			drawPoint(pointInt((int)(k*i + d + 0.5), i), thickness);
		}
	}


	
	return true;
}

bool picture::writeToFile(std::ostream &f) {
	int bmp_size = (width*height + 7) / 8;
	BITMAPFILEHEADER bmpHeader;
	//bmpHeader.bfType = 0x424D;
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp_size + 2;
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;

	f.write("BM", 2);
	f.write((char*)&bmpHeader, sizeof bmpHeader);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(bmiHeader);
	bmiHeader.biWidth = width;
	bmiHeader.biHeight = height;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 1;
	bmiHeader.biCompression = 0;
	bmiHeader.biSizeImage = bmp_size;
	bmiHeader.biXPelsPerMeter = 10000;
	bmiHeader.biYPelsPerMeter = 10000;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;
	f.write((char*)&bmiHeader, sizeof bmiHeader);

	char color[] = { (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0xFF, (char)0xFF, (char)0xFF, (char)0x00 };
	f.write(color, 8);

	int length = (width + 31) / 32 * 4;   // byte per line
	int size = length * height;    // bit in total
	char* buffer = (char*)malloc(size);
	memset(buffer, 0, size);
	char power[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	char temp = 0x00;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			temp |= (data[i*width + j])*power[7 - j % 8];
			if (j % 8 == 7) {
				buffer[j / 8 + length*(height-i-1)] = temp;
				temp = 0x00;
			}
		}
		if (width % 8 != 0) {
			buffer[width / 8 + length*(height - i-1)] = temp;
			temp = 0x00;
		}
	}

	f.write((char*)buffer, size);
	return true;
}

bool picture::readFromFile(std::FILE &f) {
	return false;
}

template <typename T>
pair<T, T> picture::getExtremun(T* start, T* end) {
	if (start > end) {
		return pair<T, T>(*start, *end);
	}
	T* tempMax = start;
	T* tempMin = start;
	for (T* i = start; i != end; i++) {
		if (*i > *tempMax) {
			tempMax = i;
		}
		if (*i < *tempMin) {
			tempMin = i;
		}
	}
	return pair<T, T>(*tempMax, *tempMin);
}

pointFloat picture::getExtremun(const function &f, float x1, float x2, float interval) {
	if (x1 > x2)
		return pointFloat(0.0f, 0.0f);

	int size = (x2 - x1) / interval + 1;
	float* yBuffer = (float*)malloc(size*sizeof(float));
	if (yBuffer == nullptr)
		return pointFloat(0.0f, 0.0f);

	for (int i = 0; i < size; i++) {
		yBuffer[i] = f(x1 + i*interval);
	}
	pointInt buffer = getExtremun(yBuffer, yBuffer + size - 1);
	free(yBuffer);
	return buffer;
}

pointInt picture::getPosition(
	const pointFloat &pointAim, const pointInt &p1, const pointInt &p2, const pointFloat &v1, const pointFloat &v2) {
	int x = p1.first + (int)((p2.first - p1.first) / (v2.first - v1.first)* (pointAim.first - v1.first) + 0.5);
	int y = p1.second + (int)((p2.second - p1.second) / (v2.second - v1.second)* (pointAim.second - v1.second) + 0.5);
	if ((x < p1.first && x < p2.first) || (x > p2.first && x > p1.first) || (y < p1.second && y < p2.second) || (y > p2.second && y > p1.second)) {
		return pointInt(numeric_limits<int>::min(), numeric_limits<int>::min());
	} else {
		return pointInt(x, y);
	}
}

bool picture::drawRegion(
	const region &r, const pointInt &p1, const pointInt &p2, 
	const pointFloat &v1, const pointFloat &v2, const pointFloat &l1, const pointFloat &l2) {
	static pointFloat default(0.0f, 0.0f);
	int sizeX = p2.first > p1.first ? (p2.first - p1.first + 1) : (p1.first - p2	.first + 1);
	int sizeY = p2.second > p1.second ? (p2.second - p1.second + 1) : (p1.second - p2.second + 1);
	float stepX = (v2.first - v1.first) / (sizeX-1);
	float stepY = (v2.second - v1.second) / (sizeY -1);
	int startX;
	int endX;
	int startY;
	int endY;
	int bufferX1;
	int bufferX2;
	int bufferY1;
	int bufferY2;

	if (l1 == default && l2 == default) {
		startX = 0;
		startY = 0;
		endX = sizeX;
		endY = sizeY;
	} else {
		bufferX1 = p1.first + ((l1.first - v1.first) / stepX + 0.5);
		bufferX2 = p1.first + ((l2.first - v1.first) / stepX + 0.5);
		bufferY1 = p1.second + ((l1.second - v1.second) / stepX + 0.5);
		bufferY2 = p1.second + ((l2.second - v1.second) / stepX + 0.5);
		startX = bufferX1 > bufferX2 ? bufferX2 : bufferX1;
		endX = bufferX1 < bufferX2 ? bufferX2 : bufferX1;
		startY = bufferY1 > bufferY2 ? bufferY2 : bufferY1;
		endY = bufferY1 < bufferY2 ? bufferY2 : bufferY1;
		endX = endX > sizeX ? sizeX : endX;
		endY = endY > sizeY ? sizeY : endY;
	}
	for (int i = startX < 0? 0 : startX; i < endX; i++) {
		for (int j = startY < 0 ? 0 : startY; j < endY; j++) {
			if (r(v1.first + i*stepX, v1.second + j*stepY)) {
				drawPixel(pointInt(p1.first + i, p1.second + j));
			}
		}
	}
	return true;
}

bool picture::drawChar(char c, pointInt p, int size, rasterSize rs) {
	bool* font = (bool*)malloc(getHeight(rs) * getHeight(rs) * sizeof(bool));
	memset(font, 0, 40 * sizeof(bool));
	getRasterFont5x8(c ,font);
	int height = getHeight(rs);
	int width = getWidth(rs);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (font[j * width + i]) {
				for (int k = 0; k < size; k++) {    // draw square
					for (int l = 0; l < size; l++) {
						drawPixel(pointInt(p.first + i*size+k, p.second + j*size+l));
					}
				}
			}
		}
	}
	free(font);
	return true;
}

bool picture::getRasterFont5x8(char c, bool* container) {
	static bool zero[] = {
		false, true, true, false, false,
		true, false, false, true, false,
		true, false, true, true, false,
		true, true, false, true, false,
		true, false, false, true, false,
		true, false, false, true, false,
		false, true, true, false, false,
		false, false, false, false, false
	};
	static bool one[] = {
		false, true, false, false, false,
		true, true, false, false, false,
		false, true, false, false, false,
		false, true, false, false, false,
		false, true, false, false, false,
		false, true, false, false, false,
		true, true, true, false, false,
		false, false, false, false, false
	};
	static bool two[] = {
		false, true, true, false, false,
		true, false, false, true, false,
		true, false, false, true, false,
		false, false, true, false, false,
		false, true, false, false, false,
		true, false, false, false, false,
		true, true, true, true, false,
		false, false, false, false, false
	};
	static bool three[] = {
		false, true, true, false, false,
		true, false, false, true, false,
		false, false, false, true, false,
		false, true, true, false, false,
		false, false, false, true, false,
		true, false, false, true, false,
		false, true, true, false, false,
		false, false, false, false, false
	};
	static bool four[] = {
		false, false, true, false, false,
		false, true, true, false, false,
		true, false, true, false, false,
		true, false, true, false, false,
		true, false, true, false, false,
		true, true, true, true, false,
		false, false, true, false, false,
		false, false, false, false, false
	};
	static bool five[] = {
		true, true, true, true, false,
		true, false, false, false, false,
		true, false, false, false, false,
		true, true, true, false, false,
		false, false, false, true, false,
		false, false, false, true, false,
		true, true, true, false, false,
		false, false, false, false, false
	};
	static bool six[] = {
		false, true, true, false, false,
		true, false, false, true, false,
		true, false, false, false, false,
		true, true, true, false, false,
		true, false, false, true, false,
		true, false, false, true, false,
		false, true, true, false, false,
		false, false, false, false, false
	};
	static bool seven[] = {
		true, true, true, true, false,
		false, false, false, true, false,
		false, false, true, false, false,
		false, false, true, false, false,
		false, false, true, false, false,
		false, false, true, false, false,
		false, false, true, false, false,
		false, false, false, false, false
	};
	static bool eight[] = {
		false, true, true, false, false,
		true, false, false, true, false,
		true, false, false, true, false,
		false, true, true, false, false,
		true, false, false, true, false,
		true, false, false, true, false,
		false, true, true, false, false,
		false, false, false, false, false
	};
	static bool nine[] = {
		false, true, true, false, false,
		true, false, false, true, false,
		true, false, false, true, false,
		false, true, true, true, false,
		false, false, false, true, false,
		true, false, false, true, false,
		false, true, true, false, false,
		false, false, false, false, false
	};
	static bool caret[] = {
		false, false, true, false, false,
		false, true, false, true, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false
	};
	static bool plus[] = {
		false, false, false, false, false,
		false, true, false, false, false,
		false, true, false, false, false,
		true, true, true, false, false,
		false, true, false, false, false,
		false, true, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false
	};
	static bool minus[] = {
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		true, true, true, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false
	};
	static bool equal[] = {
		false, false, false, false, false,
		false, false, false, false, false,
		true, true, true, false, false,
		false, false, false, false, false,
		true, true, true, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false
	};
	static bool x[] = {
		false, false, false, false, false,
		false, false, false, false, false,
		true, false, false, true, false,
		false, true, true, false, false,
		false, true, true, false, false,
		true, false, false, true, false,
		true, false, false, true, false,
		false, false, false, false, false
	};
	static bool y[] = {
		false, false, false, false, false,
		false, false, false, false, false,
		true, false, false, true, false,
		true, false, false, true, false,
		false, true, true, true, false,
		false, false, false, true, false,
		true, true, true, false, false,
		false, false, false, false, false
	};
	static bool O[] = {
		false, true, true, false, false,
		true, false, false, true, false,
		true, false, false, true, false,
		true, false, false, true, false,
		true, false, false, true, false,
		true, false, false, true, false,
		false, true, true, false, false,
		false, false, false, false, false
	};
	static bool dot[] = {
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, true, false, false, false,
		false, false, false, false, false
	};
	static bool empty[] = {
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false,
		false, false, false, false, false
	};
	switch (c) {
	case '0':
		std::memcpy(container, zero, 40 * sizeof(bool));
		return true;
	case '1':
		std::memcpy(container, one, 40 * sizeof(bool));
		return true;
	case '2':
		std::memcpy(container, two, 40 * sizeof(bool));
		return true;
	case '3':
		std::memcpy(container, three, 40 * sizeof(bool));
		return true;
	case '4':
		std::memcpy(container, four, 40 * sizeof(bool));
		return true;
	case '5':
		std::memcpy(container, five, 40 * sizeof(bool));
		return true;
	case '6':
		std::memcpy(container, six, 40 * sizeof(bool));
		return true;
	case '7':
		std::memcpy(container, seven, 40 * sizeof(bool));
		return true;
	case '8':
		std::memcpy(container, eight, 40 * sizeof(bool));
		return true;
	case '9':
		std::memcpy(container, nine, 40 * sizeof(bool));
		return true;
	case '^':
		std::memcpy(container, caret, 40 * sizeof(bool));
		return true;
	case '+':
		std::memcpy(container, plus, 40 * sizeof(bool));
		return true;
	case '-':
		std::memcpy(container, minus, 40 * sizeof(bool));
		return true;
	case 'x':
		std::memcpy(container, x, 40 * sizeof(bool));
		return true;
	case 'y':
		std::memcpy(container, y, 40 * sizeof(bool));
		return true;
	case '=':
		std::memcpy(container, equal, 40 * sizeof(bool));
		return true;
	case 'O':
		std::memcpy(container, O, 40 * sizeof(bool));
		return true;
	case '.':
		std::memcpy(container, dot, 40 * sizeof(bool));
		return true;
	default:
		std::memcpy(container, empty, 40 * sizeof(bool));
		return false;
	}
}

bool picture::drawRegion(const region &r, const pointFloat &l1, const pointFloat &l2 ) {
	drawRegion(r, pointInt(0, 0), pointInt(width, height), pointFloat(0.0f, 0.0f), pointFloat(width*1.0f, height*1.0f), l1, l2);
	return true;
}

bool picture::drawString(const string &s, const pointInt &p, int size, rasterSize rs, strPos ps) {
	int len = s.length();
	int modifier;
	switch (ps) {
	case juntong_liu_lib::LEFT:
		modifier = 0;
		break;
	case juntong_liu_lib::CENTER:
		modifier = getWidth(rs) * size * len / 2;
		break;
	case juntong_liu_lib::RIGHT:
		modifier = getWidth(rs) *size * len;
		break;
	default:
		break;
	}
	for (int i = 0; i < len; i++) {
		drawChar(s[i], pointInt(p.first + i*getWidth(rs)*size-modifier, p.second), size, rs);
	}
	return true;
}

int picture::getWidth(rasterSize rs) {
	switch (rs) {
	case juntong_liu_lib::_5x8:
		return 5;
	default:
		return -1;
	}
}

int picture::getHeight(rasterSize rs) {
	switch (rs) {
	case juntong_liu_lib::_5x8:
		return 8;
	default:
		return -1;
	}
}

bool picture::drawFuncWithFrame(const function &f, const pointInt &p1, const pointInt &p2, const pointFloat &v1, const pointFloat &v2, rasterSize rs) {
	int top = p1.second > p2.second ? p2.second : p1.second;
	int bottom = p1.second < p2.second ? p2.second : p1.second;
	int left = p1.first < p2.first ? p1.first : p2.first;
	int right = p1.first > p2.first ? p1.first : p2.first;
	int sizeX = right - left;
	int sizeY = bottom - top;
	int scale = sizeX > sizeY ? sizeY / 200 + 1 : sizeX / 200 + 1;
	drawFunc(f, p1, p2, v1, v2, scale);
	drawLine(pointInt(left, bottom), pointInt(left, top-scale*20), scale);
	drawLine(pointInt(left, bottom), pointInt(right+scale*20, bottom), scale);
	drawString("O", pointInt(left - scale * getWidth(rs)*2, bottom + getHeight(rs)*scale), scale, _5x8, RIGHT);
	stringstream buffer;
	buffer << int(v1.second > v2.second ? v2.second : v1.second);
	drawString(buffer.str(), pointInt(left - scale * getWidth(rs) * 2, bottom - getHeight(rs)*scale), scale, rs, RIGHT);
	buffer.str("");
	buffer << int(v1.second < v2.second ? v2.second : v1.second);
	drawString(buffer.str(), pointInt(left - scale * getWidth(rs) * 2, top - getHeight(rs)*scale), scale, rs, RIGHT);
	buffer.str("");
	buffer << int(v1.first > v2.first ? v2.first : v1.first);
	drawString(buffer.str(), pointInt(left, bottom + getHeight(rs)* scale), scale, rs);
	buffer.str("");
	buffer << int(v1.first < v2.first ? v2.first : v1.first);
	drawString(buffer.str(), pointInt(right, bottom + getHeight(rs)* scale), scale, rs);
	drawString(f.getExpression(), pointInt((left + right) / 2, top - getHeight(rs)* scale * 2 - scale*10), scale, rs, CENTER);
	return true;
}

function::~function() {}

region::~region() {}

circle::circle(pointFloat center, float radius) 
	: center(center), radius(radius) {}

circle::~circle() {}

bool circle::operator()(const float x, const float y) const {
	float distSqr = (x - center.first) * (x - center.first) + (y - center.second) * (y - center.second);
	return distSqr < radius*radius;
}
