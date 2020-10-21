#include "image.hpp"


void Image::calculateHistogram(){
  unsigned long size = _width * _height * _channels;
  for (uint32 i = 0; i < 256; i++){
    _histogram[ i ] = 0;
  }
  for (uint32 i = 0; i < size; i++){
    uint32 intensity = (int)_data[i];
    _histogram[ intensity ] ++;
  }
}

void Image::transformPixels(){
  for (uint32 i = 0; i < (_height * _width * _channels); i++){
    if (round(_lookupTable[ (int)_data[i] ]) > pow(2, _bpp) - 1){
      _data[i] = static_cast<unsigned char>(255);
    } 
    else{
      _data[i] = static_cast<unsigned char>(round(_lookupTable[ (int)_data[i] ]));
    }
    
  }
}

void Image::intensityNegate(){
  uint16 L = pow(2, _bpp);
  for (uint16 i = 0; i < L; i++){
    _lookupTable[i] = (L - 1) - i;
  }
  transformPixels();
  calculateHistogram();
}
void Image::intensityPowerLaw(float a, float gamma){
  uint16 L = pow(2, _bpp);
  for (uint16 i = 0; i < L; i++){
    float scaledPixel = (float)i/(float)(L-1);
    _lookupTable[i] = (a * pow(scaledPixel, gamma)) * 255;
  }
  transformPixels();
  calculateHistogram();
}

void Image::contrastStretching(uint16 numberOfSlopeChangePoints, float* slopeChangeFractionPoints, float* desiredValueFractionsAtPoints){
  uint16 L = pow(2, _bpp);
  Interval* intervals = new Interval[numberOfSlopeChangePoints + 1];

  if (numberOfSlopeChangePoints == 1){
    Eigen::Vector2f middle = Eigen::Vector2f{round(slopeChangeFractionPoints[0] * (float)L),
     round(desiredValueFractionsAtPoints[0] * (float)L)};

    intervals[0] = Interval(middle, "right", L);
    intervals[1] = Interval(middle, "left", L);
  }
  else if (numberOfSlopeChangePoints == 2){
    Eigen::Vector2f point1 = Eigen::Vector2f{round(slopeChangeFractionPoints[0] * (float)L),
     round(desiredValueFractionsAtPoints[0] * (float)L)};
    Eigen::Vector2f point2 = Eigen::Vector2f{round(slopeChangeFractionPoints[1] * (float)L),
     round(desiredValueFractionsAtPoints[1] * (float)L)};

    intervals[0] = Interval(point1, "right", L);
    intervals[1] = Interval(point1, point2);
    intervals[2] = Interval(point2, "left", L);
  }
  else if (numberOfSlopeChangePoints > 2){
    Eigen::Vector2f point1 = Eigen::Vector2f{round(slopeChangeFractionPoints[0] * (float)L),
     round(desiredValueFractionsAtPoints[0] * (float)L)};
    Eigen::Vector2f point2 = Eigen::Vector2f{round(slopeChangeFractionPoints[numberOfSlopeChangePoints - 1] * (float)L),
     round(desiredValueFractionsAtPoints[numberOfSlopeChangePoints - 1] * (float)L)};

    intervals[0] = Interval(point1, "right", L);
    intervals[2] = Interval(point2, "left", L);

    for (uint16 i = 1; i < numberOfSlopeChangePoints - 1; i++){
      Eigen::Vector2f left = Eigen::Vector2f{round(slopeChangeFractionPoints[i] * (float)L),
       round(desiredValueFractionsAtPoints[i] / (float)L)};
      Eigen::Vector2f right = Eigen::Vector2f{round(slopeChangeFractionPoints[i+1] * (float)L),
       round(desiredValueFractionsAtPoints[i+1] / (float)L)};

      intervals[i] = Interval(left, right);
    }
  }
  else{
    std::cout << "Invalid number of slope points" << std::endl;
  }
  uint16 point = 0;
  for (uint16 i = 0; i < L; i++){
    _lookupTable[i] = intervals[point].linearInterpolation(i);
    if ((float)i == intervals[point].getRight()(0)) {
      point ++;
    }
  }
  transformPixels();
  calculateHistogram();
}

Interval::Interval(Eigen::Vector2f singleVec, std::string side, uint16 L){
  if (side == "left"){
    _left = singleVec;
    _right = Eigen::Vector2f{(float)(L - 1), (float)(L - 1)};
  }
  else if (side == "right"){
    _right = singleVec;
    _left = Eigen::Vector2f{0, 0};
  }
  else {
    std::cout << "Side selection must be \"left\" or \"right\"" << std::endl;
  }
}

Interval::Interval(Eigen::Vector2f left, Eigen::Vector2f right){
  _left = left;
  _right = right;
}

float Interval::linearInterpolation(float x){
  float x0 = _left(0);
  float x1 = _right(0);
  float y0 = _left(1);
  float y1 = _right(1);
  std::cout << x << " " << x0 << " " << y0 << " " << x1 << " " << y1 << " ";

  if (x == x0) { 
    std::cout << y0 << std::endl;
    return y0; 
  }
  else if ( x == x1 ) { 
    std::cout << y1 << std::endl;
    return y1; 
  }
  else { 
    std::cout << y0 + ((y1-y0)/(x1-x0)) * (x - x0) << std::endl;
    return (y0 + ((y1-y0)/(x1-x0)) * (x - x0)); 
  }
}

Eigen::Vector2f Interval::getLeft(){ return _left; }
Eigen::Vector2f Interval::getRight(){ return _right; }

