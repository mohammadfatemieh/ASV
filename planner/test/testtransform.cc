/*
***********************************************************************
* testtransform.cc:
* Utility test for transformation from Cartesian x,y coordinates
* to Frenet s,d coordinates
* This header file can be read by C++ compilers
*
* by Hu.ZH(CrossOcean.ai)
***********************************************************************
*/

#include <cstdlib>
#include "timecounter.h"
#include "trajectorygenerator.h"
#include "utilityio.h"

using namespace ASV;

int main() {
  el::Loggers::addFlag(el::LoggingFlag::CreateLoggerAutomatically);
  LOG(INFO) << "The program has started!";

  CartesianState cartesianstate{
      1,            // x
      0,            // y
      -M_PI / 3.0,  // theta
      0.11,         // kappa
      2,            // speed
      0,            // dspeed
  };
  FrenetState frenetstate{
      10,  // s
      0,   // s_dot
      0,   // s_ddot
      2,   // d
      0,   // d_dot
      0,   // d_ddot
      0,   // d_prime
      0    // d_pprime
  };
  Eigen::VectorXd X(5);
  Eigen::VectorXd Y(5);
  X << 0.0, 10.0, 20.5, 35.0, 70.5;
  Y << 0.0, -6.0, 5.0, 6.5, 0.0;
  trajectorygenerator _trajectorygenerator(X, Y);

  transformc2f(_trajectorygenerator, frenetstate, cartesianstate);
  std::cout << "Results of Transformation from Cartesian to Frenet\n";
  std::cout << "s: " << frenetstate.s << std::endl;
  std::cout << "s_dot: " << frenetstate.s_dot << std::endl;
  std::cout << "s_ddot: " << frenetstate.s_ddot << std::endl;
  std::cout << "d: " << frenetstate.d << std::endl;
  std::cout << "d_dot: " << frenetstate.d_dot << std::endl;
  std::cout << "d_ddot: " << frenetstate.d_ddot << std::endl;
  std::cout << "d_prime: " << frenetstate.d_prime << std::endl;
  std::cout << "d_pprime: " << frenetstate.d_pprime << std::endl;

  transformf2c(_trajectorygenerator, frenetstate, cartesianstate);
  std::cout << "Results of Transformation from Frenet to Cartesian\n";
  std::cout << "x: " << cartesianstate.x << std::endl;
  std::cout << "y: " << cartesianstate.y << std::endl;
  std::cout << "theta: " << cartesianstate.theta << std::endl;
  std::cout << "kappa: " << cartesianstate.kappa << std::endl;
  std::cout << "speed: " << cartesianstate.speed << std::endl;
  std::cout << "a: " << cartesianstate.dspeed << std::endl;

  LOG(INFO) << "Shutting down.";
}