#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QTimer>
#include <QString>
#include <ctime>
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {
  ui->setupUi(this);

  this->IMG_SIZE = 600;
  this->imgFrameCounter = 1;

  this->colors.clear( );
  colors.push_back(Qt::red);
  colors.push_back(Qt::black);
  colors.push_back(Qt::blue);
  colors.push_back(Qt::green);
  colors.push_back(Qt::cyan);
  colors.push_back(Qt::magenta);
  colors.push_back(Qt::yellow);
  colors.push_back(Qt::darkBlue);
  colors.push_back(Qt::darkGreen);
  colors.push_back(Qt::darkCyan);
  colors.push_back(Qt::darkMagenta);
  colors.push_back(Qt::darkYellow);

  this->cp = new CoupledPendulum(1, 0.75, 1, 1, 900);
  this->isRunning = false;

  QTimer::singleShot(25, this, SLOT(on_resetButton_clicked( )));

  QImage img = cp->ToQImage(ui->smoothBox->isChecked( ));

  ui->imgLabel->setPixmap(QPixmap::fromImage(img.scaled(IMG_SIZE, IMG_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

  QTimer::singleShot(500, this, SLOT(UpdateGUI( )));
  QTimer::singleShot(50, this, SLOT(Next( )));
} // MainWindow

MainWindow::~MainWindow( ) {
  delete ui;
} // ~MainWindow


// toggles play/pause
void MainWindow::on_nextButton_clicked( ) {

  if (this->isRunning) {
    isRunning = false;
    ui->nextButton->setText("Run");

    this->cp->stepNum = 0;
    this->cp->T = 0;
    this->cp->v_theta1 = 0;
    this->cp->v_theta2 = 0;
  } // if
  else {
    isRunning = true;
    ui->nextButton->setText("Pause");
  } // else

} // on_nextButton_clicked



// computes next state of pendulum
void MainWindow::Next( ) {
  if (isRunning) {

    if (ui->frameCheck->isChecked( )) {
      QString frameStr = QString::number(this->imgFrameCounter);
      while (frameStr.length( ) < 3)
        frameStr = "0" + frameStr;

      QString str = "/Users/fmarsman/Dropbox/Qt_Projects/DoublePendulum/frames/frame_";
      str += frameStr;
      str += ".png";
      //auto active_window = qApp->activeWindow();
      //if (active_window) { // could be null if your app doesn't have focus
      //  QPixmap pixmap(active_window->size());
      //  active_window->render(&pixmap);
      //  QImage image = pixmap.toImage();
      //  bool success = image.save(str, "png");
      //  qDebug( ) << "SImage size!" << image.width( ) << "sucess = " << success << "name = " << str;
      QImage img = this->pendula[0]->ToQImage(ui->smoothBox->isChecked( ));
      bool success = img.save(str, "png");
      qDebug( ) << "SImage size!" << img.width( ) << "sucess = " << success << "name = " << str;
      ui->statusBar->showMessage("Frame: " + QString::number(imgFrameCounter));
      this->imgFrameCounter++;
    } // if

    this->cp->Next(ui->stepBox->value( ));

    for (unsigned int i = 0; i < this->pendula.size( ); i++)
      this->pendula[i]->Next(ui->stepBox->value( ));

    QTimer::singleShot(1, this, SLOT(Next( )));
    return;
  } // if
  else {
    QTimer::singleShot(50, this, SLOT(Next( )));
    return;
  } // else
} // Next


// updates GUI
void MainWindow::UpdateGUI( ) {
  for (unsigned int i = 0; i < this->pendula.size( ); i++) {
    this->pendula[i]->dt = ui->dtBox->value( ) * 0.001;
    this->pendula[i]->m1 = double(ui->m1Slider->value( )) * 0.001;
    this->pendula[i]->m2 = double(ui->m2Slider->value( )) * 0.001;
    this->pendula[i]->l1 = double(ui->l1Slider->value( )) * 0.001;
    this->pendula[i]->l2 = double(ui->l2Slider->value( )) * 0.001;
  } // for

  if (isRunning == false && this->pendula.size( ) == 1) {
    this->pendula[0]->theta1 = double(ui->theta1Slider->value( ) - 180) * (6.28318530718 / 360.0);
    this->pendula[0]->theta2 = double(ui->theta2Slider->value( ) - 180) * (6.28318530718 / 360.0);
  } // if
  else {
    ui->theta1Slider->setValue(int((this->pendula[0]->theta1 * (360.0 / 6.28318530718)) + 180) % 360);
    ui->theta2Slider->setValue(int((this->pendula[0]->theta2 * (360.0 / 6.28318530718)) + 180) % 360);
  } // else

  //QImage img = cp->ToQImage( );

  QImage img = this->pendula[0]->ToQImage(ui->smoothBox->isChecked( ));

  for (unsigned int i = 1; i < this->pendula.size( ); i++) {
    img = this->pendula[i]->DrawOnQImage(img);
  } // for

  ui->imgLabel->setPixmap(QPixmap::fromImage(img));

  ui->statusBar->showMessage("T = " + QString::number(this->pendula[0]->T, 'g', 0) + ", N = " + QString::number(double(this->pendula[0]->stepNum)/1000) + "k");

  QTimer::singleShot(25, this, SLOT(UpdateGUI( )));
} // UpdateGUI


void MainWindow::on_resetButton_clicked( ) {
  srand(time(NULL));

  qDebug( ) << "RESET!";
  int nCP = ui->nPendulaBox->value( );
  double dTheta = pow(10, -1 * ui->deltaThetaBox->value( ));

  this->pendula.clear( );
  for (int i = 0; i < nCP; i++) {
    qDebug( ) << "adding pendula, i =" << i;
    this->pendula.push_back(new CoupledPendulum(1, 0.75, 1, 1, 600));
    this->pendula[i]->dt = ui->dtBox->value( ) * 0.001;
    this->pendula[i]->m1 = double(ui->m1Slider->value( )) * 0.001;
    this->pendula[i]->m2 = double(ui->m2Slider->value( )) * 0.001;
    this->pendula[i]->l1 = double(ui->l1Slider->value( )) * 0.001;
    this->pendula[i]->l2 = double(ui->l2Slider->value( )) * 0.001;
    this->pendula[i]->theta1 = double(ui->theta1Slider->value( ) - 180) * (6.28318530718 / 360.0);
    this->pendula[i]->theta2 = double(ui->theta2Slider->value( ) - 180) * (6.28318530718 / 360.0);

    if (i > 0) {
//      this->pendula[i]->theta1 += (double(double(rand( )) / RAND_MAX) - 0.5) * dTheta;
//      this->pendula[i]->theta2 += (double(double(rand( )) / RAND_MAX) - 0.5) * dTheta;
      this->pendula[i]->theta1 += (double(i) / nCP) * dTheta;
      // this->pendula[i]->theta2 += (double(i) / nCP) * dTheta;
    } // if

    int colNr = i % this->colors.size( );
    this->pendula[i]->pathColor = this->colors[colNr];
  } // for
} // on_resetButton_clicked
