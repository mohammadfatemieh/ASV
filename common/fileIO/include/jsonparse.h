/*
***********************************************************************
* jsonparse.h:
* Parse JSON file for USV
* This header file can be read by C++ compilers
*
* by Hu.ZH(CrossOcean.ai)
***********************************************************************
*/

#ifndef JSONPARSE_H
#define JSONPARSE_H
#include <cmath>
#include <common/fileIO/include/json.hpp>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "common/fileIO/include/utilityio.h"
#include "common/property/include/vesseldata.h"
#include "modules/controller/include/controllerdata.h"
#include "modules/estimator/include/estimatordata.h"
#include "modules/perception/marine_radar/include/SpokeProcessingData.h"
#include "modules/planner/common/include/plannerdata.h"
#include "modules/planner/lanefollow/include/LatticePlannerdata.h"
/*
global coordinate (GLOBAL), which is an inertial reference frame;
body-fixed coordinate (BODY); whose origin located at the stern
body-fixed coordinate (BODY-G), whose origin located at the center of gravity
*/

namespace ASV::common {
template <int m, int n = 3>
class jsonparse {
 public:
  explicit jsonparse(const std::string& _jsonname) : jsonname(_jsonname) {
    readjson();
  }
  jsonparse() = delete;
  ~jsonparse() {}

  vessel getvessel() const noexcept { return vesseldata_input; }
  auto getcontrollerdata() const noexcept { return controllerdata_input; }
  auto getthrustallocationdata() const noexcept {
    return thrustallocationdata_input;
  }
  auto gettunneldata() const noexcept { return tunnelthrusterdata_input; }
  auto getazimuthdata() const noexcept { return azimuththrusterdata_input; }
  auto getmainrudderdata() const noexcept { return ruddermaindata_input; }
  auto gettwinfixeddata() const noexcept { return twinfixeddata_input; }
  auto getpiddata() const noexcept { return pidcontrollerdata_input; }
  auto getestimatordata() const noexcept { return estimatordata_input; }
  auto getsimulatordata() const noexcept { return simulator_sample_time; }
  auto getlatticedata() const noexcept { return latticedata_input; }
  auto getcollisiondata() const noexcept { return collisiondata_input; }
  auto getSpokeProcessdata() const noexcept { return SpokeProcess_data; }
  auto getalarmzonedata() const noexcept { return Alarm_Zone; }

  std::string getsqlitedata() const noexcept { return dbpath; }
  std::string getgpsport() const noexcept { return gps_port; }
  std::string getguiport() const noexcept { return gui_port; }
  std::string getremotecontrolport() const noexcept { return rc_port; }
  std::string getwindport() const noexcept { return wind_port; }
  std::string getstm32port() const noexcept { return stm32_port; }

  unsigned long getgpsbaudrate() const noexcept { return gps_baudrate; }
  unsigned long getguibaudrate() const noexcept { return gui_baudrate; }
  unsigned long getrcbaudrate() const noexcept { return rc_baudrate; }
  unsigned long getwindbaudrate() const noexcept { return wind_baudrate; }
  unsigned long getstm32baudrate() const noexcept { return stm32_baudrate; }

  void readjson() {
    parsejson();
    parsevesselpropertydata();
    parsesimulatordata();
    parsecontrollerdata();
    parseestimatordata();
    parsesqlitedata();
    paresecomcenter();
    parsefrenetdata();
    parseSpokedata();
  }

 private:
  std::string jsonname;
  // json file = json::parse(in);
  nlohmann::json file;

  std::string dbpath;  // directory for database file

  unsigned long gps_baudrate = 9600;
  std::string gps_port;
  unsigned long gui_baudrate = 9600;
  std::string gui_port;
  unsigned long rc_baudrate = 9600;
  std::string rc_port;
  unsigned long wind_baudrate = 9600;
  std::string wind_port;
  unsigned long stm32_baudrate = 9600;
  std::string stm32_port;

  // vessel property
  vessel vesseldata_input{
      Eigen::Matrix3d::Zero(),  // Mass
      Eigen::Matrix3d::Zero(),  // AddedMass
      Eigen::Matrix3d::Zero(),  // LinearDamping
      Eigen::Matrix3d::Zero(),  // QuadraticDamping
      Eigen::Vector3d::Zero(),  // cog
      Eigen::Vector2d::Zero(),  // x_thrust
      Eigen::Vector2d::Zero(),  // y_thrust
      Eigen::Vector2d::Zero(),  // mz_thrust
      Eigen::Vector2d::Zero(),  // surge_v
      Eigen::Vector2d::Zero(),  // sway_v
      Eigen::Vector2d::Zero(),  // yaw_v
      Eigen::Vector2d::Zero(),  // roll_v
      0,                        // L
      0                         // B
  };

