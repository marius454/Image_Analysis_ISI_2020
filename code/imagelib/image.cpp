#include "image.hpp"

Image::Image(){};

Image::Image(std::string filename){
  openFile(filename, 0);
  calculateHistogram();
}
Image::Image(std::string filename, short directory){
  openFile(filename, directory);
  calculateHistogram();
}
Image::Image(std::string filename1, std::string filename2, std::string filename3){
  combineFiles(filename1, filename2, filename3);
  calculateHistogram();
}
Image::Image(const Image &original){
  performCopy(original);
  calculateHistogram();
}
// Image::Image(unsigned int w, unsigned int h, float pixelUnit){
//   _width = w;
//   _height = h;
//   _data = (unsigned char*)malloc( w*h );
// }
// Image::Image(unsigned int w, unsigned int h, BBox region){
//   _width = w;
//   _height = h;
//   _data = (unsigned char*)malloc( w*h );
// }

Image::~Image() {
  delete[] _data;
};

void Image::performCopy(Image const & original){
  _width = original.getWidth();
  _height = original.getHeight();
  _channels = original.getChannels();
  _bpp = original.getBPP();

  unsigned long size = _width * _height * _channels;
  _data = new unsigned char[size];
  for (unsigned int i = 0; i < size; i++){
    _data[i] = original.getImageData()[i];
  }
}

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

unsigned char* Image::getImageData() const { return _data; };
unsigned long Image::getWidth() const {return _width;};
unsigned long Image::getHeight() const { return _height;};
unsigned long Image::getDepth() const { return _depth;} ;
unsigned long Image::getChannels() const { return _channels; };
unsigned long Image::getBPP() const { return _bpp;};
unsigned long Image::getSamplesPerPixel() const { return _channels; };
unsigned long Image::getBitsPerSample() const { return _bpp;};
std::vector<unsigned int> Image::getHistogram() const { return _histogram; };

