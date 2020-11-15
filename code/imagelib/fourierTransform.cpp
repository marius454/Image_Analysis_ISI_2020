#include "image.hpp"

// Define the real and imaginary parts of fftw_complex
#define REAL 0
#define IMAG 1


void Image::fourierTransform(char imgLetter){
  float* tempImgData;

  padImage(2, 2);
  if (imgLetter == 'b') return;
  if (imgLetter == 'c'){
    tempImgData = shiftedForPeriodicity(true);
    delete(tempImgData);
    calculateHistogram();
    return;
  }
  tempImgData = shiftedForPeriodicity(false);

  // uint32 imgSize = _width * _height * _channels;
  // tempImgData = new float[imgSize];
  // for (uint32 i = 0; i < imgSize; i++){
  //   tempImgData[i] = _data[i];
  // }

  if (imgLetter == 'd'){
    DFT(tempImgData, true);
    delete(tempImgData);
    histogramNormalization();
    calculateHistogram();
    return;
  }
  DFT(tempImgData, false);
}

void Image::padImage(uint32 xMultiplier, uint32 yMultiplier){
  uint32 tempWidth = _width;
  uint32 tempHeight = _height;

  if (xMultiplier < 1 || yMultiplier < 1){
    std::cout << "Padding multipliers must be equal to or greater than 1";
    std::exit(0);
  }
  else if (xMultiplier == 1 && yMultiplier == 1){
    return;
  }
  else if (xMultiplier == 1){
    tempHeight *= yMultiplier;
  }
  else if (yMultiplier == 1){
    tempWidth *= xMultiplier;
  }
  else{
    tempWidth *= xMultiplier;
    tempHeight *= yMultiplier;
  }

  unsigned char* paddedData = new unsigned char[tempWidth * tempHeight * _channels];
  for (uint32 y = 0; y < tempHeight; y++)
    for (uint32 x = 0; x < tempWidth; x++){
      if (x < _width && y < _height){
        paddedData[y*tempWidth + x] = _data[y*_width + x];
      }
      else{
        paddedData[y*tempWidth + x] = (unsigned char)0;
      }
    }
  delete(_data);
  _data = paddedData;
  _width = tempWidth;
  _height = tempHeight;
  calculateHistogram();
}

float* Image::shiftedForPeriodicity(bool visualise){
  float* tempImgData = new float[_width * _height * _channels];
  int shift;
  int maxIntensity = pow(2, _bpp) - 1;
  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++){
      if ((x+y) % 2 == 0) shift = 1;
      else shift = -1;

      tempImgData[y*_width + x] = _data[y*_width + x] * shift;
      if (visualise) _data[y*_width + x] = _data[y*_width + x] * shift;
    }
  return tempImgData;
}

void Image::DFT(float* tempImgData, bool visualise){
  uint16 L = pow(2, _bpp);
  uint32 imgSize = _width * _height * _channels;
  fftw_complex* in = new fftw_complex[imgSize];
  _complexData = new fftw_complex[imgSize];

  for (uint32 i = 0; i < imgSize; i++){
    in[i][REAL] = tempImgData[i];
    in[i][IMAG] = 0;
  }

  fftw_plan DFT = fftw_plan_dft_2d (_width, _height, in, _complexData, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(DFT);
  fftw_destroy_plan(DFT);
  fftw_cleanup();

  double* complexToReal = new double[imgSize];
  double max = std::numeric_limits<float>::min();
  double min = std::numeric_limits<float>::min();

  for (uint32 i = 0; i < imgSize; i++){
    complexToReal[i] = sqrt(pow(_complexData[i][REAL], 2) + pow(_complexData[i][IMAG], 2));
    // complexToReal[i] = abs(_complexData[i][REAL]) + abs(_complexData[i][IMAG]);
    // complexToReal[i] = abs(_complexData[i][REAL] + _complexData[i][IMAG]);
    if (complexToReal[i] < min) min = complexToReal[i];
    if (complexToReal[i] > max) max = complexToReal[i];
  }
  for (uint32 i = 0; i < imgSize; i++){
    complexToReal[i] = ((L-1)*(complexToReal[i] - min)) / (max - min);
    _data[i] = static_cast<unsigned char>((int)complexToReal[i]);
  }
  std::cout << std::endl;
}