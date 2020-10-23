#ifndef SIMPLE_IMAGE_LIBRARY_HPP
#define SIMPLE_IMAGE_LIBRARY_HPP

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

	virtual ~Image();
	// File related
	bool openFile(std::string filename, short directory);
	bool combineFiles(std::string filename1, std::string filename2, std::string filename3);
  // Image transformations
	bool manipulateImage(unsigned short n, Eigen::Matrix3f *changeMatrices, bool useNN = true);
  void scaleImage(std::string interpolationScheme, float scaleX, float scaleY);
  void rotateImage(std::string interpolationScheme, float degreeOfRotation);
  void shearImage(std::string interpolationScheme, float shearAmount);
	// Intensity transformations
  void intensityNegate();
  void intensityPowerLaw(float a, float gamma);
  void contrastStretching(uint16 numberOfSlopeChangePoints, float* slopeChangeFractionPoints,
   float* desiredValueFractionsAtPoints, uint8 algorithm = 0);
  void histogramNormalization();
  // Spacial filtering
  void imageBlurring(uint32 filterWidth);
  void sharpeningUnsharpMask(uint16 blurringFilterWidth, uint8 k = 1);
  void sharpeningLaplacian(bool useN8 = false, bool getOnlyLaplacian = false);
  void sobelOperator();
  void Fig3_43(char imgLetter);
  // Image related
	unsigned char* getImageData() const;
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
  float* _lookupTable = new float[256];
  std::vector<unsigned int> _histogram = std::vector<unsigned int>(256,0);

	unsigned char* _data{nullptr};
	BBox _bbox;
  Eigen::Matrix3f I_W;
  Eigen::Matrix3f W_I;
	// Tiff related stuff
	bool loadTiff(std::string filename, short directory);
	// Jpeg related stuff
	bool loadJpeg(std::string filename);
	// any other...
	bool loadCombinedTiff(std::string filename1, std::string filename2, std::string filename3);
  void recalculateBBox(unsigned short n, Eigen::Matrix3f *changeMatrices);
  void setIntensities(unsigned short n, Eigen::Matrix3f *changeMatrices, bool userNN = true);
  unsigned char NN(Eigen::Vector3f indexVec);
  unsigned char bilinearInterpolation(Eigen::Vector3f indexVec);
  void performCopy(Image const & obj);
  void calculateHistogram();
  void transformPixels();
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
