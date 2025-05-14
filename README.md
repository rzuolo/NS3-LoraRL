> *Disclaimer*
This is - a work in progress - part of the experiments being carried at CEOT (University of Algarve).
There is no official public documentation about it yet. Should you need to get further information or more details, please, reach out to me rzcarvalho@ualg.pt.

### NS3-RLADR-Simulations Overview

This codebase consists of a set of automation scripts and a container image (ridiculously huge in size) already prepared to run **Python, C++, NS3, [LoRaWAN module](https://github.com/signetlabdei/lorawan "LoRaWAN module")**, and their supporting tools.
A compose file is used to fire up a container that reads from the main configuration files: **nodes.csv** and **arguments.yml**. Upon reading and parsing them accordingly, the main script calls the NS3 simulations.
The simulations entail 4 major loop executions:
- A LoRaWan RL-ADR agent training phase.
- A LoRaWan RL-ADR test phase
- A LoRaWan ADR-MAX test
- A LoRaWan ADR-AVG test
- A LoRaWan ADR-MIN test

All test phases run under the same context, thus allowing a more reasonable comparison between them. The training phase may have a different round number, but it is something shaped according to the experiments' goals.
At the end of the simulations' stage, the main script will format and store the results for the KPI at hand: **Energy** (mJ utilized/packets transmitted) and the **Goodput** (packets received/packets transmitted). These files are stored in a CSV format.
The last step is carried out by a Python script that reads the aforementioned CSV files and generates the whisker boxplot comparing the results for 4 different ADR types. 
Such results and PNG image files are saved in  */tmp* directory of the host.

### Parameters
The ***rladr-lorans3/automation-interface/nodes.csv*** file is a plain text file containing the nodes' coordinates, identified line by line. Each line contains the positional coordinates x,y,z described by integer numbers. Given that each line represents an unique device location, the number of lines of the file will be processed as the number of nodes used for the simulations.
The ***rladr-lorans3/automation-interface/arguments.yml*** file holds the settings for the NS3 simulations. Their semantics are the following:
  - training : section to define variables for training phase 
  - enable: whether the training will run or not (loads and updates the Q table for RL)
  - max_rounds: number of rounds to run the training
  - gamma: discount factor for RL. 
  - epsilon: initial exploration factor for RL 
  - alpha: learning rate for RL.
  - round_time: each round timespan, given in seconds
  - rladr: section to define the scope of variables for the RLADR test 
  - mode: define which mode RL is using. 0 means no RL, 1 runs with RL. 
  - algorithm: type of ADR used. Possible values: AVERAGE, MAXIMUM, and MINIMUM.
  - gamma: discount factor (valid when mode equals 1)
  - epsilon: exploration factor (valid when mode equals 1)
  - alpha: learning rate (valid when mode equals 1)
  - context: scope to define arguments of the NS3 scenario.
  - buildings_radius: this is the radial area in which NS3 will deploy 3D buildings (walls for interference). The positions of the buildings are randomly generated and saved in the local file* **buildings.txt***. 
  - round_time: NS3 test time for each round, given in seconds.
  - max_rounds: Number of test rounds
  - pkg_size: the size of all packages used in the simulation. This must be 14 for the time being.

### Usage

To run the simulation, simply edit the input parameters files and run it by calling docker-compose:
    `   docker-compose up --build`

Using *--build* is good because it forces the refresh/recreation of the container locally, although it is optional.

### Caveats
- The docker image [rladr-lorans3](https://hub.docker.com/repository/docker/rzuolo/rladr-lorans3 "rladr-lorans3") is huge (~ 2.4 GB) and it should be reduced. As this is at an experimental phase and being handled locally only, it is not at the top of priorities.
- The NS3 compilation is not optimized. There is (a lot) of room for improvement in the gcc parameters and possibly in the unnecessary modules removal (keep only lorawan and related helpers).
- The Python.c is able to run from inside NS3 but it still requires some more complex tests and validation (e.g. session persistence, multiple python inclusion, data exchange, etc).
- The RL training is favoring the Goodput performance to the detriment of energy. It selects the best performing goodput Qtable among those produced during the training phase.
- The package size is hardcoded to 14 inside the C++ lorawan modules. This can be changed and it is in the list for the next improvements cycle. :-) 
- This has been tested with ***docker-compose version 1.25.0***. Different versions are not contemplated although they still may work. 

### TBD PROD
