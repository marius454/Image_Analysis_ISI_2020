#include "image.hpp"

void Image::fourierTransform(std::string visualizedStage, float gamma){
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
    DFT();
    visualiseComplex(gamma);
    calculateHistogram();
    return;
  }
  DFT();
  IDFT();
  // shiftForPeriodicity(true);
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
  uint32 tempWidth = (int)((float)_width * xMultiplier);
  uint32 tempHeight = (int)((float)_height * yMultiplier);

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
  bool floatImage = false;
  if (_floatData != nullptr) floatImage = true;
  if (!visualise && !floatImage) _floatData = new float[_width * _height * _channels];
  int shift;
  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++){
      if ((x+y) % 2 == 0) shift = 1;
      else shift = -1;
      
      if (visualise){
        if (!floatImage){
          if (_data[y*_width + x] * shift < 0){
            _data[y*_width + x] = 0;
          }
        }
        else{
          _data[y*_width + x] = static_cast<int>(round(_floatData[y*_width + x])) * shift;
        }
      }
      else {
        if (floatImage){
          _floatData[y*_width + x] = _floatData[y*_width + x] * shift;
        }
        else{
          _floatData[y*_width + x] = _data[y*_width + x] * shift;
        }
      }
    }
}

void Image::DFT(){
  uint32 imgSize = _width * _height * _channels;
  fftw_complex* in = new fftw_complex[imgSize];
  _complexData = new fftw_complex[imgSize];

  for (uint32 i = 0; i < imgSize; i++){
    in[i][REAL] = (float)_floatData[i];
    in[i][IMAG] = 0.0;
  }
  delete(_floatData);

  fftw_plan DFT = fftw_plan_dft_2d (_width, _height, in, _complexData, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(DFT);
  fftw_destroy_plan(DFT);
  delete(in);
  fftw_cleanup();
}

void Image::IDFT(){
  uint32 imgSize = _width * _height * _channels;
  fftw_complex* out = new fftw_complex[imgSize];
  uint16 L = pow(2, _bpp);

  fftw_plan IDFT = fftw_plan_dft_2d (_width, _height, _complexData, out, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(IDFT);
  fftw_destroy_plan(IDFT);
  delete(_complexData);
  fftw_cleanup();

  float max = std::numeric_limits<float>::min();
  float min = std::numeric_limits<float>::max();

  _floatData = new float[imgSize];

  // for (uint32 i = 0; i < imgSize; i++){
  //   _floatData[i] = round(out[i][REAL] / (float)imgSize);
  // }

  for (uint32 i = 0; i < imgSize; i++){
    _floatData[i] = out[i][REAL] / (float)imgSize;
  }
  shiftForPeriodicity(false);
  for (uint32 i = 0; i < imgSize; i++){
    if (_floatData[i] < -1){
      _floatData[i] = 0;
    }
    if (_floatData[i] > max) max = _floatData[i];
    if (_floatData[i] < min) min = _floatData[i];
  }
  if (min > 0) min = 0;
  if (max < L-1) max = L-1;
  for (uint32 i = 0; i < imgSize; i++){
    _floatData[i] = (((L-1)*(_floatData[i] - min)) / (max - min));
    _data[i] = static_cast<int>(round(_floatData[i]));
  }
  std::cout << std::endl;
}

void Image::visualiseComplex(float gamma){
  uint32 imgSize = _width * _height * _channels;
  uint16 L = pow(2, _bpp);
  _floatData = new float[imgSize];
  double max = std::numeric_limits<float>::min();
  double min = std::numeric_limits<float>::max();

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
