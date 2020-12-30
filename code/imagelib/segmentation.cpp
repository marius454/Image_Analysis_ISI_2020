#include "image.hpp"
#define BACKGROUND 100

void Image::createFISHreport(uint32 threshholdT, uint32 SEwidth, uint32 backgroundX, uint32 backgroundY){
  threshold(threshholdT);
  // calculatedThreshold(threshholdT);
  squareErosion(SEwidth);
  squareDilation(SEwidth);
  findNeighbourhood(backgroundX, backgroundY, 0, BACKGROUND);
  findNeighbourhood(200, 0, 0, BACKGROUND);
  fillHoles();
  uint16 cellNR = findCells();
}

void Image::calculatedThreshold(uint8 changePoint){
  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bpp);

  float sumG1 = 0;
  uint32 countG1 = 0;
  float sumG2 = 0;
  uint32 countG2 = 0;
  float deltaT = 1;
  float T = 0;
  
  while (abs(T - changePoint) > deltaT){
    if (T != 0) changePoint = T;

    for (uint32 i = 0; i < imgSize; i++){
      if (_data[i] > changePoint){
        sumG1 += (float)_data[i];
        countG1++;
      }
      else{
        sumG2 += (float)_data[i];
        countG2++;
      } 
    }
    T = ((sumG1 / (float)countG1) + (sumG2 / (float)countG2)) / 2;
  }
  
  std::cout << "T = " << T << std::endl;

  for (uint32 i = 0; i < imgSize; i++){
    if (_data[i] > T) _data[i] = static_cast<unsigned char>(L-1);
    else _data[i] = static_cast<unsigned char>(0);
  }
  calculateHistogram();
}

void Image::threshold(uint8 changePoint){
  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bpp);
  uint32 T = changePoint;

  for (uint32 i = 0; i < imgSize; i++){
    if (_data[i] > T) _data[i] = static_cast<unsigned char>(L-1);
    else _data[i] = static_cast<unsigned char>(0);
  }
  calculateHistogram();
}

void Image::squareErosion(uint16 squareWidth){
  uint32 imgSize = _width * _height * _channels;
  unsigned char *tempData = new unsigned char[imgSize];
  uint16 L = pow(2, _bpp);
  uint16 squareSize = squareWidth * squareWidth;
  int a = (squareWidth - 1) / 2;
  uint16 nrPixelsInsideSet = 0;

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      for (int t = -a; t <= a; t++)
        for (int s = -a; s <= a; s++){
          int fx = x+s;
          int fy = y+t;
          if (fx >= 0 && fy >= 0 && fx < _width && fy < _height){
            if (_data[fy*_width + fx] == L-1) nrPixelsInsideSet++;
          }
        }
      if (nrPixelsInsideSet == squareSize) tempData[y*_width + x] = static_cast<unsigned char>(L-1);
      else tempData[y*_width + x] = static_cast<unsigned char>(0);
      nrPixelsInsideSet= 0;
    }
  delete(_data);
  _data = tempData;
}

void Image::squareDilation(uint16 squareWidth){
  uint32 imgSize = _width * _height * _channels;
  unsigned char *tempData = new unsigned char[imgSize];
  uint16 L = pow(2, _bpp);
  int a = (squareWidth - 1) / 2;

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++)
      for (int t = -a; t <= a; t++)
        for (int s = -a; s <= a; s++){
          int fx = x+s;
          int fy = y+t;
          if (fx >= 0 && fy >= 0 && fx <= _width && fy <= _height){
            if (_data[y*_width + x] == L-1) tempData[fy*_width + fx] = static_cast<unsigned char>(L-1);
          }
        }

  delete(_data);
  _data = tempData;
}

// give x and y coordinates of a point in the background of the image
void Image::findNeighbourhood(uint32 x, uint32 y, uint8 startIntensity, uint8 endIntensity){
  uint32 imgSize = _width * _height * _channels;
  bool *visitedPixels = new bool[imgSize];
  for (uint32 i = 0; i < imgSize; i++)
    visitedPixels[i] = false;
  
  getNeighbours(x, y, startIntensity, visitedPixels);

  for (uint32 i = 0; i < imgSize; i++){
    if (visitedPixels[i] == true) _data[i] = static_cast<unsigned char>(endIntensity);
  }
  calculateHistogram();
}

void Image::getNeighbours(uint32 x, uint32 y, uint8 intensity, bool *visitedPixels){
  visitedPixels[y*_width + x] = true;
  for (int t = -1; t <= 1; t++)
    for (int s = -1; s <= 1; s++){
      int fx = x+s;
      int fy = y+t;
      if (fx >= 0 && fy >= 0 && fx < _width && fy < _height){
        if (_data[fy*_width + fx] == intensity && !visitedPixels[fy*_width + fx]) getNeighbours(fx, fy, intensity, visitedPixels);
      }
    }
}

void Image::fillHoles(){
  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bpp);

  for (uint32 i = 0; i < imgSize; i++){
    if (_data[i] == 0) _data[i] = static_cast<unsigned char>(L-1);
    if (_data[i] == BACKGROUND) _data[i] = static_cast<unsigned char>(0);
  }

  calculateHistogram();
}

uint16 Image::findCells(){
  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bpp);
  uint16 cellNR = 0;

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      if (_data[y*_width + x] == L-1){
        cellNR ++;
        findNeighbourhood(x,y, L-1, (L-1)-cellNR*5);
      }
    }

  calculateHistogram();
  return cellNR;
}