  // controllerdata
  control::controllerdata controllerdata_input{
      0.1,                               // sample_time
      0,                                 // los_radius
      0,                                 // los_capture_radius
      control::CONTROLMODE::MANUAL,      // controlmode
      control::ACTUATION::FULLYACTUATED  // index_actuation
  };

  // simulator data
  double simulator_sample_time = 0.1;
  // thrustallocationdata
  control::thrustallocationdata thrustallocationdata_input{
      0,  // num_tunnel
      0,  // num_azimuth
      0,  // num_mainrudder
      0,  // num_twinfixed
      {}  // index_thrusters
  };
  std::vector<control::tunnelthrusterdata> tunnelthrusterdata_input;
  std::vector<control::azimuththrusterdata> azimuththrusterdata_input;
  std::vector<control::ruddermaindata> ruddermaindata_input;
  std::vector<control::twinfixedthrusterdata> twinfixeddata_input;
  std::vector<control::pidcontrollerdata> pidcontrollerdata_input;

  // estimatordata
  estimatordata estimatordata_input{
      0.1,                                      // sample_time
      Eigen::Vector3d::Zero(),                  // antenna2cog
      Eigen::Matrix<double, 6, 6>::Identity(),  // Q
      Eigen::Matrix<double, 6, 6>::Identity()   // R
  };

  planning::LatticeData latticedata_input{
      0.1,         // SAMPLE_TIME
      50.0 / 3.6,  // MAX_SPEED
      0.05,        // TARGET_COURSE_ARC_STEP
      7.0,         // MAX_ROAD_WIDTH
      1,           // ROAD_WIDTH_STEP
      7.0,         // MAXT
      6.0,         // MINT
      0.5,         // DT
      0.5,         // MAX_SPEED_DEVIATION
      0.1          // TRAGET_SPEED_STEP
  };

  planning::CollisionData collisiondata_input{
      50.0 / 3.6,  // MAX_SPEED
      4.0,         // MAX_ACCEL
      -3.0,        // MIN_ACCEL
      1.0,         // MAX_CURVATURE
      3,           // HULL_LENGTH
      1,           // HULL_WIDTH
      2.5          // ROBOT_RADIUS
  };

  perception::SpokeProcessdata SpokeProcess_data{
      0.1,   // sampletime
      -1.0,  // radar_x
      0.0    // radar_y
  };

  perception::AlarmZone Alarm_Zone{
      10,        // start_range_m
      20,        // end_range_m
      0,         // center_bearing_rad
      M_PI / 2,  // width_bearing_rad
      0xac       // sensitivity_threhold
  };

  void parsejson() {
    // read a JSON file
    std::ifstream in(jsonname);
    in >> file;
  }  // parsejson

