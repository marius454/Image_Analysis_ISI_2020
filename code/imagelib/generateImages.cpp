#include "image.hpp"

void Image::generateImage(std::string imageType, float alphaXMultiplier, float alphaYMultiplier){
  if (imageType == "Lines"){
    generateLineImage(alphaXMultiplier, alphaYMultiplier);
  }
  else {
    std::cout << "Unknown image type. Exiting";
    std::exit(0);
  }
  calculateHistogram();
}

void Image::generateLineImage(float alphaXMultiplier, float alphaYMultiplier){
  _channels = 1;
  _bpp = 8;
  _data = new unsigned char[_width * _height * _channels];

  uint16 L = pow(2, _bpp);
  float alphaX = 2.0f*M_PI / static_cast<float>(_width);
  float alphaY = 2.0f*M_PI / static_cast<float>(_height);

  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++){
      float i = (L-1) * fabs(cos( alphaXMultiplier * alphaX * static_cast<float>(x) + 
                                  alphaYMultiplier * alphaY * static_cast<float>(y)));
      _data[y*_width + x] = static_cast<unsigned char>((int)i);
    }
}