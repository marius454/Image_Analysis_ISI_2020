#include "image.hpp"

#include <stdlib.h>
#include <jpeglib.h> // Note use of jpeg library
// See https://libjpeg-turbo.org/Documentation/Documentation

bool Image::loadJpeg(std::string filename){
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr err;
  FILE *infile;
  unsigned char* imgData;
  unsigned char* rowBuffer[1];
  
  std::cout << "Loading jpeg: " << filename <<std::endl;
  infile = fopen(filename.c_str(), "rb");
  if(infile == NULL) {
		return NULL;
	}
  cinfo.err = jpeg_std_error(&err);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  

  jpeg_start_decompress(&cinfo);
  _width = cinfo.output_width;
  _height = cinfo.output_height;
  _channels = cinfo.num_components;
  _bpp = 8;
  std::cout << "Image Resolution: " << _height << "x" << _width << std::endl;
  std::cout << "Samples per Pixel: " << _channels << std::endl;

  imgData = (unsigned char*)malloc(_height * _width * _channels);
  while ((&cinfo)->output_scanline < (&cinfo)->output_height){
    rowBuffer[0] = (unsigned char*)
      (&imgData[_channels*cinfo.output_width*cinfo.output_scanline]);
    jpeg_read_scanlines(&cinfo, rowBuffer, 1);
  }

  _data = imgData;
  jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);

  return true;
};
