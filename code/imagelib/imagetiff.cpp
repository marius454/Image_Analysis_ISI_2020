#include "image.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <tiffio.h> // Note use of libtiff
// See http://www.libtiff.org/ for use
// and https://www.awaresystems.be/imaging/tiff.html for tags

bool readTiffMetaData(TIFF *tiff, short directory) {
  TIFFSetDirectory(tiff, directory); // NB!
  TIFFPrintDirectory(tiff, stdout, TIFFPRINT_NONE);
  // Read using TIFFGetField
  unsigned int width{0};
  TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &width);
  return true;
}

unsigned char* loadTiffTiled(TIFF* tiff, uint32 imageWidth, uint32 imageLength, short channels) {
  uint32 tileWidth, tileLength;
    
    TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &tileWidth);
    TIFFGetField(tiff, TIFFTAG_TILELENGTH, &tileLength);

  std::cout << "Image resolution: " << imageWidth << "x" << imageLength << std::endl;
  std::cout << "Tile resolution: " << tileWidth << "x" << tileLength << std::endl;

  // uint32 w = tileWidth * (imageWidth / tileWidth + 1);
  // uint32 l = tileLength * (imageLength / tileLength + 1);

  //unsigned char* imgData = (unsigned char*) malloc (w*l);
  unsigned char* imgData = (unsigned char*) malloc (TIFFTileSize(tiff)*TIFFNumberOfTiles(tiff));
  unsigned char* buf = (unsigned char*) malloc (TIFFTileSize(tiff));
  std::cout << TIFFTileSize(tiff) << " " << TIFFNumberOfTiles(tiff) << std::endl;

  if (channels == 3){
    for (uint32 y = 0; y < imageLength; y += tileLength)
      for (uint32 x = 0; x < imageWidth; x += tileWidth){
        TIFFReadTile(tiff, buf, x, y, 0, 0);

        for (uint32 ty = 0; ty < tileLength; ty++)
          for (uint32 tx = 0; tx < tileWidth * 3; tx += 3){
            uint32 imgx = x*3+tx;
            uint32 imgy = y+ty;
            if (imgy > (imageLength)*3+1 || imgx > (imageWidth)*3+1){
              break;
            }

            imgData[ imgy*imageWidth*3 + imgx ] = buf[ ty*tileWidth*3 + tx ];
            imgData[ imgy*imageWidth*3 + imgx + 1 ] = buf[ ty*tileWidth*3 + tx + 1 ];
            imgData[ imgy*imageWidth*3 + imgx + 2 ] = buf[ ty*tileWidth*3 + tx + 2 ];
          }
      }
  } else {
    for (uint32 y = 0; y < imageLength; y += tileLength)
      for (uint32 x = 0; x < imageWidth; x += tileWidth){
        TIFFReadTile(tiff, buf, x, y, 0, 0);

        for (uint32 ty = 0; ty < tileLength; ty++)
          for (uint32 tx = 0; tx < tileWidth; tx++){
            uint32 imgx = x+tx;
            uint32 imgy = y+ty;

            imgData[ imgy*imageWidth + imgx ] = buf[ ty*tileWidth + tx ];
          }
      }
  }
  free(buf);
  std::cout << "all pixels were moved to memory" << std::endl;
  return imgData;

  // TIFFReadTile(tiff, buf, 960, 0, 0, 0);
  // return buf;
}

unsigned char* loadTiffStrip(TIFF* tiff, uint32 imageWidth, uint32 imageLength, short channels) {
  tsize_t stripSize;
  std::cout << "Image resolution: " << imageWidth << "x" << imageLength << std::endl;

  stripSize = TIFFStripSize(tiff);
  unsigned char* imgData = (unsigned char*) malloc (stripSize*TIFFNumberOfStrips(tiff));
  unsigned char* buf = (unsigned char*) malloc (stripSize);
  
  std::cout << stripSize << " " << TIFFNumberOfStrips(tiff) << std::endl;
  for (tstrip_t strip = 0; strip < TIFFNumberOfStrips(tiff); strip++){
    TIFFReadEncodedStrip(tiff, strip, buf, -1);
    for (uint32 i = 0; i < stripSize; i ++){
      imgData[ strip*stripSize + i ] = buf[i];
    }
  }
  return imgData;
}

