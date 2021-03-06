#include "image.hpp"

Image::Image(){};

Image::Image(std::string filename){
  openFile(filename, 0);
  _imgSize = _width * _height * _channels;
  _L = pow(2, _bpp);
  calculateHistogram();
}
Image::Image(std::string filename, short directory){
  openFile(filename, directory);
  _imgSize = _width * _height * _channels;
  _L = pow(2, _bpp);
  calculateHistogram();
}
Image::Image(std::string filename1, std::string filename2, std::string filename3){
  combineFiles(filename1, filename2, filename3);
  _imgSize = _width * _height * _channels;
  _L = pow(2, _bpp);
  calculateHistogram();
}
Image::Image(const Image &original){
  performCopy(original);
  _imgSize = _width * _height * _channels;
  _L = pow(2, _bpp);
  calculateHistogram();
}
Image::Image(uint32 width, uint32 height){
  _width = width;
  _height = height;
  // generateImage("Lines", 3.0f, 0.0f);
}

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
  else return false;
}

bool isJpegFile(std::string filename){
  if (filename.substr(filename.find_last_of(".") + 1) == "jpg") return true;
  else return false;
}

bool Image::openFile(std::string filename, short directory){
  // Check file type by filename extensions
  if(isTiffFile(filename)) return loadTiff(filename, directory);
  else if(isJpegFile(filename)) return loadJpeg(filename);
  return false;
};
bool Image::combineFiles(std::string filename1, std::string filename2, std::string filename3){
  if(isTiffFile(filename1) && isTiffFile(filename2) && isTiffFile(filename3)){
    return loadCombinedTiff(filename1, filename2, filename3);
  }
  return false;
}

void Image::splitChannels(char showChannel){
  if (_channels == 3){
    _Rdata = new unsigned char[_imgSize / _channels];
    _Gdata = new unsigned char[_imgSize / _channels];
    _Bdata = new unsigned char[_imgSize / _channels];
    for (uint32 i = 0; i < _imgSize; i += _channels){
      _Rdata[i / _channels] = _data[i];
      _Gdata[i / _channels] = _data[i+1];
      _Bdata[i / _channels] = _data[i+2];
    }

    _imgSize = _imgSize / _channels;
    _channels = 1;
    
    delete(_data);
    _data = new unsigned char[_imgSize];
    if (showChannel == 'R') for (uint32 i = 0; i < _imgSize; i++) _data[i] = _Rdata[i];
    else if (showChannel == 'G') for (uint32 i = 0; i < _imgSize; i++) _data[i] = _Gdata[i];
    else if (showChannel == 'B') for (uint32 i = 0; i < _imgSize; i++) _data[i] = _Bdata[i];
    else for (uint32 i = 0; i < _imgSize; i++) _data[i] = (_Rdata[i] + _Gdata[i] + _Bdata[i]) / 3;
  }
}
void Image::initializeRGB(){
  if (_channels == 1){
    _Rdata = new unsigned char[_imgSize];
    _Gdata = new unsigned char[_imgSize];
    _Bdata = new unsigned char[_imgSize];
    for (uint32 i = 0; i < _imgSize; i += _channels){
      _Rdata[i] = _data[i];
      _Gdata[i] = _data[i];
      _Bdata[i] = _data[i];
    }
  }
}
void Image::combineRGB(){
  if (_channels == 1){
    delete (_data);
    _channels = 3;
    _imgSize = _imgSize * _channels;
    _data = new unsigned char[_imgSize];

    for (uint32 i = 0; i < _imgSize; i += _channels){
      _data[i] = _Rdata[i / _channels];
      _data[i+1] = _Gdata[i / _channels];
      _data[i+2] = _Bdata[i / _channels];
    }
  }
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