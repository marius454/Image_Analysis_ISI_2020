#include "image.hpp"
#define BACKGROUND 100
#define BOUNDDIFF 5

void Image::createFISHreport(uint16 visualizedStep){
  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bpp);

  splitChannels('B');
  threshold(20, _data);
  squareErosion(5);
  squareDilation(5);
  findNeighbourhood(0, 0, 0, BACKGROUND);
  //findNeighbourhood(200, 0, 0, BACKGROUND);
  fillHoles();
  getBoudaries();

  uint16 cellNr = findCells();
  std::cout << "Number of Cells: " << cellNr << std::endl;

  if (visualizedStep == 0){
    _data = _Gdata;
  }
  else if (visualizedStep == 1){
    threshold(120, _Rdata);
    for (uint32 i = 0; i < imgSize; i++) {
      if ((int)_data[i] + (int)_Rdata[i] > L-1){
        _data[i] = static_cast<unsigned char>(L-1);
      }
      else{
        _data[i] = _data[i] + _Rdata[i];
      }
    }
    std::cout << "Number of Acredine mutations in cells:" << std::endl;
    assignCells(cellNr);
  }
  else if (visualizedStep == 2){
    calculatedThreshold(50, _Gdata);
    for (uint32 i = 0; i < imgSize; i++) {
      if ((int)_data[i] + (int)_Gdata[i] > L-1){
        _data[i] = static_cast<unsigned char>(L-1);
      }
      else{
        _data[i] = _data[i] + _Gdata[i];
      }
    }
    std::cout << "Number of FITC mutations in cells:" << std::endl;
    assignCells(cellNr);
  }
  
  calculateHistogram();
}

void Image::calculatedThreshold(uint8 changePoint, unsigned char *data){
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
      if (data[i] > changePoint){
        sumG1 += (float)data[i];
        countG1++;
      }
      else{
        sumG2 += (float)data[i];
        countG2++;
      } 
    }
    T = ((sumG1 / (float)countG1) + (sumG2 / (float)countG2)) / 2;
  }
  
  std::cout << "T = " << T << std::endl;

  for (uint32 i = 0; i < imgSize; i++){
    if (data[i] > T) data[i] = static_cast<unsigned char>(L-1);
    else data[i] = static_cast<unsigned char>(0);
  }
  calculateHistogram();
}

void Image::threshold(uint8 changePoint, unsigned char *data){
  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bpp);
  uint32 T = changePoint;

  for (uint32 i = 0; i < imgSize; i++){
    if (data[i] > T) data[i] = static_cast<unsigned char>(L-1);
    else data[i] = static_cast<unsigned char>(0);
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
  
  calculateHistogram();
}

void Image::squareDilation(uint16 squareWidth){
  uint32 imgSize = _width * _height * _channels;
  unsigned char *tempData = new unsigned char[imgSize];
  uint16 L = pow(2, _bpp);
  int a = (squareWidth - 1) / 2;

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      tempData[y*_width + x] = static_cast<unsigned char>(0);
      for (int t = -a; t <= a; t++)
        for (int s = -a; s <= a; s++){
          int fx = x+s;
          int fy = y+t;
          if (fx >= 0 && fy >= 0 && fx < _width && fy < _height){
            if (_data[y*_width + x] == L-1) tempData[fy*_width + fx] = static_cast<unsigned char>(L-1);
          }
        }
    }
  
  delete(_data);
  _data = tempData;

  calculateHistogram();
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
  delete(visitedPixels);
  calculateHistogram();
}

void Image::getNeighbours(uint32 x, uint32 y, uint16 intensity, bool *visitedPixels){
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
  uint16 cellNr = 0;

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      if (_data[y*_width + x] == L-1){
        cellNr ++;
        findNeighbourhood(x,y, L-1, (L-1)-cellNr*BOUNDDIFF);
      }
    }

  calculateHistogram();
  return cellNr;
}

void Image::getBoudaries(){
  uint32 imgSize = _width * _height * _channels;
  unsigned char *tempData = new unsigned char[imgSize];
  for (uint32 i = 0; i < imgSize; i++){
    tempData[i] = _data[i];
  }
  squareErosion(3);
  for (uint32 i = 0; i < imgSize; i++){
    _data[i] = (int)tempData[i] - (int)_data[i];
  }
  calculateHistogram();
  delete(tempData);
}


void Image::assignCells(uint16 cellNr){
  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bpp);
  uint16 *nrMutations = new uint16[cellNr];
  for (uint16 i = 0; i < cellNr; i++){
    nrMutations[i] = 0;
  }
  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      for (int t = -1; t <= 1; t++)
        for (int s = -1; s <= 1; s++){
          int fx = x+s;
          int fy = y+t;
          if (fx >= 0 && fy >= 0 && fx < _width && fy < _height){
            if (_data[y*_width + x] == L-1 && _data[fy*_width + fx] != 0 && _data[fy*_width + fx] != L-1){
              findNeighbourhood(x, y, L-1, (int)_data[fy*_width + fx] + 1);
              nrMutations[(((L-1) - (int)_data[fy*_width + fx]) / BOUNDDIFF) - 1] += 1;
              std::cout << (((L-1) - (int)_data[fy*_width + fx]) / BOUNDDIFF) - 1 << std::endl;
            }
          }
        }
      if (_data[y*_width + x] == L-1){
        uint32 step = 0;
        while ((int)_data[(y-step)*_width + x] == 0 || (int)_data[(y-step)*_width + x] == L-1 || (int)_data[(y-step)*_width + x] % BOUNDDIFF != 0){
          step++;
        }
        findNeighbourhood(x, y, L-1, (int)_data[(y-step)*_width + x] + 1);
        nrMutations[(((L-1) - (int)_data[(y-step)*_width + x]) / BOUNDDIFF) - 1] += 1;
        std::cout << (((L-1) - (int)_data[(y-step)*_width + x]) / BOUNDDIFF) - 1 << std::endl;
      }
  }
  uint16 sumMutations = 0;
  for (uint16 i = 0; i < cellNr; i++){
    std::cout << "Cell " << i + 1 << ": " << nrMutations[i] << ";" << std::endl;
    sumMutations += nrMutations[i];
  }
  std::cout << std::endl;
  std::cout << "Total: " << sumMutations << std::endl << std::endl;

  delete(nrMutations);
  calculateHistogram();
}