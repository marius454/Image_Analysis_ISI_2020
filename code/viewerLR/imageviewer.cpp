#include "qtimageviewer.hpp"

#include <image.hpp>
#include <Eigen/Geometry>

#include <iostream>
#include <string>
#include <cstdlib>
#include <QApplication>
#include <Eigen/Core>
#include <math.h>

void uiActions(int argc, char** argv, QtImageViewer* imv);
void invalidUiCall(std::string func);
std::map<std::string, int> getFuncMap();

int main(int argc, char** argv){
  QApplication app( argc, argv );
  QtImageViewer* imv = new QtImageViewer();
  if(argc >= 2){
    std::string func = std::string(argv[1]);
    if (argc == 2){
      QString filename(argv[1]);
      std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
      imv->showFile(filename);
    }
    else if (func == "negate" || func == "powerlaw" || func == "contrastlinear"
      || func == "contrastthreshold" || func == "contrastslice" || func == "normalize"
      || func == "blur" || func == "unsharpmask" || func == "laplacian"
      || func == "sobel" || func == "fig3-43"){
        uiActions(argc, argv, imv);
      }
  }
  imv->show();
  imv->resize(1000,600);

  int ret = app.exec();
  
  delete(imv);

  return ret;
  
}

void uiActions(int argc, char** argv, QtImageViewer* imv){
  QString filename(argv[argc-1]);
  Image* myImage = new Image(filename.toStdString());

  std::string func = std::string(argv[1]);
  std::map<std::string, int> funcMap = getFuncMap();

  switch(funcMap[func]){
    case 1:
      if (argc == 3) imv->showImage(myImage, func);
      else invalidUiCall(func);
      break;
    case 2:
      if (argc == 4){
        float* values = new float[1]{atof(argv[2])};
        imv->showImage(myImage, func, values);
      } 
      else invalidUiCall(func);
      break;
    case 3:
      {
        if ((argc-3) % 2 != 0){
          std::cout << "number of fraction values must be even (x and y pairs)";
          std::exit(0);
        }
        float* values = new float[argc - 3];
        int nrOfValues = 0;
        for (unsigned short i = 2; i < argc-1; i++){
          if (atof(argv[i]) > 1 || atof(argv[i]) < 0){
            std::cout << "values must be fractions between 0 and 1 inclusively" << std::endl;
            std::exit(0);
          }
          values[i-2] = atof(argv[i]);
          nrOfValues++;
        }
        imv->showImage(myImage, func, values, nrOfValues);
      }
      break;
    case 4:
      if (argc == 4){
        float* values = new float[1]{atoi(argv[2])};
        imv->showImage(myImage, func, values);
      }
      else invalidUiCall(func);
      break;
    case 5:
      if (argc == 4){
        float* values = new float[1];
        if (std::string(argv[2]) == "n4") values[0] = 0;
        else if (std::string(argv[2]) == "n8") values[0] = 1;
        else{
          std::cout << "third argument must be 'n4' or 'n8' for neighbourhood scheme" << std::endl;
          std::exit(0);
        }
        imv->showImage(myImage, func, values);
      }
      else invalidUiCall(func);
      break;
    case 6:
      if (argc == 5) imv->showImage(myImage, argv[2][0], argv[3][0]);
      else invalidUiCall(func);
      break;
    default: 
      std::cout << "Invalid input";
      std::exit(0);
      break;
  }
  std::cout<<"Loading and doing operation " << argv[1] << ":" << filename.toStdString() << std::endl;
}

void invalidUiCall(std::string func){
  std::cout << "Invalid number of arguments for operation " << func << std::endl;
  std::exit(0);
}

std::map<std::string, int> getFuncMap(){
  std::map<std::string, int> funcMap;
  funcMap["negate"] = 1;
  funcMap["normalize"] = 1;
  funcMap["sobel"] = 1;
  funcMap["powerlaw"] = 2;
  funcMap["contrastlinear"] = 3;
  funcMap["contrastthreshold"] = 3;
  funcMap["contrastslice"] = 3;
  funcMap["blur"] = 4;
  funcMap["unsharpmask"] = 4;
  funcMap["laplacian"] = 5;
  funcMap["fig3-43"] = 6;

  return funcMap;
}