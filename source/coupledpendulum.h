#ifndef COUPLEDPENDULUM_H
#define COUPLEDPENDULUM_H

#include <QDebug>
#include <vector>
#include <QImage>
#include <QColor>

using namespace std;

// class describes double pendulum system, with variable rod
// lengths and variable masses. Class can evolve system over time
class CoupledPendulum
{
public:
  // tracking
  vector < vector < unsigned int > > path;

  // system constants
  double l1, l2; // rod lengths [m]
  double m1, m2; // point masses [kg]
  double g; // gravitational acceleration [m/s^2]
  QColor pathColor;
  unsigned int SIZE;

  // system variables
  double dt; // time step [s]
  double T; // global time
  unsigned int stepNum; // how many steps
  double theta1, theta2; // angles [rad]
  double v_theta1, v_theta2; // angle derivatives [rad/s]

  void Next(unsigned int nSteps = 1);

  QImage ToQImage(bool smooth = false);
  QImage DrawOnQImage(QImage baseImg);

  CoupledPendulum(double _l1, double _l2, double _m1, double _m2, unsigned int _SIZE);
};

#endif // COUPLEDPENDULUM_H