  void parsecontrollerdata() {
    // controller
    controllerdata_input.sample_time =
        file["controller"]["sample_time"].get<double>();

    controllerdata_input.los_radius =
        vesseldata_input.L *
        file["controller"]["LOS"]["los_radius_co"].get<double>();
    controllerdata_input.los_capture_radius =
        vesseldata_input.L *
        file["controller"]["LOS"]["capture_radius_co"].get<double>();

    control::pidcontrollerdata _pidcontrollerdata_input;
    // surge-- controller
    _pidcontrollerdata_input.position_P =
        file["controller"]["surge"]["position_P"].get<double>();
    _pidcontrollerdata_input.position_I =
        file["controller"]["surge"]["position_I"].get<double>();
    _pidcontrollerdata_input.velocity_P =
        file["controller"]["surge"]["velocity_P"].get<double>();
    _pidcontrollerdata_input.velocity_I =
        file["controller"]["surge"]["velocity_I"].get<double>();

    _pidcontrollerdata_input.position_allowed_error =
        file["controller"]["surge"]["position_allowed_error"].get<double>();
    _pidcontrollerdata_input.velocity_allowed_error =
        file["controller"]["surge"]["velocity_allowed_error"].get<double>();
    _pidcontrollerdata_input.min_output =
        file["controller"]["surge"]["min_output"].get<double>();
    _pidcontrollerdata_input.max_output =
        file["controller"]["surge"]["max_output"].get<double>();
    pidcontrollerdata_input.push_back(_pidcontrollerdata_input);

    // sway-- controller
    _pidcontrollerdata_input.position_P =
        file["controller"]["sway"]["position_P"].get<double>();
    _pidcontrollerdata_input.position_I =
        file["controller"]["sway"]["position_I"].get<double>();
    _pidcontrollerdata_input.velocity_P =
        file["controller"]["sway"]["velocity_P"].get<double>();
    _pidcontrollerdata_input.velocity_I =
        file["controller"]["sway"]["velocity_I"].get<double>();

    _pidcontrollerdata_input.position_allowed_error =
        file["controller"]["sway"]["position_allowed_error"].get<double>();
    _pidcontrollerdata_input.velocity_allowed_error =
        file["controller"]["sway"]["velocity_allowed_error"].get<double>();

    _pidcontrollerdata_input.min_output =
        file["controller"]["sway"]["min_output"].get<double>();
    _pidcontrollerdata_input.max_output =
        file["controller"]["sway"]["max_output"].get<double>();
    pidcontrollerdata_input.push_back(_pidcontrollerdata_input);

    // yaw-- controller
    _pidcontrollerdata_input.position_P =
        file["controller"]["yaw"]["position_P"].get<double>();
    _pidcontrollerdata_input.position_I =
        file["controller"]["yaw"]["position_I"].get<double>();
    _pidcontrollerdata_input.velocity_P =
        file["controller"]["yaw"]["velocity_P"].get<double>();
    _pidcontrollerdata_input.velocity_I =
        file["controller"]["yaw"]["velocity_I"].get<double>();

    _pidcontrollerdata_input.position_allowed_error =
        file["controller"]["yaw"]["position_allowed_error"].get<double>();
    _pidcontrollerdata_input.velocity_allowed_error =
        file["controller"]["yaw"]["velocity_allowed_error"].get<double>();

    _pidcontrollerdata_input.min_output =
        file["controller"]["yaw"]["min_output"].get<double>();
    _pidcontrollerdata_input.max_output =
        file["controller"]["yaw"]["max_output"].get<double>();
    pidcontrollerdata_input.push_back(_pidcontrollerdata_input);
    // thrusters
    for (int i = 0; i != m; ++i) {
      std::string str_thruster("thruster");
      str_thruster += std::to_string(i + 1);
      std::string str_type = file[str_thruster]["type"];
      if (str_type == "tunnel") {
        // update # of tunnels and index_thruster
        ++thrustallocationdata_input.num_tunnel;
        thrustallocationdata_input.index_thrusters.push_back(1);

        control::tunnelthrusterdata _thrusterdata_input;

        // position
        std::vector<double> _position = file[str_thruster]["position"];
        _thrusterdata_input.lx = _position[0] - vesseldata_input.cog(0);
        _thrusterdata_input.ly = _position[1] - vesseldata_input.cog(1);
        // thrust_constant
        std::vector<double> _thrustconstant =
            file[str_thruster]["thrust_constant"];
        _thrusterdata_input.K_positive = _thrustconstant[0];
        _thrusterdata_input.K_negative = _thrustconstant[1];

        // rotation
        _thrusterdata_input.max_delta_rotation = static_cast<int>(
            std::round(controllerdata_input.sample_time *
                       file[str_thruster]["max_delta_rotation"].get<double>()));
        _thrusterdata_input.max_rotation =
            file[str_thruster]["max_rotation"].get<int>();
        // thrust
        _thrusterdata_input.max_thrust_positive =
            _thrusterdata_input.K_positive *
            std::pow(_thrusterdata_input.max_rotation, 2);
        _thrusterdata_input.max_thrust_negative =
            _thrusterdata_input.K_negative *
            std::pow(_thrusterdata_input.max_rotation, 2);

        //
        tunnelthrusterdata_input.push_back(_thrusterdata_input);
      } else if (str_type == "azimuth") {
        // update # of azimuth and index_thruster
        ++thrustallocationdata_input.num_azimuth;
        thrustallocationdata_input.index_thrusters.push_back(2);

        control::azimuththrusterdata _thrusterdata_input;

        // position
        std::vector<double> _position = file[str_thruster]["position"];
        _thrusterdata_input.lx = _position[0] - vesseldata_input.cog(0);
        _thrusterdata_input.ly = _position[1] - vesseldata_input.cog(1);
        // thrust_constant
        _thrusterdata_input.K =
            file[str_thruster]["thrust_constant"].get<double>();

        // rotation
        _thrusterdata_input.max_delta_rotation = static_cast<int>(
            std::round(controllerdata_input.sample_time *
                       file[str_thruster]["max_delta_rotation"].get<double>()));
        _thrusterdata_input.max_rotation =
            file[str_thruster]["max_rotation"].get<int>();
        _thrusterdata_input.min_rotation =
            file[str_thruster]["min_rotation"].get<int>();

        // alpha
        double deg2rad = M_PI / 180.0;
        _thrusterdata_input.max_delta_alpha =
            controllerdata_input.sample_time * deg2rad *
            file[str_thruster]["max_delta_alpha"].get<double>();

        _thrusterdata_input.max_alpha =
            deg2rad * file[str_thruster]["max_alpha"].get<double>();
        _thrusterdata_input.min_alpha =
            deg2rad * file[str_thruster]["min_alpha"].get<double>();
        // thrust
        _thrusterdata_input.max_thrust =
            _thrusterdata_input.K *
            std::pow(_thrusterdata_input.max_rotation, 2);
        _thrusterdata_input.min_thrust =
            _thrusterdata_input.K *
            std::pow(_thrusterdata_input.min_rotation, 2);

        //
        azimuththrusterdata_input.push_back(_thrusterdata_input);
      } else if (str_type == "rudder") {
        ++thrustallocationdata_input.num_mainrudder;
        thrustallocationdata_input.index_thrusters.push_back(3);

        control::ruddermaindata _thrusterdata_input;

        // position
        std::vector<double> _position = file[str_thruster]["position"];
        _thrusterdata_input.lx = _position[0] - vesseldata_input.cog(0);
        _thrusterdata_input.ly = _position[1] - vesseldata_input.cog(1);
        // thrust_constant
        _thrusterdata_input.K =
            file[str_thruster]["thrust_constant"].get<double>();
        _thrusterdata_input.Cy =
            file[str_thruster]["rudder_constant"].get<double>();
        // rotation
        _thrusterdata_input.max_delta_rotation = static_cast<int>(
            std::round(controllerdata_input.sample_time *
                       file[str_thruster]["max_delta_rotation"].get<double>()));
        _thrusterdata_input.max_rotation =
            file[str_thruster]["max_rotation"].get<int>();
        _thrusterdata_input.min_rotation =
            file[str_thruster]["min_rotation"].get<int>();

        // varphi
        _thrusterdata_input.max_delta_varphi = static_cast<int>(
            std::round(controllerdata_input.sample_time *
                       file[str_thruster]["max_delta_varphi"].get<double>()));

        _thrusterdata_input.max_varphi =
            file[str_thruster]["max_varphi"].get<int>();
        _thrusterdata_input.min_varphi =
            file[str_thruster]["min_varphi"].get<int>();
        // thrust
        _thrusterdata_input.max_thrust =
            _thrusterdata_input.K *
            std::pow(_thrusterdata_input.max_rotation, 2);
        _thrusterdata_input.min_thrust =
            _thrusterdata_input.K *
            std::pow(_thrusterdata_input.min_rotation, 2);

        //
        ruddermaindata_input.push_back(_thrusterdata_input);

      } else if (str_type == "twinfixed") {
        ++thrustallocationdata_input.num_twinfixed;
        thrustallocationdata_input.index_thrusters.push_back(4);

        control::twinfixedthrusterdata _thrusterdata_input;

        // position
        std::vector<double> _position = file[str_thruster]["position"];
        _thrusterdata_input.lx = _position[0] - vesseldata_input.cog(0);
        _thrusterdata_input.ly = _position[1] - vesseldata_input.cog(1);
        // thrust_constant
        std::vector<double> _thrustconstant =
            file[str_thruster]["thrust_constant"];
        _thrusterdata_input.K_positive = _thrustconstant[0];
        _thrusterdata_input.K_negative = _thrustconstant[1];

        // rotation
        _thrusterdata_input.max_delta_rotation = static_cast<int>(
            std::round(controllerdata_input.sample_time *
                       file[str_thruster]["max_delta_rotation"].get<double>()));
        // delay time of positive->negative (vesra)
        double pndelaytime = file[str_thruster]["p2n_delay_time"].get<double>();
        _thrusterdata_input.max_delta_rotation_p2n = static_cast<int>(
            std::ceil(2 * controllerdata_input.sample_time *
                      _thrusterdata_input.max_delta_rotation / pndelaytime));
        _thrusterdata_input.max_rotation =
            file[str_thruster]["max_rotation"].get<int>();
        // thrust
        _thrusterdata_input.max_thrust_positive =
            _thrusterdata_input.K_positive *
            std::pow(_thrusterdata_input.max_rotation, 2);
        _thrusterdata_input.max_thrust_negative =
            _thrusterdata_input.K_negative *
            std::pow(_thrusterdata_input.max_rotation, 2);

        //
        twinfixeddata_input.push_back(_thrusterdata_input);

      } else {
        std::cout << "unknow thruster type!\n";
      }
    }

  }  // parsecontrollerdata()

