#include "coupledpendulum.h"

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>



// computes next state of system [nSteps] times, so state
// moves [dt] * [nSteps] time forwards
void CoupledPendulum::Next(unsigned int nSteps) {
  double m1m2 = m1 + m2;
  double dTheta;
  double v_theta1_sqr, v_theta2_sqr; // squared values
  double a_theta1, a_theta2; // accelerations
  double temp;
  double PI2 = 6.28318530718;

  double totLen = (l1 + l2) * 2.2;
  double mid = double(SIZE) / 2;
  double scale = double(SIZE) / totLen;

  for (unsigned int i = 0; i < nSteps; i++) {
    dTheta = theta1 - theta2;
    v_theta1_sqr = v_theta1 * v_theta1;
    v_theta2_sqr = v_theta2 * v_theta2;

    // compute a_theta1
    a_theta1 = 0;
    a_theta1 -= g * (2*m1 + m2) * sin(theta1);
    a_theta1 -= g * m2 * sin(theta1 - 2*theta2);

    temp = -2.0 * sin(dTheta) * m2;
    temp *= v_theta2_sqr * l2 + v_theta1_sqr * l1 * cos(dTheta);

    a_theta1 += temp;

    temp = l1;
    temp *= 2 * m1 + m2 - m2 * cos(2 * dTheta);

    a_theta1 /= temp;

    // compute a_theta2
    a_theta2 = 0;
    a_theta2 += 2.0 * sin(dTheta);

    temp = v_theta1_sqr * l1 * m1m2;
    temp += g * m1m2 * cos(theta1);
    temp += v_theta2_sqr * l2 * m2 * cos(dTheta);

    a_theta2 *= temp;

    temp = l2;
    temp *= 2 * m1 + m2 - m2 * cos(2 * dTheta);

    a_theta2 /= temp;

    theta1 += v_theta1 * dt;  // + 0.5 * a_theta1 * dt * dt;
    theta2 += v_theta2 * dt; // + 0.5 * a_theta2 * dt * dt;

    v_theta1 += a_theta1 * dt;
    v_theta2 += a_theta2 * dt;

    if (theta1 < 0)
      theta1 += PI2;
    if (theta2 < 0)
      theta2 += PI2;

    // compute positions of the two points
    double dx1 = sin(theta1) * l1 * scale;
    double dy1 = cos(theta1) * l1 * scale;
    double dx2 = dx1 + sin(theta2) * l2 * scale;
    double dy2 = dy1 + cos(theta2) * l2 * scale;

    this->path[mid + dx2][mid + dy2]++;
  } // for

  this->T += dt * nSteps;
  this->stepNum += nSteps;
} // Next

QImage CoupledPendulum::ToQImage(bool smooth) {
  double totLen = (l1 + l2) * 2.2;
  double mid = double(SIZE) / 2;
  double scale = double(SIZE) / totLen;

  // compute positions of the two points
  double dx1 = sin(theta1) * l1 * scale;
  double dy1 = cos(theta1) * l1 * scale;
  double dx2 = dx1 + sin(theta2) * l2 * scale;
  double dy2 = dy1 + cos(theta2) * l2 * scale;

  QPointF pos1(mid + dx1, mid + dy1);
  QPointF pos2(mid + dx2, mid + dy2);

  //QImage img(SIZE, SIZE, QImage::Format_RGB888);
  QImage img(SIZE, SIZE, QImage::Format_ARGB32);


  double maxVal = 0;
  for (unsigned int i = 0; i < SIZE; i++)
    for (unsigned int j = 0; j < SIZE; j++)
      if (path[i][j] > maxVal)
        maxVal = path[i][j];

  if (maxVal == 0)
    maxVal = 1;

  for (unsigned int i = 0; i < SIZE; i++)
    for (unsigned int j = 0; j < SIZE; j++) {
      //double val = sqrt(double(path[i][j]) / maxVal);
      double val = pow(double(path[i][j]) / maxVal, 0.1);
      if (smooth == false)
        val = (path[i][j] > 0)? 1 : 0;

      double cr = pathColor.red( ) * val + 255 * (1.0 - val);
      double cg = pathColor.green( ) * val + 255 * (1.0 - val);
      double cb = pathColor.blue( ) * val + 255 * (1.0 - val);
      img.setPixelColor(i, j, QColor(cr, cg, cb, 255 * val));
      //img.setPixelColor(i, j, QColor((1.0 - val) * 255, (1.0 - val) * 255, 255));
    } // for

  QPainter qPainter(&img);
  qPainter.setRenderHint(QPainter::Antialiasing);

  QPen qPen(Qt::black);
  qPen.setWidthF(2.8);
  qPainter.setPen(qPen);

  // draw two rods:
  qPainter.drawLine(QPointF(mid, mid), pos1);
  qPainter.drawLine(pos1, pos2);

  qPainter.setBrush(Qt::yellow);
  qPen.setColor(Qt::black);
  qPainter.setPen(qPen);

  // draw center:
  qPainter.drawEllipse(QPointF(mid, mid), 8, 8);

  qPainter.setBrush(Qt::green);
  qPen.setWidthF(1.5);
  qPainter.setPen(qPen);

  // draw two points:
  double avgM = (m1 + m2) / 2;
  qPainter.drawEllipse(pos1, 5.0 * m1 / avgM, 5.0 * m1 / avgM);
  qPainter.drawEllipse(pos2, 5.0 * m2 / avgM, 5.0 * m2 / avgM);

  qPainter.end( );

  return img;
} // ToQImage

