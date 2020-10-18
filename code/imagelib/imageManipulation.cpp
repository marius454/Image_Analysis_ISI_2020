#include "image.hpp"

#include <iostream>
#include <string>
#include <Eigen/Core>
#include <Eigen/LU>

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
  IndexToWorld(1);
  
  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      for (unsigned short i = 0; i < n; i++){
        _worldCoordinates[ y*_width + x ] = changeMatrices[i] * _worldCoordinates[ y*_width + x ];
      }
    }
  recalculateBBox();
  WorldToIndex(1);
  std::cout << _indexCoordinates[_width * (_height - 1)] << std::endl;
  std::cout << _worldCoordinates[_width * (_height - 1)] << std::endl;
  std::cout << "Bounding box:" << std::endl;
  std::cout << "(minX, minY) = (" << _bbox.minX << ", " << _bbox.minY << ")" <<std::endl;
  std::cout << "(maxX, maxY) = (" << _bbox.maxX << ", " << _bbox.maxY << ")" <<std::endl;
  std::exit(0);
  return true;
}

void Image::IndexToWorld(float pixelUnit){
  Eigen::Matrix3f MinMaxOffset, unitScale, swapY, offsetY;
  MinMaxOffset.setIdentity();
  MinMaxOffset(0,2) = _bbox.minX;
  MinMaxOffset(1,2) = _bbox.minY;
  unitScale.setIdentity();
  unitScale(0,0) = pixelUnit;
  unitScale(1,1) = pixelUnit;
  swapY.setIdentity();
  swapY(1,1) = -1;
  offsetY.setIdentity();
  offsetY(1,2) = -1 * ((float)_height - 1);

  I_W = MinMaxOffset * unitScale * swapY * offsetY;
  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      _worldCoordinates[ y*_width + x ] = I_W * _worldCoordinates[ y*_width + x ];
    }
}

void Image::WorldToIndex(float pixelUnit){
  Eigen::Matrix3f MinMaxOffset, unitScale, swapY, offsetY;
  MinMaxOffset.setIdentity();
  MinMaxOffset(0,2) = _bbox.minX;
  MinMaxOffset(1,2) = _bbox.minY;
  unitScale.setIdentity();
  unitScale(0,0) = pixelUnit;
  unitScale(1,1) = pixelUnit;
  swapY.setIdentity();
  swapY(1,1) = -1;
  offsetY.setIdentity();
  offsetY(1,2) = -1 * ((float)((_bbox.maxY + 1) - _bbox.minY) - 1);

  W_I = (MinMaxOffset * unitScale * swapY * offsetY).inverse();
  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      _worldCoordinates[ y*_width + x ] = W_I * _worldCoordinates[ y*_width + x ];
    }
}

void Image::recalculateBBox(){
  float maxX = std::numeric_limits<float>::min();
	float maxY = std::numeric_limits<float>::min();
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();

  std::cout << "good" << std::endl;
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
  // _height = (maxY + 1) - minY;
  // _width = (maxX + 1) - minX;
}