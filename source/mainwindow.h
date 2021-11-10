#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QColor>

#include "coupledpendulum.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  unsigned int IMG_SIZE; // size of rendered image
  unsigned int imgFrameCounter; // count frame number
  CoupledPendulum * cp;
  vector < CoupledPendulum * > pendula;
  vector <QColor> colors;
  bool isRunning;

  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow( );

private slots:
  void on_nextButton_clicked( );
  void UpdateGUI( );
  void Next( );

  void on_resetButton_clicked();

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
