#include "qtimageviewer.hpp"

#include <image.hpp>
#include <Eigen/Geometry>

#include <iostream>
#include <string>
#include <cstdlib>
#include <QApplication>
#include <Eigen/Core>
#include <math.h>

void uiActions(int argc, char** argv, Image* myImage, QtImageViewer* imv);
void invalidUiCall(std::string func);
std::map<std::string, int> getFuncMap();

int main(int argc, char** argv){
  QApplication app( argc, argv );
  QtImageViewer* imv = new QtImageViewer();
  if(argc >= 2){
    std::string func = std::string(argv[1]);
    if (func == "negate" || func == "powerlaw" || func == "contrastlinear"
      || func == "contrastthreshold" || func == "contrastslice" || func == "normalize"
      || func == "blur" || func == "unsharpmask" || func == "laplacian"
      || func == "sobel" || func == "fig3-43" || func == "fourier" || func == "frequencyfilter"
      || func == "cutout" || func == "circuitBoard" || func == "bottles"){
        QString filename(argv[argc-1]);
        Image* myImage = new Image(filename.toStdString());
        uiActions(argc, argv, myImage, imv);
      }
    else if (func == "genfourier" && (argc == 7 || argc == 8)){
      Image* myImage = new Image(atoi(argv[3]), atoi(argv[4]));
      myImage->generateImage(std::string(argv[2]), atof(argv[5]), atof(argv[6]));
      uiActions(argc, argv, myImage, imv);
    }
    else if (func == "FISH"){
      if (argc == 5 || argc == 6){
        std::cout << "Loading: " << std::endl << argv[2] << std::endl << argv[3] << std::endl << argv[4] << std::endl;
        Image* myImage = new Image(argv[2], argv[3], argv[4]);
        if (argc == 5){
          imv->showCombinedImage(myImage, 10);
        }
        else{
          imv->showCombinedImage(myImage, atoi(argv[5]));
        }
      }
      else invalidUiCall(func);
    }
    else if (func == "combine"){
      int nrImageRight;
      
      if (argc == 5) nrImageRight = 1;
      else if (argc == 6) nrImageRight = atoi(argv[5]);
      else invalidUiCall(func);

      std::cout << "Combining: " << std::endl << argv[2] << std::endl << argv[3] << std::endl << argv[4] << std::endl;
      Image* imageLeft = new Image(argv[2], argv[3], argv[4]);
      if (nrImageRight >= 1 && nrImageRight <= 3){
        QString filename(argv[nrImageRight + 1]);
        Image* imageRight = new Image(filename.toStdString());;
        imv->showCombinedImage(imageLeft, imageRight);
      }
      else {
        std::cout << "The number of the image to show on the right must be between 1 and 3" << std::endl;
        std::exit(0);
      }
    }
    else if (argc == 2){
      QString filename(argv[1]);
      std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
      imv->showFile(filename);
    }
    else{
      std::cout << "No such command exists for this program" << std::endl;
      std::exit(0);
    }
  }
  imv->show();
  imv->resize(1000,600);

  int ret = app.exec();

  delete(imv);
  return ret;
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
  funcMap["bottles"] = 4;
  funcMap["laplacian"] = 5;
  funcMap["fig3-43"] = 6;
  funcMap["fourier"] = 7;
  funcMap["circuitBoard"] = 7;
  funcMap["genfourier"] = 8;
  funcMap["frequencyfilter"] = 9;
  funcMap["cutout"] = 10;

  return funcMap;
}

void uiActions(int argc, char** argv, Image* myImage, QtImageViewer* imv){
  QString filename(argv[argc-1]);
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
      if (argc == 5){
        std::string* values = new std::string[2];
        values[0] = std::string(argv[2]);
        values[1] = std::string(argv[3]);
        imv->showImage(myImage, func, values);
      } 
      else invalidUiCall(func);
      break;
    case 7:
      if (argc == 4){
        std::string* values = new std::string[1];
        values[0] = std::string(argv[2]);
        imv->showImage(myImage, func, values, 1);
      }
      else if (argc == 5){
        std::string* values = new std::string[2];
        values[0] = std::string(argv[2]);
        values[1] = std::string(argv[3]);
        imv->showImage(myImage, func, values, 2);
      }
      else invalidUiCall(func);
      break;
    case 8:
      if (argc == 7) imv->showImage(myImage, func);
      else if (argc == 8){
        float* values = new float[1];
        values[0] = atof(argv[7]);
        imv->showImage(myImage, func, values, 1);
      }
      else invalidUiCall(func);
      break;
    case 9:
      if (argc == 6){
        float* values = new float[3]{atof(argv[2]), atoi(argv[3]), atoi(argv[4])};
        imv->showImage(myImage, func, values, 3);
      }
      else if (argc == 7){
        float* values = new float[4]{atof(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5])};
        imv->showImage(myImage, func, values, 4);
      }
      else if (argc == 8){
        float* values = new float[5]{atof(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6])};
        imv->showImage(myImage, func, values, 5);
      }
      else if (argc > 8 && (argc - 7) % 2 == 0){
        float* values = new float[4];
        values[0] = atof(argv[2]);
        values[1] = atoi(argv[3]);
        values[2] = atoi(argv[4]);
        values[3] = atoi(argv[5]);
        for (int i = 4; i < argc - 1; i ++){
          values[i] = atoi(argv[i + 2]);
        }
        imv->showImage(myImage, func, values, argc-3);
      }
      else invalidUiCall(func);
      break;
    case 10:
      if (argc == 7){
        float* values = new float[4]{atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5])};
        imv->showImage(myImage, func, values);
      }
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