unsigned char* loadTiffScanline(TIFF* tiff) {
  // ...
}

unsigned char* combineTiff(unsigned char* img1Data, unsigned char* img2Data, unsigned char* img3Data, uint32 imageWidth, uint32 imageLength){
  uint32 imgSize;
  unsigned char* imgData = (unsigned char*) malloc(imageWidth * imageLength * 3);
  imgSize = imageWidth * imageLength;

  for (uint32 i = 0; i < imgSize; i ++){
    imgData[i*3] = img1Data[i];
    imgData[i*3 + 1] = img2Data[i];
    imgData[i*3 + 2] = img3Data[i];
  }

  return imgData;
}

bool Image::loadTiff(std::string filename, short directory){
  uint32 imageWidth, imageLength;
  short channels, bpp;

  std::cout<<"Loading tif: "<<filename<<std::endl;

  TIFF *tiff = TIFFOpen(filename.c_str(),"r");
  // Read image meta data, height, width etc.
  readTiffMetaData(tiff, directory);

  TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &imageWidth);
  TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &imageLength);
  TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &channels);
  TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bpp);

  _width = imageWidth;
  _height = imageLength;
  _channels = channels;
  _bpp = bpp;
  
  if(TIFFIsTiled(tiff)){
    _data = loadTiffTiled(tiff, imageWidth, imageLength, channels);
  }
  else{
    _data = loadTiffStrip(tiff, imageWidth, imageLength, channels);
  } // or    _data = loadTiffScanline(tiff);

  return true;
};

bool Image::loadCombinedTiff(std::string filename1, std::string filename2, std::string filename3){
  uint32 imageWidth, imageLength;
  short channels, bpp;

  std::cout << "Loading combination of images" << std::endl;
  TIFF *tiff1 = TIFFOpen(filename1.c_str(), "r");
  TIFF *tiff2 = TIFFOpen(filename2.c_str(), "r");
  TIFF *tiff3 = TIFFOpen(filename3.c_str(), "r");

  TIFFGetField(tiff1, TIFFTAG_IMAGEWIDTH, &imageWidth);
  TIFFGetField(tiff1, TIFFTAG_IMAGELENGTH, &imageLength);
  TIFFGetField(tiff1, TIFFTAG_SAMPLESPERPIXEL, &channels);
  TIFFGetField(tiff1, TIFFTAG_BITSPERSAMPLE, &bpp);

  _width = imageWidth;
  _height = imageLength;
  _channels = channels * 3;
  _bpp = bpp;

  unsigned char* img1Data = (unsigned char*) malloc(imageWidth * imageLength);
  unsigned char* img2Data = (unsigned char*) malloc(imageWidth * imageLength);
  unsigned char* img3Data = (unsigned char*) malloc(imageWidth * imageLength);
  
  if(TIFFIsTiled(tiff1)){
    img1Data = loadTiffTiled(tiff1, imageWidth, imageLength, _channels);
  }
  else{
    img1Data = loadTiffStrip(tiff1, imageWidth, imageLength, _channels);
  }
  if(TIFFIsTiled(tiff2)){
    img2Data = loadTiffTiled(tiff2, imageWidth, imageLength, _channels);
  }
  else{
    img2Data = loadTiffStrip(tiff2, imageWidth, imageLength, _channels);
  }
  if(TIFFIsTiled(tiff3)){
    img3Data = loadTiffTiled(tiff3, imageWidth, imageLength, _channels);
  }
  else{
    img3Data = loadTiffStrip(tiff3, imageWidth, imageLength, _channels);
  }
  _data = combineTiff(img1Data, img2Data, img3Data, imageWidth, imageLength);

  return true;
}