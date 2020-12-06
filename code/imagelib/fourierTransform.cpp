#include "image.hpp"

void Image::fourierTransform(std::string visualizedStage){
  padImage(2, 2);
  if (visualizedStage == "padded") return;
  if (visualizedStage == "shifted"){
    shiftForPeriodicity(true);
    delete(_floatData);
    calculateHistogram();
    return;
  }
  shiftForPeriodicity(false);
  if (visualizedStage == "dft"){
    DFT(true);
    calculateHistogram();
    return;
  }
  DFT(false);
  IDFT(true);
  shiftForPeriodicity(true);
  if (visualizedStage == "idft"){
    calculateHistogram();
    return;
  }
  padImage(0.5, 0.5);
  if (visualizedStage == "recoveredOriginal"){
    calculateHistogram();
    return;
  }
  else {
    std::cout << "Invalid input given to Fourier Transform method" << std::endl;
    std::exit(0);
  }
}

void Image::padImage(float xMultiplier, float yMultiplier){
  uint32 tempWidth = _width;
  uint32 tempHeight = _height;

  if (xMultiplier == 1 && yMultiplier == 1){
    return;
  }
  else if (xMultiplier == 1){
    tempHeight = (int)(tempHeight * yMultiplier);
  }
  else if (yMultiplier == 1){
    tempWidth = (int)(tempHeight * xMultiplier);
  }
  else{
    tempWidth = (int)(tempHeight * xMultiplier);
    tempHeight = (int)(tempHeight * yMultiplier);
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

void Image::shiftForPeriodicity(bool visualise){
  if (!visualise) _floatData = new float[_width * _height * _channels];
  int shift;
  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++){
      if ((x+y) % 2 == 0) shift = 1;
      else shift = -1;

      if (visualise) _data[y*_width + x] = _data[y*_width + x] * shift;
      else _floatData[y*_width + x] = _data[y*_width + x] * shift;
    }
}

void Image::DFT(bool visualise){
  uint32 imgSize = _width * _height * _channels;
  fftw_complex* in = new fftw_complex[imgSize];
  _complexData = new fftw_complex[imgSize];

  for (uint32 i = 0; i < imgSize; i++){
    in[i][REAL] = _floatData[i] / imgSize;
    in[i][IMAG] = 0;
  }
  delete(_floatData);

  fftw_plan DFT = fftw_plan_dft_2d (_width, _height, in, _complexData, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(DFT);
  fftw_destroy_plan(DFT);
  delete(in);
  fftw_cleanup();

  if (visualise){
    visualiseComplex(0.1);
  }
}

void Image::IDFT(bool visualise){
  uint32 imgSize = _width * _height * _channels;
  fftw_complex* out = new fftw_complex[imgSize];

  fftw_plan IDFT = fftw_plan_dft_2d (_width, _height, _complexData, out, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(IDFT);
  fftw_destroy_plan(IDFT);
  delete(_complexData);
  fftw_cleanup();

  if (!visualise) _floatData = new float[imgSize];
  for (uint32 i = 0; i < imgSize; i++){
    if (visualise){
      _data[i] = static_cast<int>(out[i][REAL]);
    } 
    else _floatData[i] = out[i][REAL];
  }
  std::cout << std::endl;
}

void Image::visualiseComplex(float gamma){
  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bpp);
  _floatData = new float[imgSize];
  double max = std::numeric_limits<float>::min();
  double min = std::numeric_limits<float>::min();

  for (uint32 i = 0; i < imgSize; i++){
    _floatData[i] = sqrt(pow(_complexData[i][REAL], 2) + pow(_complexData[i][IMAG], 2));
    if (_floatData[i] < min) min = _floatData[i];
    if (_floatData[i] > max) max = _floatData[i];
  }
  for (uint32 i = 0; i < imgSize; i++){
    _floatData[i] = ((L-1)*(_floatData[i] - min)) / (max - min);
  }
  intensityPowerLaw(gamma, true);
  for (uint32 i = 0; i < imgSize; i++){
    _data[i] = static_cast<unsigned char>((int)round(_floatData[i]));
  }
}
