#ifndef SIMPLE_IMAGE_LIBRARY_HPP
#define SIMPLE_IMAGE_LIBRARY_HPP
// Define the real and imaginary parts of fftw_complex
#define REAL 0
#define IMAG 1

#include <iostream>
#include <string>
#include <stdio.h>
#include <Eigen/Core>
#include <Eigen/LU>
#include <vector>
#include <math.h>
#include <thread>
#include <chrono>
#include <tiffio.h>
#include <fftw3.h>
#include <cmath>

struct BBox{
  float maxX;
  float maxY;
  float minX;
  float minY;
};

class Image{
public:
	Image();
  Image(std::string filename);
	Image(std::string filename, short directory);
	Image(std::string filename1, std::string filename2, std::string filename3);
  Image(const Image& original);
  Image(uint32 width, uint32 height);

	virtual ~Image();

	// File related
	bool openFile(std::string filename, short directory);
	bool combineFiles(std::string filename1, std::string filename2, std::string filename3);

  // Image transformations
	bool manipulateImage(unsigned short n, Eigen::Matrix3f *changeMatrices, bool useNN = true);
  void scaleImage(std::string interpolationScheme, float scaleX, float scaleY);
  void rotateImage(std::string interpolationScheme, float degreeOfRotation);
  void shearImage(std::string interpolationScheme, float shearAmount);
  void presetTransformations(std::string interpolationScheme, uint8 transformationNumber);

	// Intensity transformations
  void intensityNegate();
  void intensityPowerLaw(float gamma, bool isFloat = false);
  void contrastStretching(uint16 numberOfSlopeChangePoints, float* slopeChangeFractionPoints,
   float* desiredValueFractionsAtPoints, uint8 algorithm = 0);
  void histogramNormalization();

  // Spatial filtering
  void imageBlurring(uint32 filterWidth);
  void sharpeningUnsharpMask(uint16 blurringFilterWidth, uint8 k = 1);
  void sharpeningLaplacian(bool useN8 = false, bool getOnlyLaplacian = false);
  void sobelOperator();
  void Fig3_43(char imgLetter);
  void fullMedianFilter(uint16 filterWidth);

  // Fourier transform
  void padImage(float xMultiplier, float yMultiplier);
  void fourierTransform(std::string visualizedStage, float gamma = 0.1);

  // Generating images
  void generateImage(std::string imageType, float alphaXMultiplier, float alphaYMultiplier);

  // Frequency filtering and Noice removal
  void frequencyFilter(uint16 type, uint16 pass, uint16 visualise, double radius, uint16 n, float W = 10, uint32** notchPoints = nullptr, uint16 nrOfNotches = 0);
  void getCutOut(uint32 startX, uint32 startY, uint32 endX, uint32 endY);

  // Image Mathematical Morphology/Segmentation
  void createFISHreport(uint16 visualizedStep);
  void threshold(uint8 changePoint, unsigned char *data);
  void calculatedThreshold(uint8 changePoint, unsigned char *data);
  void squareErosion(uint16 squareWidth);
  void squareDilation(uint16 squareWidth);
  void findNeighbourhood(uint32 x, uint32 y, uint8 startIntensity, uint8 endIntensity);
  void fillHoles();
  void getBoudaries();
  uint16 findCells();
  void assignCells(uint16 cellNr, uint16 *mutations = nullptr);

  void circuitBoardQA(std::string evaluation);
  void evaluateWires(uint16 wireIntensity);
  void evaluateSolderingIslands();
  void evaluateSolderingHoles(uint16 holeIntensity);

  void bottles();
  void checkbottles(uint16 nrChecks);

  // Image related
	unsigned char* getImageData() const;
  void splitChannels(char showChannel);
  void initializeRGB();
  void combineRGB();

