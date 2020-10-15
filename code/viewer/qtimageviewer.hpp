#ifndef QT_IMAGE_VIEWER_MAINWINDOW_HPP
#define QT_IMAGE_VIEWER_MAINWINDOW_HPP

#include <image.hpp>

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QScrollArea>

class QtImageViewer : public QMainWindow{
  Q_OBJECT /// Qt Syntax. Must be included.
public:
  QtImageViewer(QWidget *parent=nullptr);
  virtual ~QtImageViewer();

public slots: /// Qt Syntax Signal Slot mechanism.
  void showFile(const QString filename, short directory);
  void showFile(Image* myImage);
  void openFile();
  void quit();

private:
  void init();
  void createActions();
  void createMenus();
  // Qt Image related
  QImage _qImage;
  QLabel *_qImageLabel{nullptr};
  QScrollArea *_qScrollArea{nullptr};
  // Qt interface related
  QMenu *_fileMenu{nullptr};
  QAction *_fileOpenAction{nullptr};
  QAction *_quitAction{nullptr};
};
#endif