  void parseestimatordata() {
    estimatordata_input.sample_time =
        file["estimator"]["sample_time"].get<double>();

    estimatordata_input.antenna2cog =
        vesseldata_input.cog - convertstdvector2EigenMat<double, 3, 1>(
                                   file["sensors"]["GPS"]["primary_antenna"]
                                       .get<std::vector<double>>());

    estimatordata_input.Q = convertstdvector2EigenMat<double, 6, 6>(
        file["estimator"]["Kalman"]["Q"].get<std::vector<double>>());
    estimatordata_input.R = convertstdvector2EigenMat<double, 6, 6>(
        file["estimator"]["Kalman"]["R"].get<std::vector<double>>());
  }  // parseestimatordata

  void parsesimulatordata() {
    simulator_sample_time = file["simulator"]["sample_time"].get<double>();
  }  // parsesimulatordata
  void parsevesselpropertydata() {
    vesseldata_input.Mass = convertstdvector2EigenMat<double, 3, 3>(
        file["property"]["Mass"].get<std::vector<double>>());
    vesseldata_input.AddedMass = convertstdvector2EigenMat<double, 3, 3>(
        file["property"]["AddedMass"].get<std::vector<double>>());
    vesseldata_input.LinearDamping = convertstdvector2EigenMat<double, 3, 3>(
        file["property"]["LinearDamping"].get<std::vector<double>>());
    vesseldata_input.QuadraticDamping = convertstdvector2EigenMat<double, 3, 3>(
        file["property"]["QuadraticDamping"].get<std::vector<double>>());
    vesseldata_input.cog = convertstdvector2EigenMat<double, 3, 1>(
        file["property"]["CoG"].get<std::vector<double>>());

    vesseldata_input.x_thrust
        << file["controller"]["surge"]["min_output"].get<double>(),
        file["controller"]["surge"]["max_output"].get<double>();
    vesseldata_input.y_thrust
        << file["controller"]["sway"]["min_output"].get<double>(),
        file["controller"]["sway"]["max_output"].get<double>();
    vesseldata_input.mz_thrust
        << file["controller"]["yaw"]["min_output"].get<double>(),
        file["controller"]["yaw"]["max_output"].get<double>();

    vesseldata_input.surge_v = convertstdvector2EigenMat<double, 2, 1>(
        file["property"]["velocity_limit"]["surge"].get<std::vector<double>>());
    vesseldata_input.sway_v = convertstdvector2EigenMat<double, 2, 1>(
        file["property"]["velocity_limit"]["sway"].get<std::vector<double>>());
    vesseldata_input.yaw_v = convertstdvector2EigenMat<double, 2, 1>(
        file["property"]["velocity_limit"]["yaw"].get<std::vector<double>>());
    vesseldata_input.roll_v = convertstdvector2EigenMat<double, 2, 1>(
        file["property"]["velocity_limit"]["roll"].get<std::vector<double>>());

    vesseldata_input.L = file["property"]["L"].get<double>();
    vesseldata_input.B = file["property"]["B"].get<double>();

  }  // parsevesselpropertydata

