#include "qtimageviewer.hpp"

#include <QFileDialog>
#include <QPalette>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <image.hpp> /// include your imagelibrary

QtImageViewer::QtImageViewer(QWidget *parent) :
  QMainWindow(parent){
  init();
};

void  QtImageViewer::init(){
  _qImageLabel = new QLabel;
  
  _qImageLabel->setBackgroundRole(QPalette::Base);
  _qImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  _qImageLabel->setScaledContents(false);
  
  _qScrollArea = new QScrollArea;
  _qScrollArea->setBackgroundRole(QPalette::Dark);
  _qScrollArea->setWidget(_qImageLabel);
  _qScrollArea->setVisible(true);

  setCentralWidget(_qScrollArea);
  createActions();
}

QtImageViewer::~QtImageViewer(){
  delete(_qImageLabel);
  delete(_qScrollArea);
  delete(_fileMenu);
  delete(_fileOpenAction);
  delete(_quitAction);
};

void QtImageViewer::createActions(){
  _fileOpenAction = new QAction(tr("&Open..."), this);
  _fileOpenAction->setShortcut(QKeySequence::Open);
  connect(_fileOpenAction, SIGNAL(triggered()), this, SLOT(openFile()));

  _quitAction = new QAction(tr("&Quit..."), this);
  _quitAction->setShortcut(QKeySequence::Quit);
  connect(_quitAction, SIGNAL(triggered()), this, SLOT(quit()));

  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(_fileOpenAction);
  _fileMenu->addAction(_quitAction);
}

void QtImageViewer::quit(){
	close();
};

void QtImageViewer::openFile(){
  QFileDialog dialog(this, tr("Open File"));
  QString filename = dialog.getOpenFileName(this, "Select image to open");
  std::cout<<"Opening: "<<filename.toStdString()<<std::endl;

  showFile(filename, 0);
}

void QtImageViewer::showFile(const QString filename, short directory){  

  Image* myImage = new Image(filename.toStdString(), directory);   // Load using your library!

  QImage::Format format = QImage::Format_Invalid;
  // We deal only with 8 bits per channel for now.
  // Check for RGB vs Grayscale
  if(myImage->getChannels() == 3)
    format = QImage::Format_RGB888;
  else if (myImage->getChannels() == 1)
    format = QImage::Format_Grayscale8;

  QImage qImg(myImage->getImageData(),
	      myImage->getWidth(),
	      myImage->getHeight(),
        myImage->getWidth()*myImage->getChannels()*myImage->getBPP()/8,
	      format);
  std::cout << "good" << std::endl;
  
  _qImageLabel->setPixmap(QPixmap::fromImage(qImg));
  _qImageLabel->resize(_qImageLabel->pixmap()->size());
  _qScrollArea->setVisible(true);
  std::cout << "good" << std::endl;

  update(); /// Force Qt to redraw
  delete(myImage); /// Data is copied to Qt, we can delete our image.
  std::cout << "good" << std::endl;

  //std::this_thread::sleep_for(std::chrono::seconds(30));
}

void QtImageViewer::showFile(Image* myImage){
  
  QImage::Format format = QImage::Format_Invalid;
  // We deal only with 8 bits per channel for now.
  // Check for RGB vs Grayscale
  if(myImage->getChannels() == 3)
    format = QImage::Format_RGB888;
  else if (myImage->getChannels() == 1)
    format = QImage::Format_Grayscale8;

  QImage qImg(myImage->getImageData(),
	      myImage->getWidth(),
	      myImage->getHeight(),
        myImage->getWidth()*myImage->getChannels()*myImage->getBPP()/8,
	      format);
  
  _qImageLabel->setPixmap(QPixmap::fromImage(qImg));
  _qImageLabel->resize(_qImageLabel->pixmap()->size());
  _qScrollArea->setVisible(true);

  update(); /// Force Qt to redraw
  delete(myImage); /// Data is copied to Qt, we can delete our image.
}
