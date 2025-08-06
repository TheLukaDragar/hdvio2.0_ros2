# HDVIO2.0: Wind and Disturbance Estimation with Hybrid Dynamics VIO

THE VIDEO WILL BE HERE ONCE APPROAVED

<!-- [![HDVIO2.0: Wind and Disturbance Estimation with Hybrid Dynamics VIO](doc/eyecatcher_with_youtube_logo.jpg)](https://youtu.be/gmHnhWYfuW0) -->

This repo contains a sliding-window optimization-based odometry system fusing visual, inertial and hybrid quadrotor dynamics obtained by combining a point-mass vehicle model with a learning-based component, with access to control commands and IMU history, to capture complex aerodynamic effects. 
It builds on top of the visual-inertial odometry algorithm [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open).

## Publication
If you use this code in an academic context, please cite the following [T-RO 2025 paper](https://rpg.ifi.uzh.ch/docs/Arxiv25_Cioffi.pdf).

G. Cioffi, L. Bauersfeld, and D. Scaramuzza, "**HDVIO2.0: Wind and Disturbance Estimation with Hybrid Dynamics VIO**," IEEE Transactions on Robotics (T-RO) 2025.

```
@article{cioffi2025hdvio2,
  title={HDVIO2.0: Wind and Disturbance Estimation with Hybrid Dynamics VIO},
  author={Cioffi, Giovanni and Bauersfeld, Leonard and Scaramuzza, Davide},
  journal={IEEE Transactions on Robotics},
  year={2025},
  publisher={IEEE}
}
```

## Install

The code has been tested on

* Ubuntu 20.04 with ROS Noetic

### Install dependences

Install [catkin tools](https://catkin-tools.readthedocs.io/en/latest/installing.html) and [vcstools](https://github.com/dirk-thomas/vcstool) if you haven't done so before. Depending on your operating system, run

```sh
# For Ubuntu 20.04 + Noetic
sudo apt-get install python3-catkin-tools python3-vcstool python3-osrf-pycommon
```
Install system dependencies and dependencies for Ceres Solver
```sh
# system dep.
sudo apt-get install libglew-dev libopencv-dev libyaml-cpp-dev 
# Ceres dep.
sudo apt-get install libblas-dev liblapack-dev libsuitesparse-dev
```

### Clone and compile

Create a workspace and clone the code (`ROS-DISTRO`=`noetic`):
```sh
mkdir catkin_ws && cd catkin_ws
catkin config --init --mkdirs --extend /opt/ros/<ROS-DISTRO> --cmake-args -DCMAKE_BUILD_TYPE=Release
cd src
git clone git@github.com:uzh-rpg/hdvio2.0.git
vcs-import < ./rpg_svo_pro_with_digital_twins/dependencies.yaml
touch minkindr/minkindr_python/CATKIN_IGNORE
catkin build
```

### Trobleshooting

Refer to [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open#troubleshooting).

## Instructions

COMING SOON!

## Credits

The VIO system used in this repo is based on [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open). Check [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open) for the full list of aknowledgments.

## License

The code is licensed under GPLv3. For commercial use, please contact the authors at cioffi@ifi.uzh.ch and sdavide@ifi.uzh.ch.

Check [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open) for licenses of the external dependencies.
