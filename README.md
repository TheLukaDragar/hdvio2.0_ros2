# HDVIO2.0: Wind and Disturbance Estimation with Hybrid Dynamics VIO

[![HDVIO2.0: Wind and Disturbance Estimation with Hybrid Dynamics VIO](img/eyecatcher_with_youtube_logo.jpg)](https://www.youtube.com/watch?v=wUaEp0YGpDM)

This repo contains a sliding-window optimization-based odometry system fusing visual, inertial and hybrid quadrotor dynamics obtained by combining a point-mass vehicle model with a learning-based component, with access to control commands and IMU history, to capture complex aerodynamic effects. 
It builds on top of the visual-inertial odometry algorithm [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open). The B-spline implementation is based on this [work](https://openaccess.thecvf.com/content_CVPR_2020/papers/Sommer_Efficient_Derivative_Computation_for_Cumulative_B-Splines_on_Lie_Groups_CVPR_2020_paper.pdf).

## Publication
If you use this code in an academic context, please cite the following [T-RO 2025 paper](https://rpg.ifi.uzh.ch/docs/tro25_Cioffi.pdf).

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
mkdir hdvio2_ws && cd hdvio2_ws
catkin config --init --mkdirs --extend /opt/ros/<ROS-DISTRO> --cmake-args -DCMAKE_BUILD_TYPE=Release
cd src
git clone git@github.com:uzh-rpg/hdvio2.0.git
vcs-import < ./hdvio2.0/dependencies.yaml
touch minkindr/minkindr_python/CATKIN_IGNORE
catkin build
```

### Trobleshooting

Refer to [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open#troubleshooting).

## How to run the code

Download the rosbag and the network weights from [here](https://drive.google.com/drive/folders/1zK88WnSwcYOD7A29tu4WDksy6b0HMIC-?usp=sharing)

Terminal 1:

```sh
roslaunch hdvio2 hdvio2.launch
```

Terminal 2:

```sh
rosbag play flyingroom_flight.bag
```

## Credits

The VIO system used in this repo is based on [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open). Check [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open) for the full list of aknowledgments.

## License

The code is licensed under GPLv3. For commercial use, please contact the authors at cioffi@ifi.uzh.ch and sdavide@ifi.uzh.ch.

Check [SVO Pro](https://github.com/uzh-rpg/rpg_svo_pro_open) for licenses of the external dependencies.