QImage CoupledPendulum::DrawOnQImage(QImage baseImg) {
  double totLen = (l1 + l2) * 2.2;
  double mid = double(SIZE) / 2;
  double scale = double(SIZE) / totLen;

  // compute positions of the two points
  double dx1 = sin(theta1) * l1 * scale;
  double dy1 = cos(theta1) * l1 * scale;
  double dx2 = dx1 + sin(theta2) * l2 * scale;
  double dy2 = dy1 + cos(theta2) * l2 * scale;

  QPointF pos1(mid + dx1, mid + dy1);
  QPointF pos2(mid + dx2, mid + dy2);

  double maxVal = 0;
  for (unsigned int i = 0; i < SIZE; i++)
    for (unsigned int j = 0; j < SIZE; j++)
      if (path[i][j] > maxVal)
        maxVal = path[i][j];

  if (maxVal == 0)
    maxVal = 1;

  for (unsigned int i = 0; i < SIZE; i++)
    for (unsigned int j = 0; j < SIZE; j++) {
      double val = sqrt(double(path[i][j]) / maxVal);
      double cr = pathColor.red( ) * val + 255 * (1.0 - val);
      double cg = pathColor.green( ) * val + 255 * (1.0 - val);
      double cb = pathColor.blue( ) * val + 255 * (1.0 - val);
      if (val > 0)
        baseImg.setPixelColor(i, j, QColor(cr, cg, cb));
    } // for

  QPainter qPainter(&baseImg);
  qPainter.setRenderHint(QPainter::Antialiasing);

  QPen qPen(Qt::black);
  qPen.setWidthF(1.5);
  qPainter.setPen(qPen);

  // draw two rods:
  qPainter.drawLine(QPointF(mid, mid), pos1);
  qPainter.drawLine(pos1, pos2);

  qPainter.setBrush(Qt::black);

  // draw center:
  qPainter.drawEllipse(QPointF(mid, mid), 4, 4);

  qPainter.setBrush(Qt::red);
  qPen.setWidthF(0.5);
  qPainter.setPen(qPen);

  // draw two points:
  double avgM = (m1 + m2) / 2;
  qPainter.drawEllipse(pos1, 3.0 * m1 / avgM, 3.0 * m1 / avgM);
  qPainter.drawEllipse(pos2, 3.0 * m2 / avgM, 3.0 * m2 / avgM);

  qPainter.end( );

  return baseImg;
} // DrawOnQImage

CoupledPendulum::CoupledPendulum(double _l1, double _l2, double _m1, double _m2, unsigned int _SIZE) {
  this->l1 = _l1;
  this->l2 = _l2;
  this->m1 = _m1;
  this->m2 = _m2;
  this->SIZE = _SIZE;

  this->path = vector < vector < unsigned int > > (SIZE, vector < unsigned int > (SIZE, 0));

  this->g = 9.81;
  this->dt = 0.01;
  this->T = 0;
  this->stepNum = 0;
  this->pathColor = QColor(0, 0, 255);

  this->theta1 = 0.2;
  this->theta2 = -0.5;
  this->v_theta1 = 0;
  this->v_theta2 = 0;

  qDebug( ) << "NEW MOFO!";
} // CoupledPendulum
