#include "image.hpp"

// Define the setting for high pass and low pass filtering
#define LOW 0
#define HIGH 1
// Define the setting for type of filtering
#define IDEAL 0
#define BUTTERWORTH 1
#define GAUSSIAN 2

void validation (uint8 type, uint8 pass);

void Image::frequencyFilter(uint8 type, uint8 pass, uint32 D0, uint8 n){
  validation(type, pass);
  uint32 imgSize = _width * _height * _channels;
  float* filter = new float[imgSize];
  padImage(2, 2);
  shiftForPeriodicity(false);
  DFT(false);

  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++){
      float D = pow((pow(x − _width/2, 2) + pow(y − _height/2, 2)), −0.5);
      if (type == IDEAL){
        filter[y*_width + x] = buildIdealFilterPixel(filter[y*_width + x], pass, D0, D);
      }
      else if (type == BUTTERWORTH){
        filter[y*_width + x] = buildButterworthFilterPixel(filter[y*_width + x], pass, D0, D, n);
      }
      else if (type == GAUSSIAN){
        filter[y*_width + x] = buildGaussianFilterPixel(filter[y*_width + x], pass, D0, D);
      }
      _complexData[REAL] *= filter[y*_width + x];
      _complexData[IMAG] *= filter[y*_width + x];
    }
  IDFT(true)
  shiftForPeriodicity(true);
  padImage(0.5, 0,5)
}

float buildIdealFilterPixel(uint8 pass, uint32 D0, float D){
  float filterPixel = 0;
  if (pass == LOW){
    if (D <= D0) filterPixel = 1;
    else filterPixel = 0;
  }
  if (pass == HIGH){
    if (D <= D0) filterPixel = 0;
    else filterPixel = 1;
  }
  return filterPixel;
}
float buildButterworthFilterPixel(float filterPixel, uint8 pass, uint32 D0, float D, uint8 n){
  float filterPixel = 0;
  if (pass == LOW){
    filterPixel = 1 / (1 + pow(D/D0, 2*n));
  }
  if (pass == HIGH){
    filterPixel = 1 / (1 + pow(D0/D, 2*n));
  }
  return filterPixel;
}
float buildGaussianFilterPixel(float filterPixel, uint8 pass, uint32 D0, float D){
  float filterPixel = 0;
  if (pass == LOW){
    filterPixel = exp(pow(-1*D, 2) / 2*D0);
  }
  if (pass == HIGH){
    filterPixel = 1 - exp(pow(-1*D, 2) / 2*D0);
  }
  return filterPixel;
}

void validation (uint8 type, uint8 pass){
  if (type < 0 || type > 2){
    std::cout << "We only support Ideal (0), Butterworth (1) and Gaussian (2) types of filters" << std::endl;
    std::exit(0);
  }
  if (pass < 0 || pass > 1){
    std::cout << "We only support Low Pass (0) and High Pass (1) filters" << std::endl;
    std::exit(0);
  }
}