	// Get attributes
	unsigned long getWidth() const;
	unsigned long getHeight() const;
	unsigned long getDepth() const;
	unsigned long getChannels() const;
	unsigned long getBPP() const;
  unsigned long getSamplesPerPixel() const;
  unsigned long getBitsPerSample() const;
  std::vector<unsigned int> getHistogram() const;

private:
	unsigned long _width{0};
	unsigned long _height{0};
	unsigned long _depth{0};
	unsigned long _channels{0};
	unsigned long _bpp{0};
  uint32 _imgSize{0};
  uint16 _L{0};
  float* _lookupTable = new float[256];
  std::vector<unsigned int> _histogram = std::vector<unsigned int>(256,0);

  // data in different formats
	unsigned char* _data{nullptr};
  unsigned char* _Rdata{nullptr};
  unsigned char* _Gdata{nullptr};
  unsigned char* _Bdata{nullptr};
  fftw_complex* _complexData{nullptr};
  float* _floatData{nullptr};

	BBox _bbox;
  Eigen::Matrix3f I_W;
  Eigen::Matrix3f W_I;

	// Tiff related
	bool loadTiff(std::string filename, short directory);
	bool loadCombinedTiff(std::string filename1, std::string filename2, std::string filename3);
	// Jpeg related
	bool loadJpeg(std::string filename);

  // Image transformation related
  void recalculateBBox(unsigned short n, Eigen::Matrix3f *changeMatrices);
  void setIntensities(unsigned short n, Eigen::Matrix3f *changeMatrices, bool userNN = true);
  unsigned char NN(Eigen::Vector3f indexVec);
  unsigned char bilinearInterpolation(Eigen::Vector3f indexVec);
  void performCopy(Image const & obj);

  // Intensity transformation related
  void calculateHistogram();
  void transformPixels();

  // Spacial filtering related
  void localMedianFilter(uint32 x, uint32 y, uint16 filterWidth);

  // Fourier transform related
  void shiftForPeriodicity(bool visualise);
  void DFT();
  void IDFT();
  void visualiseComplex(float gamma);

  // Generating Images related
  void generateLineImage(float alphaXMultiplier, float alphaYMultiplier);
  void generateTest1Image(float alphaXMultiplier, float alphaYMultiplier);

  // Frequency filtering related
  float buildIdealFilterPixel(uint16 pass, float D0, float D, float W);
  float buildButterworthFilterPixel(uint16 pass, float D0, float D, uint16 n, float W);
  float buildGaussianFilterPixel(uint16 pass, float D0, float D, float W);
  float buildIdealNotchFilterPixel(uint16 pass, int x, int y, float D0, uint32** notchPoints, uint16 nrOfNotches);
  float buildButterworthNotchFilterPixel(uint16 pass, int x, int y, float D0, uint16 n, uint32** notchPoints, uint16 nrOfNotches);
  float buildGaussianNotchFilterPixel(uint16 pass, int x, int y, float D0, uint32** notchPoints, uint16 nrOfNotches);

  // Segmentation/Morphology related
  void getNeighbours(uint32 x, uint32 y, uint16 intensity, bool *visitedPixels);
  bool checkForConnection(uint16 neighbourhoodIntensity, uint16 backgroundIntensity);
  void removeWires();
  void evaluateIsland(uint16 islandIntensity, uint16 checkedIslandIntensity);
  void fillRGBByIntensity(uint16 dataIntensity, uint16 R, uint16 G, uint16 B);
  void fillRGB(uint32 pixelIndex, uint16 R, uint16 G, uint16 B);

};

class Interval{
public:
  Interval() { }
  Interval(Eigen::Vector2f singleVec, std::string side, uint16 L);
  Interval(Eigen::Vector2f left, Eigen::Vector2f right);

  float linearInterpolation(float x);
  unsigned int threshold(uint32 x);

  Eigen::Vector2f getLeft();
  Eigen::Vector2f getRight();
private:
  Eigen::Vector2f _left, _right;
};
#endif