  void parsesqlitedata() {
    std::time_t result = std::time(nullptr);
    std::string utctime = std::asctime(std::localtime(&result));
    utctime.pop_back();
    dbpath = file["project_directory"].get<std::string>() +
             file["dbpath"].get<std::string>() + utctime + ".db";

  }  // parsesqlitedata

  void paresecomcenter() {
    gps_port = file["comcenter"]["GPS"]["port"];
    gps_baudrate = file["comcenter"]["GPS"]["baudrate"].get<unsigned long>();
    gui_port = file["comcenter"]["GUI_server"]["port"];
    gui_baudrate =
        file["comcenter"]["GUI_server"]["baudrate"].get<unsigned long>();
    rc_port = file["comcenter"]["remotecontrol"]["port"];
    rc_baudrate =
        file["comcenter"]["remotecontrol"]["baudrate"].get<unsigned long>();
    wind_port = file["comcenter"]["Wind"]["port"];
    wind_baudrate = file["comcenter"]["Wind"]["baudrate"].get<unsigned long>();
    stm32_port = file["comcenter"]["stm32"]["port"];
    stm32_baudrate =
        file["comcenter"]["stm32"]["baudrate"].get<unsigned long>();
  }  // paresecomcenter

  void parsefrenetdata() {
    std::vector<double> speed_limit =
        file["property"]["velocity_limit"]["surge"].get<std::vector<double>>();
    // Frenet Lattice Generator
    latticedata_input.SAMPLE_TIME =
        file["planner"]["sample_time"].get<double>();
    latticedata_input.MAX_SPEED = vesseldata_input.surge_v(1);
    latticedata_input.TARGET_COURSE_ARC_STEP =
        file["planner"]["FrenetLattice"]["target_course_arc_step"]
            .get<double>();
    latticedata_input.MAX_ROAD_WIDTH =
        file["planner"]["FrenetLattice"]["max_road_width"].get<double>();
    latticedata_input.ROAD_WIDTH_STEP =
        file["planner"]["FrenetLattice"]["road_width_step"].get<double>();
    latticedata_input.MAXT =
        file["planner"]["FrenetLattice"]["max_planning_horizon"].get<double>();
    latticedata_input.MINT =
        file["planner"]["FrenetLattice"]["min_planning_horizon"].get<double>();
    latticedata_input.DT =
        file["planner"]["FrenetLattice"]["planning_horizon_step"].get<double>();
    latticedata_input.MAX_SPEED_DEVIATION =
        file["planner"]["FrenetLattice"]["max_speed_deviation"].get<double>();
    latticedata_input.TRAGET_SPEED_STEP =
        file["planner"]["FrenetLattice"]["target_speed_step"].get<double>();
    // constatnt data for collision and constraint check
    collisiondata_input.MAX_SPEED = vesseldata_input.surge_v(1);
    collisiondata_input.MAX_ACCEL =
        1.5 * vesseldata_input.x_thrust[1] / vesseldata_input.Mass(0, 0);
    collisiondata_input.MIN_ACCEL =
        1.5 * vesseldata_input.x_thrust[0] / vesseldata_input.Mass(0, 0);
    collisiondata_input.MAX_CURVATURE =
        1 / file["property"]["mini_turn_radius"].get<double>();

    collisiondata_input.HULL_LENGTH = vesseldata_input.L;
    collisiondata_input.HULL_WIDTH = vesseldata_input.B;
    collisiondata_input.ROBOT_RADIUS =
        file["planner"]["FrenetCollision"]["robot_radius"].get<double>();
  }  // parsefrenetdata

