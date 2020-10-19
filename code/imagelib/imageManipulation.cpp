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
  // std::cout << MinMaxOffset << std::endl;
  // std::cout << std::endl;

  unitScale.setIdentity();
  unitScale(0,0) = pixelUnit;
  unitScale(1,1) = pixelUnit;
  // std::cout << unitScale << std::endl;
  // std::cout << std::endl;

  swapY.setIdentity();
  swapY(1,1) = -1;
  // std::cout << swapY << std::endl;
  // std::cout << std::endl;

  offsetY.setIdentity();
  offsetY(1,2) = -1 * (height - 1);
  // std::cout << offsetY << std::endl;
  // std::cout << std::endl;

  Eigen::Matrix3f I_W = MinMaxOffset * unitScale * swapY * offsetY;
  // std::cout << I_W << std::endl;
  // std::cout << std::endl;
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

  _bbox.maxX = (float)_width;
  _bbox.maxY = (float)_height;
  _bbox.minX = 0.0f;
  _bbox.minY = 0.0f;

  for (unsigned short i = 0; i < n; i++){
    std::cout << changeMatrices[i] << std::endl;
  }
  // _width = _width + 1;
  // _height = _height + 1;
  unsigned int check = 0;
  _worldCoordinates = new Eigen::Vector3f[ _width * _height ];
  for (int y = 1; y <= _height; y++)
    for (int x = 1; x <= _width; x++){
      check++;
      _worldCoordinates[ (y-1)*_width + (x-1) ] = Eigen::Vector3f{(float)x, (float)y, 1.0f};
    }
  std::cout << check << std::endl << std::endl;
  std::cout << _worldCoordinates[0] << std::endl << std::endl;
  std::cout << _worldCoordinates[_width * _height - 1] << std::endl << std::endl;
  I_W = IndexToWorld(_bbox, 1);
  
  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      _worldCoordinates[ y*_width + x ] = I_W * _worldCoordinates[ y*_width + x ];
      for (unsigned short i = 0; i < n; i++){
        _worldCoordinates[ y*_width + x ] = changeMatrices[i] * _worldCoordinates[ y*_width + x ];
      }
    }
  std::cout << _worldCoordinates[_width * (_height - 1)] << std::endl;
  std::cout << std::endl;
  std::cout << _worldCoordinates[_width - 1] << std::endl;
  std::cout << std::endl;
  // std::this_thread::sleep_for(std::chrono::seconds(30));

  recalculateBBox();
  W_I = WorldToIndex(_bbox, 1);
  // _width = _width - 1;
  // _height = _height - 1;
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

  std::cout << _bbox.maxX << std::endl;
  std::cout << _bbox.maxY << std::endl;
  std::cout << _bbox.minX << std::endl;
  std::cout << _bbox.minY << std::endl;
}

void Image::setIntensities(unsigned short n, Eigen::Matrix3f *changeMatrices){
  unsigned int newHeight = static_cast<unsigned int>(abs(ceil((_bbox.maxY + 1) - _bbox.minY)));
  unsigned int newWidth = static_cast<unsigned int>(abs(ceil((_bbox.maxX + 1) - _bbox.minX)));
  _IPrimeCoordinates = new Eigen::Vector3f[ newHeight * newWidth ];
  unsigned char* transformedImageData = (unsigned char*)malloc( newHeight * newWidth );
  std::cout << "New Height: " << newHeight << std::endl;
  std::cout << "New Width: " << newWidth << std::endl;
  Eigen::Matrix3f invW_I = W_I.inverse();
  Eigen::Matrix3f invI_W = I_W.inverse();

  for (unsigned int y = 1; y <= newHeight; y++)
    for (unsigned int x = 1; x <= newWidth; x++){
      _IPrimeCoordinates[ (y-1)*newWidth + (x-1) ] = Eigen::Vector3f{(float)x, (float)y, 1.0f};
      Eigen::Vector3f indexVec = invW_I * _IPrimeCoordinates[ (y-1)*newWidth + (x-1) ];
      for (short i = n-1; i >= 0; i--){
        indexVec = changeMatrices[i].inverse() * indexVec;
      }
      indexVec = invI_W * indexVec;
      indexVec(0) = indexVec(0) - 1;
      indexVec(1) = indexVec(1) - 1;
      if (indexVec(0) >= 0 && indexVec(0) <= _width - 1 && indexVec(1) >= 0 && indexVec(1) <= _height - 1){
        transformedImageData[ (y-1)*newWidth + (x-1) ] = NN(indexVec);
        //transformedImageData[ y*newWidth + x ] = BilinearInterpolation(indexVec);
      } else{
        transformedImageData[ (y-1)*newWidth + (x-1) ] = static_cast<unsigned char>(0);
      }
      //std::cout << "(" << x-1 << ", " << y-1 << "): " << (int)transformedImageData[ (y-1)*newWidth + (x-1) ] << std::endl;
    }
  std::cout << "good" << std::endl;
  free(_data);
  std::cout << "good" << std::endl;
  _data = transformedImageData;
  _height = newHeight;
  _width = newWidth;
}

