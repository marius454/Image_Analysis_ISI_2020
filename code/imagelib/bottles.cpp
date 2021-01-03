#include "image.hpp"

void Image::bottles(){
  float x[] = {0.2, 0.8};
  float y[] = {0.5, 1};
  getCutOut(0, 67, _width - 1, 90);
  contrastStretching(2, x, y, 1);
  fullMedianFilter(3);
  calculateHistogram();
  checkbottles(10);
}

// add differentiation of bottles
void Image::checkbottles(uint16 nrChecks){
  initializeRGB();
  uint32 startX, endX;
  uint16 bottleNr = 0;
  for (int x = 0; x < _width; x++) {
    uint16 prevPixel = _L-2;
    uint16 nextPixel = _L-2;
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
      findNeighbourhood(x, 0, _L-1, _L-2);
      startX = x;
    }
    if (x == _width - 1){
      nextPixel = 0;
    }
    else {
      prevPixel = (int)_data[x - 1];
      nextPixel = (int)_data[x + 1];
    }
    if ((x == _width - 1 && _data[x] == _L-2) || (nextPixel != _L-2 && _data[x] == _L-2)){
      endX = x;
      uint32 distance = endX - startX;
      uint16 goodChecks = 0;
      //tracking checks that were actually done just in case a mistake occurs somewhere.
      uint16 doneChecks = 0;
      
      for (float i = startX + ((float)distance/(float)(nrChecks)) / 2; i <= endX; i = i + (float)distance/(float)(nrChecks)){
        int yStep = _height - 1;
        while (_data[yStep*_width + (int)i] != _L-2){
          yStep--;
          if (yStep == 0){
            break;
          }
        }
        doneChecks++;
        bool goodFill;
        if (yStep <= _height / 2){
          goodFill = true;
          goodChecks++;
        }
        else goodFill = false;
        while (yStep != -1){
          uint32 xy = yStep*_width + (int)i;
          yStep--;
          _data[xy] = 50;
          if (goodFill) fillRGB(xy, 0, _L-1, 0);
          else fillRGB(xy, _L-1, 0, 0);
        }
      }

      
      bottleNr++;
      std::cout << "Bottle Nr. " << bottleNr << " - ";
      if(goodChecks < (int)ceil((float)doneChecks*0.4)){
        fillRGBByIntensity(_L-2, _L-1, 0, 0);
        std::cout << "BAD" << std::endl;
      } 
      else if (goodChecks < (int)ceil((float)doneChecks*0.7) + 1){
        fillRGBByIntensity(_L-2, _L-1, _L-1, 0);
        std::cout << "WARNING" << std::endl;
      }
      else{
        fillRGBByIntensity(_L-2, 0, _L-1, 0);
        std::cout << "GOOD" << std::endl;
      }
      std::cout << "Number of checks done: " << doneChecks << std::endl
       << "Number of checks that gave the desired result: " << goodChecks << std::endl << std::endl;

      for (uint32 i = 0; i < _imgSize; i++){
        if ((int)_data[i] == _L-2) _data[i] = static_cast<unsigned char>(_L-1);
      }
    }
  }
  combineRGB();
  calculateHistogram();
}