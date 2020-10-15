#include "qtimageviewer.hpp"
#include "image.hpp"

#include <iostream>
#include <QApplication>

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
    else if (argc == 3){
      QString filename(argv[1]);
      std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
      imv->showFile(filename, atoi(argv[2]));
    } 
    else if (argc == 5 && std::string(argv[1]) == "combine"){
      std::cout << "Combining " << argv[2] << " " << argv[3] << " " << argv[4] << std::endl;
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
