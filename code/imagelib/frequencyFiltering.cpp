#include "image.hpp"

// Define the setting for high pass and low pass filtering
#define LOW 0
#define HIGH 1
// Define the setting for type of filtering
#define IDEAL 0
#define BUTTERWORTH 1
#define GAUSSIAN 2
// Define the setting for type of visualization
#define FILTER 0
#define FILTEREDFOURIER 1
#define FILTEREDIMAGE 2

void validation (uint16 type, uint16 pass, uint16 visualise);

void Image::frequencyFilter(uint16 type, uint16 pass, uint16 visualise, double radius, uint16 n){
  // std::cout << type << " " << pass << " " << visualise << " " << radius << std::endl;
  validation(type, pass, visualise);

  uint32 D0 = radius;
  padImage(2, 2);
  shiftForPeriodicity(false);
  DFT(false);
  uint32 imgSize = _width * _height * _channels;
  float* filter = new float[imgSize];

  double max = std::numeric_limits<float>::min();
  double min = std::numeric_limits<float>::min();

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      double D = sqrt(pow((double)x - (double)_width/2, 2) + pow(y - (double)_height/2, 2));
      // double D = sqrt(x*x + y*y);
      if (x == 511 && y == 511){
        // std::cout << x - (int)_width/2 << " " << y - (int)_height/2 << " " <<  D << std::endl;
        // std::cout << pow(x - _width/2, 2) << " " << pow(y - _height/2, 2) << " " <<  D << std::endl;

        std::cout << D << std::endl;
        std::cout << exp(-1*((D*D) / 2*(D0*D0))) << std::endl;
      }
      if (type == IDEAL){
        filter[y*_width + x] = buildIdealFilterPixel(pass, D0, D);
      }
      else if (type == BUTTERWORTH){
        filter[y*_width + x] = buildButterworthFilterPixel(pass, D0, D, n);
      }
      else if (type == GAUSSIAN){
        filter[y*_width + x] = buildGaussianFilterPixel(pass, D0, D);
      }
      if (visualise == FILTER){
        if (filter[y*_width + x] > max) max = filter[y*_width + x];
        if (filter[y*_width + x] < min) min = filter[y*_width + x];
      }
      else{
        _complexData[y*_width + x][REAL] *= filter[y*_width + x];
        _complexData[y*_width + x][IMAG] *= filter[y*_width + x];
      }
    }
  // std::cout << "good" << std::endl;
  if (visualise == FILTER){
    uint16 L = pow(2, _bpp);
    float filterPixel;
    for (uint32 i = 0; i < imgSize; i++){
      filterPixel = ((L-1)*(filter[i] - min)) / (max - min);
      _data[i] = static_cast<unsigned char>((int)round(filterPixel));
    }
  }
  else if (visualise == FILTEREDFOURIER){
    visualiseComplex(0.15);
  }
  else if (visualise == FILTEREDIMAGE) {
    IDFT(true);
    shiftForPeriodicity(true);
    padImage(0.5, 0.5);
  }
}

float Image::buildIdealFilterPixel(uint16 pass, uint32 D0, float D){
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
float Image::buildButterworthFilterPixel(uint16 pass, uint32 D0, float D, uint16 n){
  float filterPixel = 0;
  if (pass == LOW){
    filterPixel = 1 / (1 + pow(D/D0, 2*n));
  }
  if (pass == HIGH){
    filterPixel = 1 / (1 + pow(D0/D, 2*n));
  }
  return filterPixel;
}
float Image::buildGaussianFilterPixel(uint16 pass, uint32 D0, float D){
  float filterPixel = 0;
  if (pass == LOW){
    // std::cout << exp(pow(-1*D, 2) / 2*pow(D0, 2)) << std::endl;
    filterPixel = exp(-1*((D*D) / 2*(D0*D0)));
  }
  if (pass == HIGH){
    filterPixel = 1 - exp(-1*((D*D) / 2*(D0*D0)));
  }
  return filterPixel;
}

void validation (uint16 type, uint16 pass, uint16 visualise){
  if (type < 0 || type > 2){
    std::cout << "We only support Ideal (0), Butterworth (1) and Gaussian (2) types of filters" << std::endl;
    std::exit(0);
  }
  if (pass < 0 || pass > 1){
    std::cout << "We only support Low Pass (0) and High Pass (1) filters" << std::endl;
    std::exit(0);
  }
  if (visualise < 0 || visualise > 2){
    std::cout << "We only support Filter (0) and Filtered Faurier (1), and Filtered Image (2) visualizations" << std::endl;
    std::exit(0);
  }
}
