#pragma once

#include <fstream>
#include <utility>



namespace juntong_liu_lib {

	typedef std::pair<int, int> pointInt;
	typedef std::pair<float, float> pointFloat;

	enum rasterSize {_5x8};

	enum strPos { LEFT, CENTER, RIGHT };

	class function {
	public:
		virtual ~function() = 0;

		virtual float operator()(const float x) const = 0;

		virtual std::string getExpression() const = 0;
	};

	class region {
	public:
		virtual ~region() = 0;

		virtual bool operator()(const float x, const float y) const = 0;
	};

	class circle : public region {
	public:
		circle(pointFloat center, float radius);

		virtual ~circle();

		virtual bool operator()(const float x, const float y) const override;

	private:
		pointFloat center;
		float radius;
	};

	class picture {
	public:

		picture();

		picture(int width, int height);

		~picture();

		bool drawPixel(const pointInt &p);

		bool drawPoint(const pointInt &p, int thickness);

		bool drawLine(pointInt &p1, pointInt &p2, int thickness);

		bool drawFunc(const function &f, const pointInt &p1, const pointInt &p2, const pointFloat &v1, const pointFloat &v2, int thickness);

		bool drawRegion(const region &r, const pointInt &p1, const pointInt &p2, const pointFloat &v1, const pointFloat &v2, const pointFloat &l1 = pointFloat(0.0f, 0.0f), const pointFloat &l2 = pointFloat(0.0f, 0.0f));

		bool drawRegion(const region &r, const pointFloat &l1 = pointFloat(0.0f, 0.0f), const pointFloat &l2 = pointFloat(0.0f, 0.0f));

		bool drawChar(char c, pointInt p, int size, rasterSize rs = _5x8);

		bool drawString(const std::string &s, const pointInt &p, int size, rasterSize rs = _5x8, strPos ps = LEFT);

		bool drawFuncWithFrame(const function &f, const pointInt &p1, const pointInt &p2, const pointFloat &v1, const pointFloat &v2, rasterSize rs = _5x8);

		bool writeToFile(std::ostream &f);

		bool readFromFile(std::FILE &f);

		template <typename T> static std::pair<T, T> getExtremun(T* start, T* end);

		static pointInt getPosition(const pointFloat &pointAim, const pointInt &p1, const pointInt &p2, const pointFloat &v1, const pointFloat &v2);

		static pointFloat getExtremun(const function &f, float x1, float x2, float interval);

		static bool getRasterFont5x8(char c, bool* container);

	private:
		typedef struct tagBITMAPINFOHEADER {
			unsigned int biSize;
			unsigned int biWidth;
			unsigned int biHeight;
			unsigned short biPlanes;
			unsigned short biBitCount;
			int biCompression;
			int biSizeImage;
			unsigned int biXPelsPerMeter;
			unsigned int biYPelsPerMeter;
			unsigned int biClrUsed;
			unsigned int biClrImportant;
		} BITMAPINFOHEADER;

		typedef struct tagBITMAPFILEHEADER {
			//unsigned short bfType;
			unsigned int bfSize;
			unsigned short bfReserved1;
			unsigned short bfReserved2;
			unsigned int bfOffBits;
		} BITMAPFILEHEADER;

		char* data;
		int width;
		int height;

		static int getWidth(rasterSize rs);

		static int getHeight(rasterSize rs);
	};

	
	
}
