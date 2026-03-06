<img align="left" style="width:40%;margin:10px" src="./visuals/raycast.png">

**A simple, user-friendly and lightweight simulator.**

This library provides a visual interface and an API for programming, testing and evaluating autonomous planning, mapping and navigation algorithms, its only dependency is raylib which is used to create and manage the visuals. Support is given to simulate LiDAR sensors and add noise to them, create arbitrary 2D environments with dynamic/static obstacles and detect collisions between agents and obstacles or between themselves, it allows for simulations to be run visually or headlessly and provides a user controlled mode for debugging. Each simulation exists as a separate instance to allow for multiple simluations to be run parallely to train genetic or reinforcement learning based planners.


<div style="clear: both;"></div>

Installation
----------------------

raylib must be installed and added to PATH first, for instructions on how to do that refer to [raylib's README](https://github.com/raysan5/raylib). Now clone this repository and compile main.cpp while linking with OpenGL and raylib(depending on your OS this might be implicit)

```bash
brew install raylib
touch simulation; cd simulation
git clone https://github.com/Kaustubh-Kaka/AGV-Simulator.git
```

If you are using a Mac and installed raylib with homebrew, your compilation command should look something like this

```bash
clang++ main.cpp -O3 -o rendered -I /opt/homebrew/include -L /opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit
```

If you are using Linux or wsl you can install raylib directly from source into the cloned repository

```bash
touch simulation; cd simulation
git clone https://github.com/Kaustubh-Kaka/AGV-Simulator.git
cd AGV-Simulator
git clone https://github.com/raysan5/raylib.git
cd raylib; mkdir build; cd build
CC=gcc CXX=g++ cmake ..
cd ../src
make
cd ../..
g++ main.cpp -O3 -o rendered -Iraylib/src -Lraylib/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXi -lXinerama -lXcursor
./rendered
```

You first need to create the agents in the simulation

```c
#include "draw.hpp"
#include "geometry.hpp"
#include "simulation.hpp"

int main() {
    
    agent myagent;
    myagent.calculate =
        [&](const envmap& curmap,
            const array<pair<point, point>, playercount>& playerdata,
            const array<point, rays>& raycasts, const agent& curplayer,
            ftype& a, ftype& steer) {
            // write you planning logic here
        };

    return 0;
}
```

To actually run a simulation you should need to create a simulation instance, and specify a list of agents as an `std::array<agent, playercount>` and a simulation end time which will be used if you run the simulation in the non visual mode

```c
#include "draw.hpp"
#include "geometry.hpp"
#include "simulation.hpp"

int main() {
    // the planners can share data by writing to and reading from variables
    // defined here

    agent myagent;
    myagent.calculate_1 =
        [&](const envmap& curmap,
            const array<pair<point, point>, playercount>& playerdata,
            const array<point, rays>& raycasts, const agent& curplayer,
            ftype& a, ftype& steer) {
            // write you planning logic here
        };

    array<agent, playercount> myagents;
    for (int i = 0; i < playercount; i++) myagents[i] = myagent;

    ftype simultime = 30;

    simulationinstance s(myagents, simultime);
    s.run();

    return 0;
}
```

You also need to specify a map to run the simulation that you can choose to create yourself or use the given random map generator for, please note that the raycasting algorithm only handles convex obstacles and in general `shape = vector<point>` is assumed to be a convex polygon, if you wish to use nonconvex polygons please decompose them into convex polygons first. The random map generator guarantees non intersecting convex obstacles given an initial random seed.

For a more detailed and complete documentation of the library helper functions you can refer to the in code comments next to the forward declerations.

I will also post a rolling log of knows bugs, patches and backwards compatible API changes here, most changes will be algorithmic optimizations and won't change functionality in any way.

Please feel free to contact me for any bugs or suggestions on [discord @intricacies_](https://discordapp.com/users/1287657787668697091).