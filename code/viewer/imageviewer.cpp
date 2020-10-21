#include "qtimageviewer.hpp"
#include "image.hpp"

#include <iostream>
#include <QApplication>
#include <Eigen/Core>
#include <math.h>

#define PI 3.14159265

int main(int argc, char** argv){

  // return 0;

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
    else if (argc == 3 && std::string(argv[1]) == "change"){
      QString filename(argv[2]);
      Image* myImage = new Image(filename.toStdString(), 0);

      // unsigned short n = 2;
      // Eigen::Matrix3f m;
      // m.setIdentity();
      // m(0,0) = 10.0f;
      // m(1,1) = 10.0f;
      // Eigen::Matrix3f m1;
      // m1.setIdentity();
      // m1(0,0) = 0.866f;
      // m1(1,1) = 0.866f;
      // m1(1,0) = -0.5f;
      // m1(0,1) = 0.5f;
      // Eigen::Matrix3f* ms = new Eigen::Matrix3f[n];
      // ms[0] = m;
      // ms[1] = m1;

      unsigned short n = 1;
      Eigen::Matrix3f m;
      m.setIdentity();
      m(0,0) = 10.0f;
      m(1,1) = 10.0f;
      Eigen::Matrix3f* ms = new Eigen::Matrix3f[n];
      ms[0] = m;

      myImage->manipulateImage(n, ms);
      imv->showFile(myImage);
    }
    else if (argc == 3){
      QString filename(argv[1]);
      std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
      imv->showFile(filename, atoi(argv[2]));
    } 
    else if (argc == 5 && std::string(argv[1]) == "combine"){
      std::cout << "Combining: " << argv[2] << " " << argv[3] << " " << argv[4] << std::endl;
      Image* myImage = new Image(argv[2], argv[3], argv[4]);
      imv->showFile(myImage);
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
