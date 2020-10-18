#include "image.hpp"

#include <iostream>
#include <string>
#include <Eigen/Core>
#include <Eigen/LU>
#include <math.h>
#include <thread>
#include <chrono>


Eigen::Matrix3f IndexToWorld(BBox bbox, float pixelUnit){
  float height = abs(ceil((bbox.maxY + 1) - bbox.minY));
  Eigen::Matrix3f MinMaxOffset, unitScale, swapY, offsetY;
  MinMaxOffset.setIdentity();
  MinMaxOffset(0,2) = bbox.minX;
  MinMaxOffset(1,2) = bbox.minY;
  unitScale.setIdentity();
  unitScale(0,0) = pixelUnit;
  unitScale(1,1) = pixelUnit;
  swapY.setIdentity();
  swapY(1,1) = -1;
  offsetY.setIdentity();
  offsetY(1,2) = -1 * (height - 1);

  Eigen::Matrix3f I_W = MinMaxOffset * unitScale * swapY * offsetY;
  return I_W;
}

Eigen::Matrix3f WorldToIndex(BBox bbox, float pixelUnit){
  float height = abs(ceil((bbox.maxY + 1) - bbox.minY));
  
  Eigen::Matrix3f MinMaxOffset, unitScale, swapY, offsetY;
  MinMaxOffset.setIdentity();
  MinMaxOffset(0,2) = bbox.minX;
  MinMaxOffset(1,2) = bbox.minY;
  unitScale.setIdentity();
  unitScale(0,0) = pixelUnit;
  unitScale(1,1) = pixelUnit;
  swapY.setIdentity();
  swapY(1,1) = -1;
  offsetY.setIdentity();
  offsetY(1,2) = -1 * (height - 1);

  Eigen::Matrix3f W_I = (MinMaxOffset * unitScale * swapY * offsetY).inverse();
  return W_I;
}

bool Image::manipulateImage(unsigned short n, Eigen::Matrix3f *changeMatrices){
  if (_channels > 1){
    std::cout << "Only grayscale images can be manipulated" << std::endl;
    std::exit(0);
  }

  // bbox = BBox{(float)_width, (float)_height, 0.0f, 0.0f};
  _bbox.maxX = (float)_width - 1;
  _bbox.maxY = (float)_height - 1;
  _bbox.minX = 0.0f;
  _bbox.minY = 0.0f;

  for (unsigned short i = 0; i < n; i++){
    std::cout << changeMatrices[i] << std::endl;
  }
  _indexCoordinates = new Eigen::Vector3i[ _width * _height ];
  _worldCoordinates = new Eigen::Vector3f[ _width * _height ];
  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      _indexCoordinates[ y*_width + x ] = Eigen::Vector3i{x,y,1};
      _worldCoordinates[ y*_width + x ] = Eigen::Vector3f{(float)x, (float)y, 1.0f};
    }
  I_W = IndexToWorld(_bbox, 1);
  
  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      _worldCoordinates[ y*_width + x ] = I_W * _worldCoordinates[ y*_width + x ];
      for (unsigned short i = 0; i < n; i++){
        _worldCoordinates[ y*_width + x ] = changeMatrices[i] * _worldCoordinates[ y*_width + x ];
      }
    }
  recalculateBBox();
  W_I = WorldToIndex(_bbox, 1);
  setIntensities(n, changeMatrices);
  return true;
}

void Image::recalculateBBox(){
  float maxX = std::numeric_limits<float>::min();
	float maxY = std::numeric_limits<float>::min();
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      float wx = _worldCoordinates[ y*_width + x ](0);
      float wy = _worldCoordinates[ y*_width + x ](1);
      if (maxX < wx){ maxX = wx; }
      if (maxY < wy){ maxY = wy; }
      if (minX > wx){ minX = wx; }
      if (minY > wy){ minY = wy; }
    }

  _bbox.maxX = maxX;
  _bbox.maxY = maxY;
  _bbox.minX = minX;
  _bbox.minY = minY;
}

void Image::setIntensities(unsigned short n, Eigen::Matrix3f *changeMatrices){
  std::cout << "good" << std::endl;
  unsigned int newHeight = static_cast<unsigned int>(abs(ceil((_bbox.maxY + 1) - _bbox.minY)));
  unsigned int newWidth = static_cast<unsigned int>(abs(ceil((_bbox.maxX + 1) - _bbox.minX)));
  _IPrimeCoordinates = new Eigen::Vector3f[ newHeight * newWidth ];
  unsigned char* transformedImageData = (unsigned char*)malloc( newHeight * newWidth );
  std::cout << "New Height: " << newHeight << std::endl;
  std::cout << "New Width: " << newWidth << std::endl;
  Eigen::Matrix3f invW_I = W_I.inverse();
  Eigen::Matrix3f invI_W = I_W.inverse();
  for (unsigned int y = 0; y < 3; y++)
    for (unsigned int x = 0; x < 3; x++){
      if (invW_I(y, x) == -0.0f){
        invW_I(y, x) = 0;
      }
      if (invI_W(y, x) == -0.0f){
        invI_W(y, x) = 0;
      }
    }

  for (unsigned int y = 0; y < newHeight; y++)
    for (unsigned int x = 0; x < newWidth; x++){
      _IPrimeCoordinates[ y*newWidth + x ] = Eigen::Vector3f{(float)x, (float)y, 1.0f};
      Eigen::Vector3f indexVec = invW_I * _IPrimeCoordinates[ y*newWidth + x ];
      for (short i = n-1; i >= 0; i--){
        indexVec = changeMatrices[i].inverse() * indexVec;
      }
      indexVec = invI_W * indexVec;
      if (indexVec(0) >= 0 && indexVec(0) <= _width && indexVec(1) >= 0 && indexVec(1) <= _height){
        transformedImageData[ y*newWidth + x ] = NN(indexVec);
      } else{
        transformedImageData[ y*newWidth + x ] = static_cast<unsigned char>(0);
      }
    }
  free(_data);
  std::cout << "good" << std::endl;
  _data = transformedImageData;
  _height = newHeight;
  _width = newWidth;
  std::cout << "good" << std::endl;
}

unsigned char Image::NN(Eigen::Vector3f indexVec){
  unsigned int old_x = static_cast<unsigned int>(round(indexVec(0)));
  unsigned int old_y = static_cast<unsigned int>(round(indexVec(1)));

  return _data[ old_y * _width + old_x ];
}