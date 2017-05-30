# Quadrotor2D Simulator

[![Quadrotor 2D RL video](http://img.youtube.com/vi/acQJfkgeiZc/0.jpg)](https://www.youtube.com/watch?v=acQJfkgeiZc)

This is simulator decupled from https://github.com/parilo/quadrocopter-deep-q-learning-tensorflow project. Built in C++ using cocos2-x game engine. It communicates with reinforcement learning server via uWebSockets and json. You need to have [RL Server](https://github.com/parilo/rl-server) up and running to make this simulator works, unless simulator will be in freezed state.

# Building and running

```
git clone https://github.com/parilo/quadrotor2d-simulator.git sim
cd sim
mkdir build
cd build
cmake ..
make
./Quadrotor2DSimulator
```
