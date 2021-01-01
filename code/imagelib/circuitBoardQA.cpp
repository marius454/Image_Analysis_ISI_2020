#include "image.hpp"
#define iCONNECTORS 64
#define iSOLDERHOLE 250
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

void Image::evaluateWires(uint16 wireIntensity){
  initializeRGB();
  uint32 check = 0;
  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++){
      uint32 xy = y*_width + x;
      if (_data[xy] == wireIntensity && _Bdata[xy] != 0){
        check ++;
        std::cout << check << std::endl;
        findNeighbourhood(x, y, wireIntensity, wireIntensity + 1);
        if (checkForConnection(wireIntensity + 1, iBACKGROUND) == true){
          for (uint32 i = 0; i < _imgSize; i++){
            if (_data[i] == wireIntensity + 1){
              _Gdata[i] = _L-1;
              _Rdata[i] = 0;
              _Bdata[i] = 0;
            }
          }
        }
        else {
          for (uint32 i = 0; i < _imgSize; i++){
            if (_data[i] == wireIntensity + 1){
              _Rdata[i] = _L-1;
              _Gdata[i] = 0;
              _Bdata[i] = 0;
            }
          }
        }
        findNeighbourhood(x, y, wireIntensity + 1, wireIntensity);
      }
    }


  calculateHistogram();
  for (uint32 i = 0; i < 256; i++){
    if (_histogram[i] != 0) std::cout << "(" << i << ", " << _histogram[i] << ") ";
  }
  std::cout << std::endl;
  
  combineRGB();
  calculateHistogram();
}
void Image::evaluateSolderingIslands(){

}
void Image::evaluateSolderingHoles(uint16 holeIntensity){

}

// checks if the object that is of given intensity has two neighbours that are not the bachground (touching the edge of the image also counts as a neighbour)
// meaning it finds if the neighbourhood has two seperate neighbours (they be of the same intensity so if one is found its intensity needs to be temporarily
// changed so that it can be seperated for another neighbourhood).
bool Image::checkForConnection(uint16 neighbourgoodIntensity, uint16 backgroundIntensity){
  uint8 nrDetected = 0;
  bool edgeDetected = false;
  int revertIntensity = -1;
  int revertX = -1;
  int revertY = -1;

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      for (int t = -1; t <= 1; t++){
        if (_data[y*_width + x] == neighbourgoodIntensity){
          for (int s = -1; s <= 1; s++){
            int fx = x+s;
            int fy = y+t;
            if (fx >= 0 && fy >= 0 && fx < _width && fy < _height){
              if ((int)_data[fy*_width + fx] != backgroundIntensity && (int)_data[fy*_width + fx] != neighbourgoodIntensity
               && (int)_data[fy*_width + fx] != revertIntensity){
                std::cout << (int)_data[fy*_width + fx] << " " << revertIntensity << std::endl;
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
                std::cout << -1 << std::endl;
              }
              if (nrDetected == 2){
                std::cout << -1 << std::endl;
                if (revertIntensity != -1) findNeighbourhood (revertX, revertY, revertIntensity, revertIntensity - 1);
                return true;
              }
            }
          }
        }
      }
    }
  if (revertIntensity != -1) findNeighbourhood (revertX, revertY, revertIntensity, revertIntensity - 1);
  return false;
}
