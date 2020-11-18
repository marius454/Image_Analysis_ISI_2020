#include "image.hpp"


void Image::calculateHistogram(){
  uint32 imgSize = _width * _height * _channels;
  for (uint32 i = 0; i < 256; i++){
    _histogram[ i ] = 0;
  }
  for (uint32 i = 0; i < imgSize; i++){
    uint32 intensity = (int)_data[i];
    _histogram[ intensity ] ++;
  }
}

void Image::transformPixels(){
  uint32 imgSize = _height * _width * _channels;
  for (uint32 i = 0; i < imgSize; i++){
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

void Image::intensityPowerLaw(float gamma, bool isFloat){
  uint16 L = pow(2, _bpp);

  if (isFloat){
    uint32 imgSize = _width * _height * _channels;
    for (uint32 i = 0; i < imgSize; i++){
      float scaledPixel = _floatData[i]/(L-1);
      _floatData[i] = pow(scaledPixel, gamma) * (L-1);
    }
  }
  else {
    _lookupTable[0] = pow(0.5f / float(L-1), gamma) * (L-1);
    for (uint16 i = 1; i < L; i++){
      float scaledPixel = (float)i/(float)(L-1);
      _lookupTable[i] = pow(scaledPixel, gamma) * (L-1);
    }
    transformPixels();
    calculateHistogram();
  }
}

void Image::contrastStretching(uint16 numberOfSlopeChangePoints, float* slopeChangeFractionPoints, float* desiredValueFractionsAtPoints, uint8 algorithm /*= 0*/){
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
    intervals[numberOfSlopeChangePoints] = Interval(point2, "left", L);

    for (uint16 i = 1; i < numberOfSlopeChangePoints; i++){
      Eigen::Vector2f left = Eigen::Vector2f{round(slopeChangeFractionPoints[i-1] * (float)L),
       round(desiredValueFractionsAtPoints[i-1] * (float)L)};
      Eigen::Vector2f right = Eigen::Vector2f{round(slopeChangeFractionPoints[i] * (float)L),
       round(desiredValueFractionsAtPoints[i] * (float)L)};

      intervals[i] = Interval(left, right);
    }
  }
  else{
    std::cout << "Invalid number of slope points" << std::endl;
  }
  
  if(algorithm == 0){
    uint16 point = 0;
    for (uint16 i = 0; i < L; i++){
      if ((float)i == intervals[point].getRight()(0) &&
       intervals[point].getRight()(0) != 255 && intervals[point].getRight()(0) != 256) {
        point ++;
      }
      _lookupTable[i] = intervals[point].linearInterpolation(i);
    }
  }
  else if (algorithm == 1){
    uint16 point = 0;
    for (uint16 i = 0; i < L; i++){
      if ((float)i == intervals[point].getRight()(0) &&
       intervals[point].getRight()(0) != 255 && intervals[point].getRight()(0) != 256) {
        point ++;
      }
      _lookupTable[i] = intervals[point].threshold(i);
    }
  }
  else if (algorithm == 2){
    uint16 point = 0;
    for (uint16 i = 0; i < L; i++){
      if ((float)i == intervals[point].getRight()(0) &&
       intervals[point].getRight()(0) != 255 && intervals[point].getRight()(0) != 256) {
        point ++;
      }
      if (point == 0 || point == numberOfSlopeChangePoints){
        _lookupTable[i] = i;
      }
      else{
        _lookupTable[i] = intervals[point].threshold(i);
      }
    }
  }
  else {
    std::cout << "Invalid algorithm identifier (must bet between 0 and 2)";
  }
  
  transformPixels();
  calculateHistogram();
}

void Image::histogramNormalization(){
  uint16 L = pow(2, _bpp);
  float imgSize = (float)(_height * _width * _channels);
  for (uint16 i = 0; i < L; i++){
    float sumPr = 0;
    for (uint16 j = 0; j < i; j++){
      sumPr += (float)_histogram[j] / imgSize;
    }
    _lookupTable[i] = (L - 1) * sumPr;
  }

  transformPixels();
  calculateHistogram();
}

