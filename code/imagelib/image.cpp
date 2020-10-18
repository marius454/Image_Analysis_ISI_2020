#include "image.hpp"

#include <iostream>
#include <string>
#include <Eigen/Core>
#include <Eigen/LU>

Image::Image(){};

Image::Image(std::string filename, short directory){
  openFile(filename, directory);
}
Image::Image(std::string filename1, std::string filename2, std::string filename3){
  combineFiles(filename1, filename2, filename3);
}
Image::Image(unsigned int w, unsigned int h, float pixelUnit){
  _width = w;
  _height = h;
  _data = (unsigned char*)malloc( w*h );
}
Image::Image(unsigned int w, unsigned int h, BBox region){
  _width = w;
  _height = h;
  _data = (unsigned char*)malloc( w*h );
}

Image::~Image() {
  delete[] _data;
};

bool isTiffFile(std::string filename){
  if (filename.substr(filename.find_last_of(".") + 1) == "tif" ||
  filename.substr(filename.find_last_of(".") + 1) == "svs") {
    return true;
  }
  else {
    return false;
  }
}

bool isJpegFile(std::string filename){
  if (filename.substr(filename.find_last_of(".") + 1) == "jpg") {
    return true;
  }
  else {
    return false;
  }
}

bool Image::openFile(std::string filename, short directory){
  // Check file type by filename extensions
  if(isTiffFile(filename)){
    return loadTiff(filename, directory);
  }
  else if(isJpegFile(filename)){
    return loadJpeg(filename);
  }
  return false;
};
bool Image::combineFiles(std::string filename1, std::string filename2, std::string filename3){
  if(isTiffFile(filename1) && isTiffFile(filename2) && isTiffFile(filename3)){
    return loadCombinedTiff(filename1, filename2, filename3);
  }
  return false;
}

unsigned char* Image::getImageData(){ return _data; };
unsigned long Image::getWidth() {return _width;};
unsigned long Image::getHeight() { return _height;};
unsigned long Image::getDepth() { return _depth;} ;
unsigned long Image::getChannels() { return _channels; };
unsigned long Image::getBPP() { return _bpp;};
