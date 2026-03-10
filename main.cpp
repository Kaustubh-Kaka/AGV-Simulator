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