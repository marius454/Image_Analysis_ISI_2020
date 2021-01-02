#include "image.hpp"

void Image::bottles(){
  float x[] = {0.2, 0.8};
  float y[] = {0.5, 1};
  getCutOut(0, 67, 497, 90);
  contrastStretching(2, x, y, 1);
  fullMedianFilter(3);
  checkbottles(10);
}

// add differentiation of bottles
void Image::checkbottles(uint16 nrChecks){
  initializeRGB();
  uint32 startX, endX;
  for (int x = 0; x < _width; x++) {
    uint16 prevPixel = _L-1;
    uint16 nextPixel = _L-1;
    if (x == 0){
      prevPixel = 0;
    }
    else if (x == _width - 1){
      nextPixel = 0;
    }
    else {
      prevPixel = (int)_data[x - 1];
      nextPixel = (int)_data[x + 1];
    }
    if ((x == 0 && _data[x] == _L-1) || (prevPixel != _L-1 && _data[x] == _L-1)){
      // findNeighbourhood(x, 0, _L-1, _L-2);
      startX = x;
    }
    if ((x == _width - 1 && _data[x] == _L-1) || (nextPixel != _L-1 && _data[x] == _L-1)){
      endX = x;
      uint32 distance = endX - startX;
      for (uint32 i = startX + (distance/nrChecks) / 2; i < endX; i = i + distance/nrChecks){
        int yStep = _height - 1;
        while (_data[yStep*_width + i] != _L-1){
          yStep--;
          if (yStep == 0){
            break;
          }
        }
        bool goodFill;
        if (yStep <= _height / 2) goodFill = true;
        else goodFill = false;
        while (yStep != -1){
          uint32 xy = yStep*_width + i;
          yStep--;
          // _data[xy] = 50;
          if (goodFill) {
            _Gdata[xy] = _L-1;
            _Rdata[xy] = 0;
            _Bdata[xy] = 0;
          }
          else {
            _Rdata[xy] = _L-1;
            _Gdata[xy] = 0;
            _Bdata[xy] = 0;
          }
        }
      }
    }
  }
  combineRGB();
  calculateHistogram();
}