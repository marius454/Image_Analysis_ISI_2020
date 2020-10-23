#include "image.hpp"


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
  Eigen::Matrix3f I_W = IndexToWorld(bbox, pixelUnit);

  Eigen::Matrix3f W_I = I_W.inverse();
  return W_I;
}

float degreesToRadians(float degree){
  float pi = 3.14159265359;
  return (degree * (pi / 180));
}

void Image::scaleImage(std::string interpolationScheme, float scaleX, float scaleY){
  Eigen::Matrix3f m;
  m.setIdentity();
  m(0,0) = scaleX;
  m(1,1) = scaleY;
  Eigen::Matrix3f* ms = new Eigen::Matrix3f[1];
  ms[0] = m;
  if (interpolationScheme == "nn") manipulateImage(1, ms, true);
  else if (interpolationScheme == "bilin") manipulateImage(1, ms, false);
  else {
    std::cout << "Invalid interpolation scheme, must be 'nn' or 'bilin'";
    std::exit(0);
  }
}

void Image::rotateImage(std::string interpolationScheme, float degreeOfRotation){
  degreeOfRotation = degreesToRadians(degreeOfRotation);
  Eigen::Matrix3f m;
  m.setIdentity();
  m(0,0) = cos(degreeOfRotation);
  m(1,1) = cos(degreeOfRotation);
  m(1,0) = -sin(degreeOfRotation);
  m(0,1) = sin(degreeOfRotation);
  Eigen::Matrix3f* ms = new Eigen::Matrix3f[1];
  ms[0] = m;
  if (interpolationScheme == "nn") manipulateImage(1, ms, true);
  else if (interpolationScheme == "bilin") manipulateImage(1, ms, false);
  else {
    std::cout << "Invalid interpolation scheme, must be 'nn' or 'bilin'";
    std::exit(0);
  }
}

void Image::shearImage(std::string interpolationScheme, float shearAmount){
  Eigen::Matrix3f m;
  m.setIdentity();
  m(0,1) = shearAmount;
  Eigen::Matrix3f* ms = new Eigen::Matrix3f[1];
  ms[0] = m;
  if (interpolationScheme == "nn") manipulateImage(1, ms, true);
  else if (interpolationScheme == "bilin") manipulateImage(1, ms, false);
  else {
    std::cout << "Invalid interpolation scheme, must be 'nn' or 'bilin'";
    std::exit(0);
  }
}

bool Image::manipulateImage(unsigned short n, Eigen::Matrix3f *changeMatrices, bool userNN){
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

  I_W = IndexToWorld(_bbox, 1);
  recalculateBBox(n, changeMatrices);
  W_I = WorldToIndex(_bbox, 1);
  setIntensities(n, changeMatrices, userNN);
  return true;
}

void Image::recalculateBBox(unsigned short n, Eigen::Matrix3f *changeMatrices){
  float maxX = std::numeric_limits<float>::min();
	float maxY = std::numeric_limits<float>::min();
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();

  Eigen::Vector3f* minMax = new Eigen::Vector3f[4];
  minMax[0] = Eigen::Vector3f{_bbox.minX+1, _bbox.minY+1, 1.0f};
  minMax[1] = Eigen::Vector3f{_bbox.minX+1, _bbox.maxY, 1.0f};
  minMax[2] = Eigen::Vector3f{_bbox.maxX, _bbox.minY+1, 1.0f};
  minMax[3] = Eigen::Vector3f{_bbox.maxX, _bbox.maxY, 1.0f};

  for (int i = 0; i < 4; i++){
    minMax[i] = I_W * minMax[i];
    for (unsigned short j = 0; j < n; j++){
      minMax[i] = changeMatrices[j] * minMax[i];
    }
  }

  for (int i = 0; i < 4; i++){
    float wx = minMax[i](0);
    float wy = minMax[i](1);
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

void Image::setIntensities(unsigned short n, Eigen::Matrix3f *changeMatrices, bool useNN){
  unsigned int newHeight = static_cast<unsigned int>(abs(ceil((_bbox.maxY + 1) - _bbox.minY)));
  unsigned int newWidth = static_cast<unsigned int>(abs(ceil((_bbox.maxX + 1) - _bbox.minX)));
  unsigned char* transformedImageData = (unsigned char*)malloc( newHeight * newWidth );
  std::cout << "New Height: " << newHeight << std::endl;
  std::cout << "New Width: " << newWidth << std::endl;
  Eigen::Matrix3f invW_I = W_I.inverse();
  Eigen::Matrix3f invI_W = I_W.inverse();

  for (unsigned int y = 1; y <= newHeight; y++)
    for (unsigned int x = 1; x <= newWidth; x++){
      Eigen::Vector3f indexVec = Eigen::Vector3f{(float)x, (float)y, 1.0f};
      indexVec = invW_I * indexVec;
      for (short i = n-1; i >= 0; i--){
        indexVec = changeMatrices[i].inverse() * indexVec;
      }
      indexVec = invI_W * indexVec;
      indexVec(0) = indexVec(0) - 1;
      indexVec(1) = indexVec(1) - 1;
      if (indexVec(0) >= 0 && indexVec(0) <= _width - 1 && indexVec(1) >= 0 && indexVec(1) <= _height - 1){
        if (useNN) transformedImageData[ (y-1)*newWidth + (x-1) ] = NN(indexVec);
        else transformedImageData[ (y-1)*newWidth + (x-1) ] = bilinearInterpolation(indexVec);
      } else{
        transformedImageData[ (y-1)*newWidth + (x-1) ] = static_cast<unsigned char>(0);
      }
    }
  free(_data);
  _data = transformedImageData;
  _height = newHeight;
  _width = newWidth;
}

unsigned char Image::NN(Eigen::Vector3f indexVec){
  unsigned int old_x = static_cast<unsigned int>(round(indexVec(0)));
  unsigned int old_y = static_cast<unsigned int>(round(indexVec(1)));

  return _data[ old_y * _width + old_x ];
}

unsigned char Image::bilinearInterpolation(Eigen::Vector3f indexVec){
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

// leaving this here so i dont foget the syntax
// std::this_thread::sleep_for(std::chrono::seconds(30));