# OpenVR-Tracker-Driver-Example

This project provides an example of how one can create a tracking device driver for SteamVR/OpenVR. The goal is to demonstrate tracking overrides for development of third-party tracking systems compatible with any SteamVR HMD.

This example provides 6DOF control of the user's head tracking via the keyboard.
- `IJKL` control forward/back/left/right movement
- `UO` controls up/down movement
- `RDFG` controls up/down/left/right rotation (pitch/yaw)
- `ET` controls head tilt (roll)

## Build the project
- Clone the project and its submodules
    - `git clone --recursive https://gitlab.com/radionavlab/virtual-reality/openvr-tracker-driver-example.git`
- Navigate to the project directory
    - `cd openvr-tracker-driver-example`
- Build the project with CMake
    - `cmake ./`
- In Windows, CMake will generate a Visual Studio Project. Open the `Tracker-Driver-Example.sln` file and build the solution (F7).
    - The output folder should now contain a `tracker_example` folder

## Installation
### Install the driver
- Make sure SteamVR is installed
- Navigate to the SteamVR driver directory. By default, this should be `C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers`
- Copy the `tracker_example` output folder into the driver directory

### Enable tracking override
- Open the `steamvr.vrsettings` file. By default, this should be in the `C:\Program Files (x86)\Steam\config` directory
    - `steamvr.vrsettings` is a json file that specifies the settings for SteamVR
- Add the following blocks:
    ```
    "TrackingOverrides" : {
            "/devices/tracker_example/tracker_example_1234" : "/user/head"
   },
   "driver_tracker_example" : {
      "enable" : true
   },
   "steamvr" : {
    "activateMultipleDrivers" : true
    },
    ```
    - More information can be found in [OpenVR Documentation](https://github.com/ValveSoftware/openvr/wiki/TrackingOverrides)

### Configure tracker type
- Now, run SteamVR. The driver and tracking override should be fully functional, but you may see a laser pointer following the camera. This is because the driver is configured as a handheld controller. Open SteamVR Settings -> Controllers -> Manage Trackers.
    - Configure the Tracker Role of `/devices/tracker_example/tracker_example_1234` as anything other than `HELD IN HAND`. For example, `CAMERA` will work.

## Resources
- [Official OpenVR Documentation](https://github.com/ValveSoftware/openvr/wiki)
    - The OpenVR source code also contains sample code
- [An Overview of VR Software Components](https://fredemmott.com/blog/2022/05/29/vr-software-components.html)
    - Explanation of VR software stack + state of the industry
- [OpenVR Quick Start](https://github.com/osudrl/CassieVrControls/wiki/OpenVR-Quick-Start)
    - Explanation of OpenVR call flow
- [Open VR Driver Example](https://github.com/finallyfunctional/openvr-driver-example)
    - Simple example of a treadmill controller.
- [OpenVR Driver for DIY](https://github.com/r57zone/OpenVR-driver-for-DIY)
    - Keyboard controlled virtual HMD and hand controllers
- [Simple OpenVR Driver Tutorial](https://github.com/terminal29/Simple-OpenVR-Driver-Tutorial/blob/master/README.md)
    - Examples of all device types + debugging tutorial
