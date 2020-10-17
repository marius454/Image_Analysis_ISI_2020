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

bool Image::manipulateImage(unsigned short n, Eigen::Matrix3f *changeMatrices){
  if (_channels > 1){
    std::cout << "Only grayscale images can be manipulated" << std::endl;
    std::exit;
  }
  std::cout << "good" << std::endl;
  _indexCoordinates = new Eigen::Vector3i[ _width * _height ];
  _worldCoordinates = new Eigen::Vector3f[ _width * _height ];
  for (unsigned int y = 0; y < _height; y++)
    for (unsigned int x = 0; x < _width; x++){
      _indexCoordinates[ y*_width + x ] = Eigen::Vector3i{x,y,1};
      _worldCoordinates[ y*_width + x ] = Eigen::Vector3f{(float)x, (float)y, 1.0f};
      for (unsigned short i = 0; i < n; i++){
        _worldCoordinates[ y*_width + x ] = changeMatrices[i] * _worldCoordinates[ y*_width + x ];
      }
    }
  std::cout << _indexCoordinates[_width + 100] << std::endl;
  std::cout << _worldCoordinates[_width + 100] << std::endl;
  std::exit;
}

unsigned char* Image::getImageData(){ return _data; };
unsigned long Image::getWidth() {return _width;};
unsigned long Image::getHeight() { return _height;};
unsigned long Image::getDepth() { return _depth;} ;
unsigned long Image::getChannels() { return _channels; };
unsigned long Image::getBPP() { return _bpp;};
