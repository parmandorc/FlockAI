# FlockAI
Implementation of flocking behavior AI developed in Unreal Engine 4.

Author: Pablo Rodriguez (parmandorc)

## Relevant links
- Download [FlockAI.app (Mac OS X)](https://mega.co.nz/#!EsA1TBAb!AoFh6SDRl98dKjriK1uROJLv3vpmhL2yPJzb-aUtfKA)
- Download [FlockAI.exe (Windows)](https://mega.co.nz/#!gxQDUSDY!qbo5SQzG-hMmCzlZ4kJxGFxImQv6CnssdCpCJHft7Ew)
- My [LinkedIn profile](https://es.linkedin.com/in/parmandorc/en)


## Engine Verion used
Unreal Engine 4.7.6


## Instructions for the game
- Basic movement using WASD or arrows.
- Mouse wheel to zoom in and out.
- Mouse left clic to spawn new Agents to the scene.
(Hold left clic and move the mouse to orient the starting direction of the agent, or right clic to cancel spawning).
- Press 2 in order to spawn Positive Stimuli (or food) instead when left clicking.
- Press 3 in order to spawn Negative Stimuli (or enemies) instead when left clicking.
- Press 1 again to spawn new Agents.


## About this project
This project implements a traditional flocking AI algorithm, with its 3 traditional components:
- Alignment: agents will face the same direction as their neighbors.
- Cohesion: agents will try to stay together as a group.
- Separation: agents will try to steer away from very close neighbors so the group doesn't collapse.

Nevertheless, while the traditional algorithm uses normalized vectors, this project takes a different approach by taking into account the distance between agents. This means for instance that an agent will try to separate from its neighbors more intensely if they are about to collide than if they are rather far away, which provides a much more realistic behavior.

Another new aspect is the introduction of external stimuli to the agents. This means they will react to points of interest in their field of view, simulating behaviors like going towards food or avoiding an enemy.

The purpose of this project is purely academic, and so its focus was to implement an Artificial Intelligence algorithm in a real time environment, but also to gain experience with Unreal Engine as a game development environment, and that is why the majority of the funcionalities of the game are developed using C++ programming, except for the main algorithm which is developed using Blueprint programming so as to gain experience with this technology.
