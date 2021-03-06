#ifndef QT_IMAGE_VIEWER_MAINWINDOW_HPP
#define QT_IMAGE_VIEWER_MAINWINDOW_HPP

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QScrollArea>

#include <QSplitter>
#include <QChartView>

#include <image.hpp>

class QtImageViewer : public QMainWindow{

  Q_OBJECT

public:
  QtImageViewer(QWidget *parent=nullptr);
  QtImageViewer(const QString filename, QWidget *parent=nullptr);

  virtual ~QtImageViewer();
			  
public slots:
  void showFile(const QString filename);
  void showImage(Image *img);
  void showImage(Image *img, std::string transformationType, float* values = nullptr, int nrOfValues = 0);
  void showImage(Image *img, std::string transformationType, std::string* values, int nrOfValues = 0);
  void showImageLeft(const Image *img);
  void showImageRight(const Image *img);
  void showCombinedImage(Image *img, uint16 visualizedStep);
  void showCombinedImage(Image *imageLeft, Image *imageRight);
  void openFile();
  void quit();

private:
  void init();
  void createActions();
  void createMenus();

  QImage _lImage;
  QLabel *_lImageLabel{nullptr};
  QScrollArea *_lScrollArea{nullptr};

  QImage _rImage;
  QLabel *_rImageLabel{nullptr};
  QScrollArea *_rScrollArea{nullptr};
  
  QMenu *_fileMenu{nullptr};
  
  QAction *_fileOpenAction{nullptr};
  QAction *_quitAction{nullptr};

  QSplitter *_mainSplitter{nullptr};
  QSplitter *_leftSplitter{nullptr};
  QSplitter *_rightSplitter{nullptr};
  QtCharts::QChartView *_lChartView{nullptr};
  QtCharts::QChartView *_rChartView{nullptr};
};

#endif