  void parseSpokedata() {
    std::vector<double> radar_atennna =
        file["sensors"]["marine_radar"]["antenna"].get<std::vector<double>>();

    SpokeProcess_data.radar_x = radar_atennna[0];
    SpokeProcess_data.radar_y = radar_atennna[1];

    SpokeProcess_data.sample_time =
        file["perception"]["sample_time"].get<double>();
    // AlarmZone
    Alarm_Zone.start_range_m =
        file["perception"]["alarm_zone"]["start_range_m"].get<double>();
    Alarm_Zone.end_range_m =
        file["perception"]["alarm_zone"]["end_range_m"].get<double>();
    Alarm_Zone.center_bearing_rad =
        file["perception"]["alarm_zone"]["center_bearing_deg"].get<double>() *
        M_PI / 180.0;
    Alarm_Zone.width_bearing_rad =
        file["perception"]["alarm_zone"]["width_bearing_deg"].get<double>() *
        M_PI / 180.0;
    Alarm_Zone.sensitivity_threhold =
        file["perception"]["alarm_zone"]["sensitivity_threhold"].get<uint8_t>();

  }  // parseSpokedata

 public:
  template <int _m, int _n>
  friend std::ostream& operator<<(std::ostream&, const jsonparse<_m, _n>&);
};  // end class jsonparse

template <int _m, int _n>
std::ostream& operator<<(std::ostream& os, const jsonparse<_m, _n>& _jp) {
  os << "index of each thruster:\n";
  for (int i = 0; i != _m; ++i)
    os << _jp.thrustallocationdata_input.index_thrusters[i] << " ";

  os << "info of each tunnel thruster:\n";
  for (unsigned int i = 0; i != _jp.tunnelthrusterdata_input.size(); ++i) {
    os << _jp.tunnelthrusterdata_input[i].lx << std::endl;
    os << _jp.tunnelthrusterdata_input[i].ly << std::endl;
    os << _jp.tunnelthrusterdata_input[i].K_positive << std::endl;
    os << _jp.tunnelthrusterdata_input[i].K_negative << std::endl;
    os << _jp.tunnelthrusterdata_input[i].max_delta_rotation << std::endl;
    os << _jp.tunnelthrusterdata_input[i].max_rotation << std::endl;
    os << _jp.tunnelthrusterdata_input[i].max_thrust_positive << std::endl;
    os << _jp.tunnelthrusterdata_input[i].max_thrust_negative << std::endl;
  }

  os << "info of each azimuth thruster:\n";
  for (unsigned int i = 0; i != _jp.azimuththrusterdata_input.size(); ++i) {
    os << _jp.azimuththrusterdata_input[i].lx << std::endl;
    os << _jp.azimuththrusterdata_input[i].ly << std::endl;
    os << _jp.azimuththrusterdata_input[i].K << std::endl;
    os << _jp.azimuththrusterdata_input[i].max_delta_rotation << std::endl;
    os << _jp.azimuththrusterdata_input[i].max_rotation << std::endl;
    os << _jp.azimuththrusterdata_input[i].min_rotation << std::endl;
    os << _jp.azimuththrusterdata_input[i].max_delta_alpha << std::endl;
    os << _jp.azimuththrusterdata_input[i].max_alpha << std::endl;
    os << _jp.azimuththrusterdata_input[i].min_alpha << std::endl;
    os << _jp.azimuththrusterdata_input[i].max_thrust << std::endl;
    os << _jp.azimuththrusterdata_input[i].min_thrust << std::endl;
  }

  os << "info of each main thruster with rudder:\n";
  for (unsigned int i = 0; i != _jp.ruddermaindata_input.size(); ++i) {
    os << _jp.ruddermaindata_input[i].lx << std::endl;
    os << _jp.ruddermaindata_input[i].ly << std::endl;
    os << _jp.ruddermaindata_input[i].K << std::endl;
    os << _jp.ruddermaindata_input[i].Cy << std::endl;
    os << _jp.ruddermaindata_input[i].max_delta_rotation << std::endl;
    os << _jp.ruddermaindata_input[i].max_rotation << std::endl;
    os << _jp.ruddermaindata_input[i].min_rotation << std::endl;
    os << _jp.ruddermaindata_input[i].max_delta_varphi << std::endl;
    os << _jp.ruddermaindata_input[i].max_varphi << std::endl;
    os << _jp.ruddermaindata_input[i].min_varphi << std::endl;
  }

  os << "info of each twin fixed thruster:\n";
  for (unsigned int i = 0; i != _jp.twinfixeddata_input.size(); ++i) {
    os << _jp.twinfixeddata_input[i].lx << std::endl;
    os << _jp.twinfixeddata_input[i].ly << std::endl;
    os << _jp.twinfixeddata_input[i].K_positive << std::endl;
    os << _jp.twinfixeddata_input[i].K_negative << std::endl;
    os << _jp.twinfixeddata_input[i].max_delta_rotation << std::endl;
    os << _jp.twinfixeddata_input[i].max_delta_rotation_p2n << std::endl;
    os << _jp.twinfixeddata_input[i].max_rotation << std::endl;
    os << _jp.twinfixeddata_input[i].max_thrust_positive << std::endl;
    os << _jp.twinfixeddata_input[i].max_thrust_negative << std::endl;
  }

  os << "controller:\n";
  os << _jp.controllerdata_input.sample_time << std::endl;
  os << _jp.controllerdata_input.los_radius << std::endl;
  os << _jp.controllerdata_input.los_capture_radius << std::endl;
  os << "pid controller:\n";
  for (unsigned int i = 0; i != _jp.pidcontrollerdata_input.size(); ++i) {
    os << _jp.pidcontrollerdata_input[i].position_P << std::endl;
    os << _jp.pidcontrollerdata_input[i].position_I << std::endl;
    os << _jp.pidcontrollerdata_input[i].velocity_P << std::endl;
    os << _jp.pidcontrollerdata_input[i].velocity_I << std::endl;
    os << _jp.pidcontrollerdata_input[i].position_allowed_error << std::endl;
    os << _jp.pidcontrollerdata_input[i].velocity_allowed_error << std::endl;
    os << _jp.pidcontrollerdata_input[i].min_output << std::endl;
    os << _jp.pidcontrollerdata_input[i].max_output << std::endl;
  }
  os << "estimator:\n";
  os << _jp.estimatordata_input.sample_time << std::endl;
  os << _jp.estimatordata_input.Q << std::endl;
  os << _jp.estimatordata_input.R << std::endl;

  os << "Mass property:\n";
  os << _jp.vesseldata_input.Mass << std::endl;
  os << _jp.vesseldata_input.AddedMass << std::endl;
  os << _jp.vesseldata_input.LinearDamping << std::endl;
  os << _jp.vesseldata_input.QuadraticDamping << std::endl;
  os << _jp.vesseldata_input.cog << std::endl;
  os << _jp.vesseldata_input.x_thrust << std::endl;
  os << _jp.vesseldata_input.y_thrust << std::endl;
  os << _jp.vesseldata_input.mz_thrust << std::endl;
  os << _jp.vesseldata_input.surge_v << std::endl;
  os << _jp.vesseldata_input.sway_v << std::endl;
  os << _jp.vesseldata_input.yaw_v << std::endl;
  os << _jp.vesseldata_input.roll_v << std::endl;

  os << "comcenter:\n";
  os << _jp.gps_port << " " << _jp.gps_baudrate << std::endl;
  os << _jp.gui_port << " " << _jp.gui_baudrate << std::endl;
  os << _jp.rc_port << " " << _jp.rc_baudrate << std::endl;
  os << _jp.wind_port << " " << _jp.wind_baudrate << std::endl;
  os << _jp.stm32_port << " " << _jp.stm32_baudrate << std::endl;

  os << "alarm:\n";
  os << _jp.Alarm_Zone.start_range_m << std::endl;
  os << _jp.Alarm_Zone.end_range_m << std::endl;
  os << _jp.Alarm_Zone.center_bearing_rad << std::endl;
  os << _jp.Alarm_Zone.width_bearing_rad << std::endl;
  os << (unsigned)_jp.Alarm_Zone.sensitivity_threhold << std::endl;

  os << "dbpath:\n";
  os << _jp.dbpath << std::endl;

  os << "Frenet:\n";
  os << _jp.latticedata_input.SAMPLE_TIME << std::endl;
  os << _jp.latticedata_input.MAX_SPEED << std::endl;
  os << _jp.latticedata_input.TARGET_COURSE_ARC_STEP << std::endl;
  os << _jp.latticedata_input.MAX_ROAD_WIDTH << std::endl;
  os << _jp.latticedata_input.ROAD_WIDTH_STEP << std::endl;
  os << _jp.latticedata_input.MAXT << std::endl;
  os << _jp.latticedata_input.MINT << std::endl;
  os << _jp.latticedata_input.DT << std::endl;
  os << _jp.latticedata_input.MAX_SPEED_DEVIATION << std::endl;
  os << _jp.latticedata_input.TRAGET_SPEED_STEP << std::endl;

  os << _jp.collisiondata_input.MAX_ACCEL << std::endl;
  os << _jp.collisiondata_input.MIN_ACCEL << std::endl;
  os << _jp.collisiondata_input.MAX_CURVATURE << std::endl;
  os << _jp.collisiondata_input.HULL_LENGTH << std::endl;
  os << _jp.collisiondata_input.HULL_WIDTH << std::endl;
  os << _jp.collisiondata_input.ROBOT_RADIUS << std::endl;
  return os;
}  // friend operator<<
}  // namespace ASV::common

#endif /* JSONPARSE_H */
