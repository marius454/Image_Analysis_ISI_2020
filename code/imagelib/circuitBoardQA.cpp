#include "image.hpp"
#define iCONNECTORS 64
#define iSOLDERHOLE 240
#define iBACKGROUND 128

void Image::circuitBoardQA(std::string evaluation){
  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++){
      if ((int)_data[y*_width + x] == 0 || (int)_data[y*_width + x] == _L-1){
        localMedianFilter(x, y, 3);
      }
    }

  calculateHistogram();
  for (uint32 i = 0; i < 256; i++){
    if (_histogram[i] != 0) std::cout << "(" << i << ", " << _histogram[i] << ") ";
  }
  std::cout << std::endl;
  
  if (evaluation == "wires"){
    evaluateWires(iCONNECTORS);
  }
  else if (evaluation == "islands"){
    evaluateSolderingIslands();
  }
  else if (evaluation == "holes"){
    evaluateSolderingHoles(iSOLDERHOLE);
  }
  else {
    std::cout << "Command \"" << evaluation << "\" not found for circuit board QA analysis";
    std::exit(0);
  }

  calculateHistogram();
}

void Image::evaluateSolderingIslands(){
  //find 3, 4 or 5 pixel thick lines to find and change the intensity of wires, so that I can find othe object that have the same intensity as wires
  initializeRGB();
  padImage(1.02, 1.02);
  removeWires();
  removeWires();

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      if (_data[y*_width + x] == iCONNECTORS){
        for (int t = -1; t <= 1; t++)
          for (int s = -1; s <= 1; s++){
            int fx = x+s;
            int fy = y+t;
            if (fx >= 0 && fy >= 0 && fx < _width && fy < _height){
              if(_data[fy*_width + fx] == iSOLDERHOLE){
                findNeighbourhood(x, y, iCONNECTORS, iCONNECTORS + 1);
                findNeighbourhood(fx, fy, iSOLDERHOLE, iCONNECTORS + 1);
                evaluateIsland(iCONNECTORS + 1, iCONNECTORS + 2);
              }
            }
          }
      }
    }

  padImage(1.0/1.02, 1.0/1.02);
  combineRGB();
  calculateHistogram();
}

void Image::removeWires(){
  for (uint32 y = 1; y < _height; y++)
    for (uint32 x = 1; x < _width; x++){
      if ((int)_data[y*_width + x] == iCONNECTORS){
        if (((int)_data[(y-1)*_width + x] == iBACKGROUND)
         && ((int)_data[(y+1)*_width + x] == iBACKGROUND || (int)_data[(y+2)*_width + x] == iBACKGROUND || (int)_data[(y+3)*_width + x] == iBACKGROUND
         || (int)_data[(y+4)*_width + x] == iBACKGROUND || (int)_data[(y+5)*_width + x] == iBACKGROUND)){
          uint16 step = 0;
          while ((int)_data[(y+step)*_width + x] == iCONNECTORS){
            _data[(y+step)*_width + x] = static_cast<unsigned char>(iBACKGROUND);
            step++;
          }
        }
        
        if (((int)_data[y*_width + (x-1)] == iBACKGROUND)
         && ((int)_data[y*_width + (x+1)] == iBACKGROUND || (int)_data[y*_width + (x+2)] == iBACKGROUND || (int)_data[y*_width + (x+3)] == iBACKGROUND
         || (int)_data[y*_width + (x+4)] == iBACKGROUND || (int)_data[y*_width + (x+5)] == iBACKGROUND)){
          uint16 step = 0;
          while ((int)_data[y*_width + (x+step)] != iBACKGROUND){
            _data[y*_width + (x+step)] = static_cast<unsigned char>(iBACKGROUND);
            step++;
          }
        }
      }
    }
}
void Image::evaluateIsland(uint16 islandIntensity, uint16 checkedIslandIntensity){
  padImage(1.0/1.02, 1.0/1.02);

  // get the largest unsigned int number
  uint32 minX = -1;
  uint32 minY = -1;

  uint32 maxX = 0;
  uint32 maxY = 0;

  bool isGood;
  float area = 0;

  for (uint32 y = 1; y < _height; y++)
    for (uint32 x = 1; x < _width; x++){
      uint32 i = y*_width + x;
      if ((int)_data[i] == islandIntensity){
        area++;
        if (x > maxX) maxX = x;
        if (y > maxY) maxY = y;
        if (x < minX) minX = x;
        if (y < minY) minY = y;
      }
    }

  float Xdiameter = maxX - minX + 1;
  float Ydiameter = maxY - minY + 1;
  float circleArea = M_PI * pow((Xdiameter)/2.0, 2.0);

  // check if the island is circular
  if (abs(Xdiameter - Ydiameter) < Xdiameter/10.0 && abs(area - circleArea) < area/10.0){
    isGood = true;
  }
  // chech if the island is rectangular
  else if (Xdiameter * Ydiameter == area){
    isGood = true;
  }
  else isGood = false;
  
  for (uint32 i = 0; i < _imgSize; i++){
    if ((int)_data[i] == islandIntensity){
      _data[i] = static_cast<unsigned char>(checkedIslandIntensity);
      if (isGood) fillRGB(i, 0, _L-1, 0);
      else fillRGB(i, _L-1, 0, 0);
    }
  }
  padImage(1.02, 1.02);
}

