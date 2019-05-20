OSM map data and GPS route import
============================
* I integrated a modified fork of [**StreetMapPlugin**](https://github.com/ue4plugins/StreetMap) to enable automatic map import and road mesh generation from OSM data.
* I implemented automatic route planner generation from GPS trajectories as **RouteGenerator**.
* The above two functionalities can be used together to easily simulate driving scenarios that are record from real-world traffic.
  
## Import OSM data and generate road meshes

1. Make sure you can clone git repos with ssh.
2. In terminal: `git clone -b master git@github.com:ruichen-v/carla.git --recursive`
3. Build carla using **UE4.21**. You will need to build from source.
4. Export OSM data for an area of interest from [**Open Street Map**](https://www.openstreetmap.org/#map=5/38.007/-95.844). A sample osm file is provided `Unreal/CarlaUE4/Plugins/Carla/Content/sample_files/CMUTrack.osm`.
5. In UE Editor, import the downloaded osm file into content folder.
6. Drag the imported osm file into main view. You will get a **StreetMapActor**.
7. In **Meshes** category in **StreetMapActor** property, assign a road mesh to each **StreetMapMeshTag** slot in **StaticMeshes**.
8. You can use `Content/Carla/Static/Road/RoadPiece.uasset` for all mesh tags for simplicity.
9. Each **StreetMapMeshTag** controls the appearance of a certain range of road types in osm data. Refer to `path/to/carla/Unreal/CarlaUE4/Plugins/StreetMap/Source/StreetMapRuntime/StreetMapComponent.cpp:262~298` for the correspondence between mesh tags and road types.


## Import GPS routes and generate route planners for navigation

1. In UE Editor, import your GPS routes in required format into content viewer, select **FGPSLocationDescriptor** in the drop down menu when prompted. A sample GPS route file is provided `Unreal/CarlaUE4/Plugins/StreetMap/Content/sample_files/buggytracegps.csv`. **PLEASE FORMAT YOUR GPS ROUTE FILE AFTER `buggytracegps.csv`**.
2. In `Carla C++ class/AutoGen/RouteGen/`, drag **RouteGenerator** into main view. Set its location to the same as **StreetMapActor**.
3. Copy **RelativeLatitude & RelativeLongitude** from **StreetMap** category in **StreetMapActor** property and paste into the **AverageLatitude & AverageLongitude** in **Projection** category in the **RouteGenerator** that you created. This aligns the projection origins of both roads and routes.
4. In **Traffic Routes** category in **RouteGenerator** property, click `+` in **GPS_routes**.
5. Assign the imported GPS route file to the newly created entry in **GPS_routes**
6. Click once in arbitrary area in editor main view to visualize the generated route planners.
7. Enlarge and place the **box trigger volume** (attached to **RouteGenerator**) at the start point of your route.
8. Vehicles entering the **box trigger volume** will follow the GPS routes.
9. Click `+` in **GPS_routes** to add as many routes as you like.

## You can create a map with road meshes from `CMUTrack.osm` and generate routes from `buggytracegps.csv` to simulate the [**Buggy Race in CMU Spring Carnival**](https://www.springcarnival.org/buggy.shtml).


CARLA Simulator
===============

[![Build Status](https://travis-ci.org/carla-simulator/carla.svg?branch=master)](https://travis-ci.org/carla-simulator/carla)
[![Documentation](https://readthedocs.org/projects/carla/badge/?version=latest)](http://carla.readthedocs.io)


[![carla.org](Docs/img/btn/web.png)](http://carla.org)
[![download](Docs/img/btn/download.png)](https://github.com/carla-simulator/carla/blob/master/Docs/download.md)
[![documentation](Docs/img/btn/docs.png)](http://carla.readthedocs.io)
[![discord](Docs/img/btn/chat.png)](https://discord.gg/8kqACuC)
<!-- [![forum](Docs/img/btn/forum.png)](link here) -->

CARLA is an open-source simulator for autonomous driving research. CARLA has
been developed from the ground up to support development, training, and
validation of autonomous driving systems. In addition to open-source code
and protocols, CARLA provides open digital assets (urban layouts, buildings,
vehicles) that were created for this purpose and can be used freely. The
simulation platform supports flexible specification of sensor suites and
environmental conditions.

[![CARLA Video](Docs/img/video_thumbnail.png)](https://www.youtube.com/watch?v=BjH-pFGlZ0M)

If you want to benchmark your model in the same conditions as in our CoRL’17
paper, check out
[Benchmarking](https://github.com/carla-simulator/driving-benchmarks).


## CARLA Ecosystem
Repositories associated to the CARLA simulation platform:

* [**Scenario_Runner**](https://github.com/carla-simulator/scenario_runner): Engine to execute traffic scenarios in CARLA 0.9.X
* [**ROS-bridge**](https://github.com/carla-simulator/ros-bridge): Interface to connect CARLA 0.9.X to ROS
* [**Driving-benchmarks**](https://github.com/carla-simulator/driving-benchmarks): Benchmark tools for Autonomous Driving tasks
* [**Conditional Imitation-Learning**](https://github.com/felipecode/coiltraine): Training and testing Conditional Imitation Learning models in CARLA [README](https://github.com/felipecode/coiltraine/blob/master/docs/view_agents.md)
* [**AutoWare AV stack**](https://github.com/carla-simulator/carla-autoware): Bridge to connect AutoWare AV stack to CARLA
* [**Reinforcement-Learning**](https://github.com/carla-simulator/reinforcement-learning): Code for running Conditional Reinforcement Learning models in CARLA

2019 Roadmap
-------
![carla.org](Docs/img/carla_timeline.png "CARLA Road Map")

We are continuously working on improving CARLA, and we appreciate contributions
from the community. Our most immediate goals are:

- [x] Support simulation of traffic scenarios 
- [x] Support ROS interface
- [x] Allowing for flexible and user-friendly import and editing of maps
- [x] Control of all vehicles from client side
- [x] Control of pedestrians from client side
- [x] No rendering mode for high performance simulation
- [ ] Support parallel simulation of traffic scenarios in the cloud
- [ ] RADAR simulation

Paper
-----

If you use CARLA, please cite our CoRL’17 paper.

_CARLA: An Open Urban Driving Simulator_<br>Alexey Dosovitskiy, German Ros,
Felipe Codevilla, Antonio Lopez, Vladlen Koltun; PMLR 78:1-16
[[PDF](http://proceedings.mlr.press/v78/dosovitskiy17a/dosovitskiy17a.pdf)]
[[talk](https://www.youtube.com/watch?v=xfyK03MEZ9Q&feature=youtu.be&t=2h44m30s)]


```
@inproceedings{Dosovitskiy17,
  title = {{CARLA}: {An} Open Urban Driving Simulator},
  author = {Alexey Dosovitskiy and German Ros and Felipe Codevilla and Antonio Lopez and Vladlen Koltun},
  booktitle = {Proceedings of the 1st Annual Conference on Robot Learning},
  pages = {1--16},
  year = {2017}
}
```

Building CARLA
--------------

Use `git clone` or download the project from this page. Note that the master
branch contains the latest fixes and features, for the latest stable code may be
best to switch to the `stable` branch.

Then follow the instruction at [How to build on Linux][buildlinuxlink] or
[How to build on Windows][buildwindowslink].

Unfortunately we don't have official instructions to build on Mac yet, please
check the progress at [issue #150][issue150].

[buildlinuxlink]: http://carla.readthedocs.io/en/latest/how_to_build_on_linux
[buildwindowslink]: http://carla.readthedocs.io/en/latest/how_to_build_on_windows
[issue150]: https://github.com/carla-simulator/carla/issues/150

Contributing
------------

Please take a look at our [Contribution guidelines][contriblink].

[contriblink]: http://carla.readthedocs.io/en/latest/CONTRIBUTING

F.A.Q.
------

If you run into problems, check our
[FAQ](http://carla.readthedocs.io/en/latest/faq/).

License
-------

CARLA specific code is distributed under MIT License.

CARLA specific assets are distributed under CC-BY License.

Note that UE4 itself follows its own license terms.
