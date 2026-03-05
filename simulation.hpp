#include "draw.hpp"
#include "geometry.hpp"

constexpr int dim = 2;
constexpr int playercount = 1;
constexpr int rays = 20;

constexpr double dt = 0.01;

using envmap = vector<shape>;
using data = pair<envmap, array<pair<point, point>, playercount>>;

ftype clip(ftype val, ftype lim) {
    if (abs(val) < lim)
        return val;
    else if (val < -lim)
        return -lim;
    else
        return lim;
}

const ftype acceldelta = 1e-4, steerdelta = 1e-2;

class agent {
    friend class simulationinstance;

    ftype aclip = 0.5, steerclip = PI / 6, vclip = 0.01;
    shape egogeometry = {rotate(point(0.02, 0), point(0, 0), 0),
                         rotate(point(0.02, 0), point(0, 0), 2 * PI / 3),
                         rotate(point(0.02, 0), point(0, 0), 4 * PI / 3)};
    ftype steer = 0, theta = 0, v = 0, acceleration = 0;
    point r = {0, 0};

    void update() {
        v += clip(acceleration, aclip), theta += clip(steer, steerclip),
            v = clip(v, vclip),
            egogeometry =
                rotate(egogeometry, point(0, 0), clip(steer, steerclip));
        r = r + v * point(cos(theta), sin(theta));
    }

    void draw() { drawpolygonboundary(r + egogeometry); }

   public:
    function<void(const envmap&, const array<pair<point, point>, playercount>&,
                  const array<point, rays>&, const agent&, ftype&, ftype&)>
        calculate_1 =
            [](const envmap& curmap,
               const array<pair<point, point>, playercount>& playerdata,
               const array<point, rays>& raycasts, const agent& curplayer,
               ftype& a, ftype& steer) {};

    function<void(const array<point, rays>&, const agent&, ftype&, ftype&)>
        calculate_2 = [](const array<point, rays>& raycasts,
                         const agent& curplayer, ftype& a, ftype& steer) {};
};

class simulationinstance {
    string debuglog;

    envmap mp;
    array<agent, playercount> players;
    array<pair<point, point>, playercount> pdata;

    bool collision = false;
    double endtime, currenttime;

    array<point, rays> raycastagent(const agent& a,
                                    const ftype& noisedeviation = 0) {
        default_random_engine gen;
        normal_distribution<ftype> nd(0, noisedeviation);

        array<point, rays> res;
        ftype theta;

        for (int i = 0; i < rays; i++) {
            theta = ur(rng);
            res[i] = raycast(a.r, mp, 2 * PI * i / rays + a.theta) +
                     nd(gen) * point(cos(2 * PI * theta), sin(2 * PI * theta));
        }

        return res;
    }

    void update() {
        for (int i = 0; i < playercount; i++) {
            players[i].calculate_1(mp, pdata, raycastagent(players[i]),
                                   players[i], players[i].acceleration,
                                   players[i].steer);
            pdata[i].first = players[i].r,
            pdata[i].second = point(players[i].v, players[i].theta);
        }
        for (int i = 0; i < playercount; i++) players[i].update();
        currenttime += dt;
    }

    void collidecheck() {
        for (int i = 0; i < playercount && !collision; i++)
            for (int j = i + 1; j < playercount && !collision; j++)
                if (intersect(players[i].egogeometry, players[j].egogeometry))
                    collision = true;

        for (int i = 0; i < playercount && !collision; i++)
            for (int j = 0; j < mp.size() && !collision; j++)
                if (intersect(players[i].egogeometry, mp[j])) {
                    collision = true;
                }
    }

   public:
    bool humanmode, visualmode, blind;
    int currentactive;

    simulationinstance(const array<agent, playercount>& playerlist,
                       const ftype& en) {
        mp = genobs(10, 0.1, 10);
        players = playerlist;
        endtime = en, currenttime = 0, collision = false, humanmode = true,
        visualmode = true, blind = false, currentactive = 0;
    }

    void run() {
        if (visualmode) {
            InitWindow(screenWidth, screenHeight, "I love Akshat Shukla");
            SetTargetFPS(60);
            while (!WindowShouldClose()) {
                BeginDrawing();
                ClearBackground(BLACK);

                if (humanmode) {
                    for (int i = KEY_ONE; i <= KEY_NINE; i++)
                        if (IsKeyPressed(i)) currentactive = i - KEY_ONE;
                    if (IsKeyDown(KEY_UP))
                        players[currentactive].acceleration = acceldelta;
                    if (IsKeyDown(KEY_DOWN))
                        players[currentactive].acceleration = -acceldelta;
                    if (IsKeyDown(KEY_LEFT))
                        players[currentactive].steer = steerdelta;
                    if (IsKeyDown(KEY_RIGHT))
                        players[currentactive].steer = -steerdelta;

                    if (IsKeyReleased(KEY_UP))
                        players[currentactive].acceleration = 0;
                    if (IsKeyReleased(KEY_DOWN))
                        players[currentactive].acceleration = 0;
                    if (IsKeyReleased(KEY_LEFT))
                        players[currentactive].steer = 0;
                    if (IsKeyReleased(KEY_RIGHT))
                        players[currentactive].steer = 0;

                    for (int i = 0; i < mp.size(); i++)
                        drawpolygonboundary(mp[i]);

                    for (int i = 0; i < playercount; i++)
                        if (i == currentactive) {
                            array<point, rays> currentagentcasts =
                                raycastagent(players[i]);

                            for (int j = 0; j < rays; j++)
                                drawline(players[i].r, currentagentcasts[j]),
                                    drawpoint(currentagentcasts[j], GREEN);
                        }

                    for (int i = 0; i < playercount; i++) players[i].draw();

                    raycastagent(players[0]);
                    update();

                } else {
                    for (int i = 0; i < mp.size(); i++)
                        drawpolygonboundary(mp[i]);

                    for (int i = 0; i < playercount; i++) players[i].draw();
                    update();
                }

                DrawFPS(10, 10);
                EndDrawing();
            }
            CloseWindow();

        } else {
            while (currenttime < endtime) update();
        }
    }

    ~simulationinstance() {}
};
