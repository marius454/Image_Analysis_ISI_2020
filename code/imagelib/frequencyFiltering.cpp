#include "image.hpp"

// Define the setting for high pass and low pass filtering, as well as band and notch filters
#define LOW 0
#define HIGH 1
#define BP 2
#define BR 3
#define NP 4
#define NR 5

// Define the setting for type of filtering
#define IDEAL 0
#define BUTTERWORTH 1
#define GAUSSIAN 2
// Define the setting for type of visualization
#define FILTER 0
#define FILTEREDFOURIER 1
#define FILTEREDIMAGE 2

void validation (uint16 type, uint16 pass, uint16 visualise, uint16 nrOfNotches);

void Image::frequencyFilter(uint16 type, uint16 pass, uint16 visualise, double radius, uint16 n, float W, uint32** notchPoints, uint16 nrOfNotches){
  validation(type, pass, visualise, nrOfNotches);

  uint32 D0 = radius;
  padImage(2, 2);
  shiftForPeriodicity(false);
  DFT();
  uint32 imgSize = _width * _height * _channels;
  float* filter = new float[imgSize];

  float max = std::numeric_limits<float>::min();
  float min = std::numeric_limits<float>::max();

  for (int y = 0; y < _height; y++)
    for (int x = 0; x < _width; x++){
      if (pass == NP || pass == NR){
        
        if (type == IDEAL){
          filter[y*_width + x] = buildIdealNotchFilterPixel(pass, x, y, D0, notchPoints, nrOfNotches);
        }
        else if (type == BUTTERWORTH){
          filter[y*_width + x] = buildButterworthNotchFilterPixel(pass, x, y, D0, n, notchPoints, nrOfNotches);
        }
        else if (type == GAUSSIAN){
          filter[y*_width + x] = buildGaussianNotchFilterPixel(pass, x, y, D0, notchPoints, nrOfNotches);
        }
      }
      else {
        double D = sqrt(pow((double)x - (double)_width/2, 2) + pow((double)y - (double)_height/2, 2));
        if (type == IDEAL){
          filter[y*_width + x] = buildIdealFilterPixel(pass, D0, D, W);
        }
        else if (type == BUTTERWORTH){
          filter[y*_width + x] = buildButterworthFilterPixel(pass, D0, D, n, W);
        }
        else if (type == GAUSSIAN){
          filter[y*_width + x] = buildGaussianFilterPixel(pass, D0, D, W);
        }
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
  if (visualise == FILTER){
    uint16 L = pow(2, _bpp);
    float filterPixel;
    for (uint32 i = 0; i < imgSize; i++){
      filterPixel = ((L-1)*(filter[i] - min)) / (max - min);
      _data[i] = static_cast<unsigned char>((int)round(filterPixel));
    }
    calculateHistogram();
  }
  else if (visualise == FILTEREDFOURIER){
    visualiseComplex(0.15);
    calculateHistogram();
  }
  else if (visualise == FILTEREDIMAGE) {
    IDFT();
    // shiftForPeriodicity(true);
    padImage(0.5, 0.5);
    calculateHistogram();
  }
}

float Image::buildIdealFilterPixel(uint16 pass, float D0, float D, float W){
  float filterPixel = 0;
  if (pass == LOW){
    if (D <= D0) filterPixel = 1;
    else filterPixel = 0;
  }
  if (pass == HIGH){
    if (D <= D0) filterPixel = 0;
    else filterPixel = 1;
  }
  if (pass == BP){
    if (D0 - W/2 <= D && D <= D0 + W/2) filterPixel = 1;
    else filterPixel = 0;
  }
  if (pass == BR){
    if (D0 - W/2 <= D && D <= D0 + W/2) filterPixel = 0;
    else filterPixel = 1;
  }
  return filterPixel;
}
float Image::buildButterworthFilterPixel(uint16 pass, float D0, float D, uint16 n, float W){
  float filterPixel = 0;
  if (pass == LOW){
    filterPixel = 1 / (1 + pow(D/D0, 2*n));
  }
  if (pass == HIGH){
    filterPixel = 1 / (1 + pow(D0/D, 2*n));
  }
  if (pass == BP){
    filterPixel = 1 - (1 / (1 + pow((D*W) / (D*D - D0*D0), 2*n)));
  }
  if (pass == BR){
    filterPixel = 1 / (1 + pow((D*W) / (D*D - D0*D0), 2*n));
  }
  return filterPixel;
}
float Image::buildGaussianFilterPixel(uint16 pass, float D0, float D, float W){
  float filterPixel = 0;
  if (pass == LOW){
    filterPixel = exp(-((D*D) / (2.0f*(D0*D0))));
  }
  if (pass == HIGH){
    filterPixel = 1 - exp(-((D*D) / (2.0f*(D0*D0))));
  }
  if (pass == BP){
    filterPixel = exp(-pow((D*D - D0*D0) / (D*W), 2));
  }
  if (pass == BR){
    filterPixel = 1 - exp(-pow((D*D - D0*D0) / (D*W), 2));
  }
  return filterPixel;
}
float Image::buildIdealNotchFilterPixel(uint16 pass, int x, int y, float D0, uint32** notchPoints, uint16 nrOfNotches){
  float filterPixel = 0;
  if (pass == NR) filterPixel = 1;
  for (uint32 i = 0; i < nrOfNotches; i++){
    double Dk1 = sqrt(pow((double)x - (double)_width/2 - (double) notchPoints[i][0], 2) + pow((double)y - (double)_height/2 - (double) notchPoints[i][1], 2));
    double Dk2 = sqrt(pow((double)x - (double)_width/2 + (double) notchPoints[i][0], 2) + pow((double)y - (double)_height/2 + (double) notchPoints[i][1], 2));
    if (pass == NP){
      if (Dk1 <= D0 || Dk2 < D0) filterPixel = 1;
    }
    if (pass == NR){
      if (Dk1 <= D0 || Dk2 < D0) filterPixel = 0;
    }
  }
  return filterPixel;
}
float Image::buildButterworthNotchFilterPixel(uint16 pass, int x, int y, float D0, uint16 n, uint32** notchPoints, uint16 nrOfNotches){
  float filterPixel = 0;
  for (uint32 i = 0; i < nrOfNotches; i++){
    double Dk1 = sqrt(pow((double)x - (double)_width/2 - (double) notchPoints[i][0], 2) + pow((double)y - (double)_height/2 - (double) notchPoints[i][1], 2));
    double Dk2 = sqrt(pow((double)x - (double)_width/2 + (double) notchPoints[i][0], 2) + pow((double)y - (double)_height/2 + (double) notchPoints[i][1], 2));
    if (pass == NP){
      filterPixel = (1 / (1 + pow(Dk1/D0, 2*n))) * (1 / (1 + pow(Dk2/D0, 2*n)));
    }
    if (pass == NR){
      filterPixel = (1 / (1 + pow(D0/Dk1, 2*n))) * (1 / (1 + pow(D0/Dk2, 2*n)));
    }
  }
  return filterPixel;
}
float Image::buildGaussianNotchFilterPixel(uint16 pass, int x, int y, float D0, uint32** notchPoints, uint16 nrOfNotches){
  float filterPixel = 0;
  for (uint32 i = 0; i < nrOfNotches; i++){
    double Dk1 = sqrt(pow((double)x - (double)_width/2 - (double) notchPoints[i][0], 2) + pow((double)y - (double)_height/2 - (double) notchPoints[i][1], 2));
    double Dk2 = sqrt(pow((double)x - (double)_width/2 + (double) notchPoints[i][0], 2) + pow((double)y - (double)_height/2 + (double) notchPoints[i][1], 2));
    if (pass == NP){
      filterPixel = (exp(-((Dk1*Dk1) / (2.0f*(D0*D0))))) * (exp(-((Dk2*Dk2) / (2.0f*(D0*D0)))));
    }
    if (pass == NR){
      filterPixel = (1 - exp(-((Dk1*Dk1) / (2.0f*(D0*D0))))) * (1 - exp(-((Dk2*Dk2) / (2.0f*(D0*D0)))));
    }
  }
  return filterPixel;
}

void Image::getCutOut(uint32 startX, uint32 startY, uint32 endX, uint32 endY){
  if (startX > _width || startY > _height || endX > _width || endY > _height){
    std::cout << "given coordinates are out of image bounds" << std::endl;
    std::exit(0);
  }
  if (startX > endX){
    uint32 temp = startX;
    startX = endX;
    endX = temp;
  }
  if (startY > endY){
    uint32 temp = startY;
    startY = endY;
    endY = temp;
  }
  uint32 tempWidth = endX - startX;
  uint32 tempHeight = endY - startY;
  unsigned char* tempData = new unsigned char[tempWidth * tempHeight];
  for (uint32 y = startY; y < endY; y++)
    for (uint32 x = startX; x < endX; x++){
      tempData[(y - startY) * tempWidth + (x - startX)] = (int)_data[y*_width + x];
    }
  delete(_data);
  _data = tempData;
  _width = tempWidth;
  _height = tempHeight;
  calculateHistogram();
}

void validation (uint16 type, uint16 pass, uint16 visualise, uint16 nrOfNotches){
  if (type < 0 || type > 2){
    std::cout << "We only support Ideal (0), Butterworth (1) and Gaussian (2) types of filters" << std::endl;
    std::exit(0);
  }
  if (pass < 0 || pass > 5){
    std::cout << "We only support Low Pass (0), High Pass (1), Band Pass(2), Band Reject(3), Notch Pass(4) and Notch Reject(5) filters" << std::endl;
    std::exit(0);
  }
  if (visualise < 0 || visualise > 2){
    std::cout << "We only support Filter (0) and Filtered Faurier (1), and Filtered Image (2) visualizations" << std::endl;
    std::exit(0);
  }
  if (pass >= 4 && nrOfNotches == 0){
    std::cout << "You must define at least one point for notch filtering" << std::endl;
    std::exit(0);
  }
}