unsigned char Image::NN(Eigen::Vector3f indexVec){
  unsigned int old_x = static_cast<unsigned int>(round(indexVec(0)));
  unsigned int old_y = static_cast<unsigned int>(round(indexVec(1)));

  //std::cout << indexVec(0) << " " << indexVec(1) << " " << (int)_data[ old_y * _width + old_x ] << std::endl;

  return _data[ old_y * _width + old_x ];
}

unsigned char Image::BilinearInterpolation(Eigen::Vector3f indexVec){
  float old_x = indexVec(0);
  float old_y = indexVec(1);
  Eigen::Vector2i p0, p1, p2, p3;

  if (old_x == (int)old_x && old_y == (int)old_y){
    return _data[ (int)old_y * _width + (int)old_x ];
  }
  else if (old_x == (int)old_x){
    p0(0) = static_cast<unsigned int>(old_x);
    p0(1) = static_cast<unsigned int>(floor(old_y));
    p1(0) = static_cast<unsigned int>(old_x);
    p1(1) = static_cast<unsigned int>(ceil(old_y));
    unsigned short v0 = static_cast<unsigned short>(_data[ p0(1)*_width + p0(0) ]);
    unsigned short v1 = static_cast<unsigned short>(_data[ p1(1)*_width + p1(0) ]);

    old_y = old_y - floor(old_y);
    float vs = (1 - old_y)*v0 + old_y*v1;

    return static_cast<unsigned char>((int)round(vs));
  } 
  else if (old_y == (int)old_y){
    p0(0) = static_cast<unsigned int>(floor(old_x));
    p0(1) = static_cast<unsigned int>(old_y);
    p1(0) = static_cast<unsigned int>(ceil(old_x));
    p1(1) = static_cast<unsigned int>(old_y);
    unsigned short v0 = static_cast<unsigned short>(_data[ p0(1)*_width + p0(0) ]);
    unsigned short v1 = static_cast<unsigned short>(_data[ p1(1)*_width + p1(0) ]);

    old_x = old_x - floor(old_x);
    float vs = (1 - old_x)*v0 + old_x*v1;
    return static_cast<unsigned char>((int)round(vs));
  }
  else {
    p0(0) = static_cast<unsigned int>(floor(old_x));
    p0(1) = static_cast<unsigned int>(floor(old_y));
    p1(0) = static_cast<unsigned int>(ceil(old_x));
    p1(1) = static_cast<unsigned int>(floor(old_y));
    p2(0) = static_cast<unsigned int>(floor(old_x));
    p2(1) = static_cast<unsigned int>(ceil(old_y));
    p3(0) = static_cast<unsigned int>(ceil(old_x));
    p3(1) = static_cast<unsigned int>(ceil(old_y));
    unsigned short v0 = static_cast<unsigned short>(_data[ p0(1)*_width + p0(0) ]);
    unsigned short v1 = static_cast<unsigned short>(_data[ p1(1)*_width + p1(0) ]);
    unsigned short v2 = static_cast<unsigned short>(_data[ p2(1)*_width + p2(0) ]);
    unsigned short v3 = static_cast<unsigned short>(_data[ p3(1)*_width + p3(0) ]);

    old_x = old_x - floor(old_x);
    old_y = old_y - floor(old_y);

    float w0 = (1 - old_x) * (1 - old_y);
    float w1 = old_x * (1 - old_y);
    float w2 = (1 - old_x) * old_y;
    float w3 = old_x * old_y;

    float vs = w0*v0 + w1*v1 + w2*v2 + w3*v3;
    return static_cast<unsigned char>((int)round(vs));
  }
}

//std::cout << "good" << std::endl;