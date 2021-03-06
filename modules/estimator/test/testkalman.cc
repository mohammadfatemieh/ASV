
#include "common/fileIO/include/utilityio.h"
#include "modules/estimator/include/eigenmvnd.hpp"
#include "modules/estimator/include/kalmanfilter.h"
using namespace ASV;
using namespace ASV::common;

void test1d() { /* Set Matrix and Vector for Kalman Filter: */
  Eigen::MatrixXd A(1, 1);
  A << 1;
  Eigen::MatrixXd B(1, 1);
  B << 1;
  Eigen::MatrixXd H(1, 1);
  H << 1;
  Eigen::MatrixXd Q(1, 1);
  Q << 1;
  Eigen::MatrixXd R(1, 1);
  R << 0.1;
  Eigen::VectorXd X0(1);
  X0 << 0;
  Eigen::MatrixXd P0(1, 1);
  P0 << 1;

  /* Create measure vector, and store measure value */
  Eigen::VectorXd Z(1);
  double mesaure[10] = {0.39, 0.50, 0.48, 0.29, 0.25,
                        0.32, 0.34, 0.48, 0.41, 0.45};

  Eigen::VectorXd U(1);
  U << 0;
  /* Create The Filter */
  kalmanfilter<1, 1, 1> filter1(A, B, H, Q, R);
  filter1.setInitial(X0, P0);
  /* This loop simulate the measure/prediction process */
  for (int i = 0; i < 10; ++i) {
    Z << mesaure[i];
    filter1.linearkalman(A, B, U, Z);
    std::cout << "X" << i << ": " << filter1.getState() << std::endl;
  }
}

void test2d() {
  constexpr int n = 4;
  constexpr int m = 2;
  constexpr int l = 1;
  Eigen::Matrix<double, n, n> A;
  A << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1;
  Eigen::Matrix<double, n, l> B;
  B.setZero();
  Eigen::Matrix<double, m, n> H;
  H << 1, 0, 0, 0, 0, 1, 0, 0;
  Eigen::Matrix<double, n, n> Q = 0.1 * Eigen::Matrix<double, n, n>::Identity();
  Eigen::Matrix<double, m, m> R = 1 * Eigen::Matrix<double, m, m>::Identity();

  Eigen::Matrix<double, n, 1> initx;
  initx << 10, 10, 1, 0;
  Eigen::Matrix<double, n, n> initP =
      10 * Eigen::Matrix<double, n, n>::Identity();

  // generate multivariate normal distribution to simulate gaussian noise
  int _TotalT = 50;
  eigenmvnd normal_Q(Eigen::MatrixXd::Zero(n, 1), Q, _TotalT);
  eigenmvnd normal_R(Eigen::MatrixXd::Zero(m, 1), R, _TotalT);
  Eigen::MatrixXd sample_normal_Q = normal_Q.perform_mvnd().get_mvnd_matrix();
  Eigen::MatrixXd sample_normal_R = normal_R.perform_mvnd().get_mvnd_matrix();

  // generate x and x with noise
  Eigen::MatrixXd x_noise(n, _TotalT);
  Eigen::MatrixXd u(l, _TotalT);
  Eigen::MatrixXd z(m, _TotalT);
  Eigen::MatrixXd PEigen(1, _TotalT);
  z.col(0) = Eigen::MatrixXd::Zero(m, 1);
  u.setZero();
  x_noise.col(0) = initx;
  for (int i = 0; i != (_TotalT - 1); ++i) {
    x_noise.col(i + 1) = A * x_noise.col(i) + B * u(i) + sample_normal_Q.col(i);
    z.col(i + 1) = H * x_noise.col(i + 1) + sample_normal_R.col(i + 1);
  }

  Eigen::MatrixXd save_x = Eigen::MatrixXd::Zero(n, _TotalT);
  save_x.col(0) = initx;
  /* Create The Filter */
  kalmanfilter<l, m, n> filter2(A, B, H, Q, R);
  filter2.setInitial(initx, initP);
  PEigen(0) = filter2.getMaxEigenP();

  for (int i = 0; i != (_TotalT - 1); ++i) {
    filter2.linearkalman(u.col(i), z.col(i + 1));
    save_x.col(i + 1) = filter2.getState();
    PEigen(0, i + 1) = filter2.getMaxEigenP();
  }

  x_noise = x_noise.transpose().eval();
  z = z.transpose().eval();
  save_x = save_x.transpose().eval();
  PEigen = PEigen.transpose().eval();

  write2csvfile("../data/truex.csv", x_noise);
  write2csvfile("../data/observed.csv", z);
  write2csvfile("../data/kalman.csv", save_x);
  write2csvfile("../data/EigenP.csv", PEigen);
}

void test3d() {
  // estimatordata _estimatordata{0.1};
  // estimatorRTdata _estimatorRTdata {
  //   Eigen::Matrix3d::Identity(),              // CTB2G
  //       Eigen::Matrix3d::Identity(),          // CTG2B
  //       Eigen::Matrix<double, 6, 1>::Zero(),  // measurement
  //       State;
  // };
}

int main() {
  test2d();
  return 0;
}
