#include "qtimageviewer.hpp"
#include "image.hpp"

#include <iostream>
#include <QApplication>
#include <Eigen/Core>
#include <math.h>

int main(int argc, char** argv){

  // Eigen::Vector3f worldCoordinate{0.0f,0.0f,1.0f};
  // Eigen::Vector3i indexCoordinate{0,0,1};

  // Eigen::Matrix3f m;
  // m.setIdentity();
  // m(0,0) = 2.0f;
  // m(1,1) = 2.0f;

  // std::cout << worldCoordinate << " ";
  // std::cout << indexCoordinate << std::endl;
  // std::cout << m << std::endl;

  // float f = 9.49;
  // int i = static_cast<int>(round(f));
  // std::cout << i << std::endl;

  // Eigen::Matrix3f m;
  // Eigen::Vector3f v{0,1,1};
  // m.setIdentity();
  // m(0,1) = -0.0f;
  // m(1,0) = -0.0f;
  // m(2,0) = -0.0f;
  // m(2,1) = -0.0f;
  // m(1,2) = 62;
  // m(1,1) = -1;
  // std::cout << m << std::endl;
  // std::cout << v << std::endl;
  // std::cout << m * v << std::endl;

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
      Eigen::Matrix3f m;
      m.setIdentity();
      m(0,0) = 20.0f;
      m(1,1) = 20.0f;
      Eigen::Matrix3f* ms = new Eigen::Matrix3f[1];
      ms[0] = m;
      myImage->manipulateImage(1, ms);
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
