#include "image.hpp"

void Image::imageBlurring(uint32 filterWidth){
  if (filterWidth % 2 == 0){
    std::cout << "Filter width must be an odd number";
    return;
  }

  uint32 imgSize = _height * _width * _channels;
  uint32 filterSize = filterWidth * filterWidth;
  float* tempImg = new float[imgSize];
  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      int a = (filterWidth - 1) / 2;
      uint32 sumNeighbourhood = 0;
      for (int t = -a; t <= a; t++)
        for (int s = -a; s <= a; s++){
          int gx = x+s;
          int gy = y+t;
          if (gx > 0 && gy > 0 && gx < _width && gy < _height){
            sumNeighbourhood += (int)_data[ gy*_width + gx ];
          }
          // else{
          //   sumNeighbourhood += (int)_data[ y*_width + x ];
          // }
        }
      float w = (float)sumNeighbourhood / (float)filterSize;
      tempImg[y*_width + x] = w;
    }

  for (uint32 i = 0; i < imgSize; i++){
    _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
  }

  calculateHistogram();
}

void Image::sharpeningUnsharpMask(uint16 blurringFilterWidth, uint8 k){
  uint16 L = pow(2, _bpp);
  uint32 imgSize = _height * _width * _channels;
  float* original = new float[imgSize];
  float* tempImg = new float[imgSize];

  for(uint32 i = 0; i < imgSize; i++){
    original[i] = static_cast<float>(_data[i]);
  }

  float max = std::numeric_limits<float>::min();
  float min = std::numeric_limits<float>::min();
  imageBlurring(blurringFilterWidth);
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = original[i] - _data[i];
    // tempImg[i] = original[i] + k*tempImg[i];
    if (tempImg[i] > max) max = tempImg[i];
    if (tempImg[i] < min) min = tempImg[i];
  }
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
    // _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
  }
  
  max = std::numeric_limits<float>::min();
  min = std::numeric_limits<float>::max();
  imageBlurring(blurringFilterWidth);
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = original[i] + k*tempImg[i];
    if (tempImg[i] > max) max = tempImg[i];
    if (tempImg[i] < min) min = tempImg[i];
  }
  for(uint32 i = 0; i < imgSize; i++){
    tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
    _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
  }
  calculateHistogram();
}

void Image::sharpeningLaplacian(bool useN8){
  uint16 L = pow(2, _bpp);
  uint32 imgSize = _height * _width * _channels;
  float* tempImg = new float[imgSize];
  float d2f;

  float max = std::numeric_limits<float>::min();
  float min = std::numeric_limits<float>::min();

  if (!useN8){
    for (int y = 0; y < _height; y++)
      for (int x = 0; x < _width; x++){
        int f1 = (int)_data[ y*_width + (x+1) ];
        int f2 = (int)_data[ y*_width + (x-1) ];
        int f3 = (int)_data[ (y+1)*_width + x ];
        int f4 = (int)_data[ (y-1)*_width + x ];

        if ((x+1) >= _width) f1 = 0;
        if ((x-1) < 0) f2 = 0;
        if ((y+1) >= _height) f3 = 0;
        if ((y-1) < 0) f4 = 0;

        d2f = f1 + f2 + f3 + f4 - 4*(int)_data[ y*_width + x ];

        tempImg[ y*_width + x ] = (int)_data[ y*_width + x ] + d2f;
        if (tempImg[ y*_width + x ] > max) max = tempImg[ y*_width + x ];
        if (tempImg[ y*_width + x ] < min) min = tempImg[ y*_width + x ];
      }

    for(uint32 i = 0; i < imgSize; i++){
      tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
      _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
    }
  }
  else{
    for (int y = 0; y < _height; y++)
      for (int x = 0; x < _width; x++){
        d2f = 0;
        for (int t = -1; t <= 1; t++)
          for (int s = -1; s <= 1; s++){
            int gx = x+s;
            int gy = y+t;
            if (gx > 0 && gy > 0 && gx < _width && gy < _height){
              d2f += (int)_data[ gy*_width + gx ];
            }
            else{
              d2f += 0;
            }
          }
        d2f = d2f - 8*(int)_data[ y*_width + x ];

        tempImg[ y*_width + x ] = (int)_data[ y*_width + x ] + d2f;
        if (tempImg[ y*_width + x ] > max) max = tempImg[ y*_width + x ];
        if (tempImg[ y*_width + x ] < min) min = tempImg[ y*_width + x ];
      }
      
    for(uint32 i = 0; i < imgSize; i++){
      tempImg[i] = ((L-1)*(tempImg[i] - min)) / (max - min);
      _data[i] = static_cast<unsigned char>((int)round(tempImg[i]));
    }
  }
}