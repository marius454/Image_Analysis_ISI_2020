#include "qtimageviewer.hpp"
#include "image.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <QApplication>
#include <Eigen/Core>
#include <math.h>


int main(int argc, char** argv){
  // Start Qt framework
  QApplication app( argc, argv );

  QtImageViewer* imv = new QtImageViewer();
  // Optionally give image on command line
  if(argc >= 2){
    if (argc == 2){
      QString filename(argv[1]);
      std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
      imv->showFile(filename, 0);
    }
    else if (std::string(argv[1]) == "scale"){
      if (argc == 5){
        QString filename(argv[4]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and scaling: "<<filename.toStdString()<<std::endl;
        myImage->scaleImage(std::string(argv[2]), atof(argv[3]), atof(argv[3]));
        imv->showFile(myImage);
      }
      else if (argc == 6){
        QString filename(argv[5]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and scaling: "<<filename.toStdString()<<std::endl;
        myImage->scaleImage(std::string(argv[2]), atof(argv[3]), atof(argv[4]));
        imv->showFile(myImage);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'scale'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "rotate"){
      if (argc == 5){
        QString filename(argv[4]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and rotating: "<<filename.toStdString()<<std::endl;
        myImage->rotateImage(std::string(argv[2]), atof(argv[3]));
        imv->showFile(myImage);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'rotate'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "shear"){
      if (argc == 5){
        QString filename(argv[4]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and flipping: "<<filename.toStdString()<<std::endl;
        myImage->shearImage(std::string(argv[2]), atof(argv[3]));
        imv->showFile(myImage);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'shear'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "transform1"){
      if (argc == 4){
        QString filename(argv[3]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and transforming: "<<filename.toStdString()<<std::endl;
        myImage->presetTransformations(std::string(argv[2]), 0);
        imv->showFile(myImage);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'transform'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "transform2"){
      if (argc == 4){
        QString filename(argv[3]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and transforming: "<<filename.toStdString()<<std::endl;
        myImage->presetTransformations(std::string(argv[2]), 1);
        imv->showFile(myImage);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'transform'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "transform3"){
      if (argc == 4){
        QString filename(argv[3]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and transforming: "<<filename.toStdString()<<std::endl;
        myImage->presetTransformations(std::string(argv[2]), 2);
        imv->showFile(myImage);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'transform'" << std::endl;
        std::exit(0);
      }
    }
    else if (argc == 3){
      QString filename(argv[1]);
      std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
      imv->showFile(filename, atoi(argv[2]));
    } 
    else if (std::string(argv[1]) == "combine"){
      if (argc == 5){
        std::cout << "Combining: " << argv[2] << " " << argv[3] << " " << argv[4] << std::endl;
        Image* myImage = new Image(argv[2], argv[3], argv[4]);
        imv->showFile(myImage);
      }else{
        std::cout << "Invalid number of arguments for operation 'combine'" << std::endl;
        std::exit(0);
      }
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



// else if (argc == 3 && std::string(argv[1]) == "change"){
    //   QString filename(argv[2]);
    //   Image* myImage = new Image(filename.toStdString(), 0);

    //   // unsigned short n = 2;
    //   // Eigen::Matrix3f m;
    //   // m.setIdentity();
    //   // m(0,0) = 10.0f;
    //   // m(1,1) = 10.0f;
    //   // Eigen::Matrix3f m1;
    //   // m1.setIdentity();
    //   // m1(0,0) = 0.866f;
    //   // m1(1,1) = 0.866f;
    //   // m1(1,0) = -0.5f;
    //   // m1(0,1) = 0.5f;
    //   // Eigen::Matrix3f* ms = new Eigen::Matrix3f[n];
    //   // ms[0] = m;
    //   // ms[1] = m1;

    //   unsigned short n = 1;
    //   Eigen::Matrix3f m;
    //   m.setIdentity();
    //   m(0,0) = 2.0f;
    //   m(1,1) = 2.0f;
    //   Eigen::Matrix3f* ms = new Eigen::Matrix3f[n];
    //   ms[0] = m;

    //   myImage->manipulateImage(n, ms);
    //   imv->showFile(myImage);
    // }