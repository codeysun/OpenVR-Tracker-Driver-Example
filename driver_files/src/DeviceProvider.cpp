#include <DeviceProvider.h>

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext)
{
    EVRInitError initError = InitServerDriverContext(pDriverContext);
    if (initError != EVRInitError::VRInitError_None)
    {
        return initError;
    }
    
    VRDriverLog()->Log("Initializing example tracker"); //this is how you log out Steam's log file.

    trackerDriver = new TrackerDriver();
    VRServerDriverHost()->TrackedDeviceAdded("tracker_example_1234", TrackedDeviceClass_GenericTracker, trackerDriver); //add all your devices like this. Sets the serial number of the device

    return vr::VRInitError_None;
}

void DeviceProvider::Cleanup()
{
    delete trackerDriver;
    trackerDriver = NULL;
}
const char* const* DeviceProvider::GetInterfaceVersions()
{
    return k_InterfaceVersions;
}

void DeviceProvider::RunFrame()
{
    trackerDriver->RunFrame();
}

bool DeviceProvider::ShouldBlockStandbyMode()
{
    return false;
}

void DeviceProvider::EnterStandby() {}

void DeviceProvider::LeaveStandby() {}