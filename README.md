# csc461_project
CSC461 UVic Screen sharing 
One computer is actively sending the screen information to be displayed to one or more other computers in real time.

–  Project Name: Remote Real-time Collaboration (RRTC)
–  Team Members:
    –  Leader: Lucas Girouard-Stranks
    –  Main Contact: Brian McCormack
    –  Members: Ben Bennetti, Lucas Girouard-Stranks, Brian McCormack

Screen sharing. One computer is actively sending the screen information to be displayed to one or more other computers in real time.

## Updates 
- May 30th: Initiated project and created project proposal.
- June 4th: Delagte inital tasks ["mile stone one"](https://github.com/CanadianCommander/csc461_project/projects/1)
- June 9th: Working screen caputure w/o network.

## How to Build

1. Install core dependencies

    - CMake
    - SDL2

2. Build 3rd party dependencies

    - libvpx
        1. Configure
        2. Make
    - openh264
        1. Make
    - libde265
        1. CMake
        2. Make

3. Build project

```
cmake -H. -Bbuild && make -C ./build
```

4. Run
```
./build/csc461.a
```
