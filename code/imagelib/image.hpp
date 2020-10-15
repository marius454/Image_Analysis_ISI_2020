#ifndef SIMPLE_IMAGE_LIBRARY_HPP
#define SIMPLE_IMAGE_LIBRARY_HPP

#include <string>

class Image{
public:
	Image();
	Image(std::string filename, short directory);
	Image(std::string filename1, std::string filename2, std::string filename3);
	virtual ~Image();
	// File related
	bool openFile(std::string filename, short directory);
	bool combineFiles(std::string filename1, std::string filename2, std::string filename3);
	// Image related
	unsigned char* getImageData();
	// Get attributes
	unsigned long getWidth();
	unsigned long getHeight();
	unsigned long getDepth();
	unsigned long getChannels();
	unsigned long getBPP();
private:
	unsigned long _width{0};
	unsigned long _height{0};
	unsigned long _depth{0};
	unsigned long _channels{0};
	unsigned long _bpp{0};

	unsigned char* _data{nullptr};
	// Tiff related stuff
	bool loadTiff(std::string filename, short directory);
	// Jpeg related stuff
	bool loadJpeg(std::string filename);
	// any other...
	bool loadCombinedTiff(std::string filename1, std::string filename2, std::string filename3);
};
#endif
