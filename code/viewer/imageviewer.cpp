#include "qtimageviewer.hpp"
#include "image.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <QApplication>
#include <Eigen/Core>
#include <math.h>

Image* uiActions(int argc, char** argv);
void invalidUiCall(std::string func);
std::map<std::string, int> getFuncMap();

int main(int argc, char** argv){
  // Start Qt framework
  QApplication app( argc, argv );

  QtImageViewer* imv = new QtImageViewer();
  // Optionally give image on command line
  if(argc >= 2){
    std::string func = std::string(argv[1]);
    if (argc == 2){
      QString filename(argv[1]);
      std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
      imv->showFile(filename, 0);
    }
    else if (func == "scale" || func == "rotate" || func == "shear"
      || func == "transform1" || func == "transform2" || func == "transform3"
      || func == "combine"){
      Image* myImage = uiActions(argc, argv);
      imv->showFile(myImage);
    }
    else if (argc == 3){
      QString filename(argv[1]);
      std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
      imv->showFile(filename, atoi(argv[2]));
    } 
    else if (func == "combine"){
      if (argc == 5){
        std::cout << "Combining: " << argv[2] << " " << argv[3] << " " << argv[4] << std::endl;
        Image* myImage = new Image(argv[2], argv[3], argv[4]);
        imv->showFile(myImage);
      }
      else invalidUiCall(func);
    }
    else{
      std::cout << "No such command exists for this program" << std::endl;
      std::exit(0);
    }
  }
  // Draw our GUI
  imv->show();
  imv->resize(1000,600);

  // Run Qt application until someone presses quit
  int ret = app.exec();

  delete(imv);
  return ret;
}

std::map<std::string, int> getFuncMap(){
  std::map<std::string, int> funcMap;
  funcMap["scale"] = 1;
  funcMap["rotate"] = 2;
  funcMap["shear"] = 3;
  funcMap["transform1"] = 4;
  funcMap["transform2"] = 5;
  funcMap["transform3"] = 6;

  return funcMap;
}


Image* uiActions(int argc, char** argv){
  QString filename(argv[argc-1]);
  Image* myImage = new Image(filename.toStdString());

  std::string func = std::string(argv[1]);
  std::map<std::string, int> funcMap = getFuncMap();

  switch(funcMap[func]){
    case 1:
      if (argc == 5) myImage->scaleImage(std::string(argv[2]), atof(argv[3]), atof(argv[3]));
      else if (argc == 6) myImage->scaleImage(std::string(argv[2]), atof(argv[3]), atof(argv[4]));
      else invalidUiCall(func);
      break;
    case 2:
      if (argc == 5) myImage->rotateImage(std::string(argv[2]), atof(argv[3]));
      else invalidUiCall(func);
      break;
    case 3:
      if (argc == 5) myImage->shearImage(std::string(argv[2]), atof(argv[3]));
      else invalidUiCall(func);
      break;
    case 4:
      if (argc == 4) myImage->presetTransformations(std::string(argv[2]), 0);
      else {
        func.pop_back();
        invalidUiCall(func);
      }
      break;
    case 5:
      if (argc == 4) myImage->presetTransformations(std::string(argv[2]), 1);
      else {
        func.pop_back();
        invalidUiCall(func);
      }
      break;
    case 6:
      if (argc == 4) myImage->presetTransformations(std::string(argv[2]), 2);
      else {
        func.pop_back();
        invalidUiCall(func);
      }
      break;
    default: 
      std::cout << "Invalid input";
      std::exit(0);
      break;
  }
  std::cout<<"Loading and doing operation " << argv[1] << ":" << filename.toStdString() << std::endl;
  return myImage;
}

void invalidUiCall(std::string func){
  std::cout << "Invalid number of arguments for operation " << func << std::endl;
  std::exit(0);
}