void Image::evaluateSolderingHoles(uint16 holeIntensity){

}
void Image::evaluateWires(uint16 wireIntensity){
  initializeRGB();
  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++){
      uint32 xy = y*_width + x;
      if (_data[xy] == wireIntensity && _Bdata[xy] != 0){
        findNeighbourhood(x, y, wireIntensity, wireIntensity + 1);
        if (checkForConnection(wireIntensity + 1, iBACKGROUND)){
          fillRGBByIntensity(wireIntensity + 1, 0, _L-1, 0);
        }
        else {
          fillRGBByIntensity(wireIntensity + 1, _L-1, 0, 0);
        }
        findNeighbourhood(x, y, wireIntensity + 1, wireIntensity);
      }
    }
  
  combineRGB();
  calculateHistogram();
}

// checks if the object that is of given intensity has two neighbours that are not the bachground (touching the edge of the image also counts as a neighbour)
// meaning it finds if the neighbourhood has two seperate neighbours (they be of the same intensity so if one is found its intensity needs to be temporarily
// changed so that it can be seperated for another neighbourhood).
bool Image::checkForConnection(uint16 neighbourhoodIntensity, uint16 backgroundIntensity){
  uint8 nrDetected = 0;
  bool edgeDetected = false;
  int revertIntensity = -1;
  int revertX = -1;
  int revertY = -1;

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      if (_data[y*_width + x] == neighbourhoodIntensity){
        for (int t = -1; t <= 1; t++)
          for (int s = -1; s <= 1; s++){
            int fx = x+s;
            int fy = y+t;
            if (fx >= 0 && fy >= 0 && fx < _width && fy < _height){
              if ((int)_data[fy*_width + fx] != backgroundIntensity && (int)_data[fy*_width + fx] != neighbourhoodIntensity
                && (int)_data[fy*_width + fx] != revertIntensity){
                if (nrDetected == 1){
                  if (revertIntensity != -1) findNeighbourhood (revertX, revertY, revertIntensity, revertIntensity - 1);
                  return true;
                }
                else{
                  nrDetected++;
                  revertIntensity = (int)_data[fy*_width + fx] + 1;
                  revertX = fx;
                  revertY = fy;
                  findNeighbourhood(fx, fy, (int)_data[fy*_width + fx], (int)_data[fy*_width + fx] + 1);
                }
              }
            }
            else{
              if (!edgeDetected){
                nrDetected++;
                edgeDetected = true;
              }
              if (nrDetected == 2){
                if (revertIntensity != -1) findNeighbourhood (revertX, revertY, revertIntensity, revertIntensity - 1);
                return true;
              }
            }
          }
      }
    }
  if (revertIntensity != -1) findNeighbourhood (revertX, revertY, revertIntensity, revertIntensity - 1);
  return false;
}

