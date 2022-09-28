#include <TrackerDriver.h>

#define KEY_I 0x49
#define KEY_J 0x4A
#define KEY_K 0x4B
#define KEY_L 0x4C
#define KEY_U 0x55
#define KEY_O 0x4F

#define KEY_R 0x52
#define KEY_D 0x44
#define KEY_F 0x46
#define KEY_G 0x47
#define KEY_E 0x45
#define KEY_T 0x54


EVRInitError TrackerDriver::Activate(TrackedDeviceIndex_t unObjectId)
{
	driverId_ = unObjectId; //unique ID for your driver

	PropertyContainerHandle_t props = VRProperties()->TrackedDeviceToPropertyContainer(driverId_); //this gets a container object where you store all the information about your driver

	// Opt out of hand selection
	VRProperties()->SetInt32Property(props, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_OptOut);

	// Set the icon
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, "{example}/icons/tracker_ready.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, "{example}/icons/tracker_not_ready.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, "{example}/icons/tracker_not_ready.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{example}/icons/tracker_not_ready.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{example}/icons/tracker_not_ready.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, "{example}/icons/tracker_not_ready.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, "{example}/icons/tracker_not_ready.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, "{example}/icons/tracker_not_ready.png");

	// Set up threads
	getPoseThread_ = std::thread(&TrackerDriver::GetPoseThreaded, this);
	lastPoseTime = std::chrono::high_resolution_clock::now();

	return VRInitError_None;
}

DriverPose_t TrackerDriver::GetPose()
{
	/* From DriverPose_t :
	* Position of the driver tracking reference in driver world space
	* +[0] (x) is right
	* +[1] (y) is up
	* -[2] (z) is forward
	*/
	DriverPose_t pose = { 0 }; 
	pose.poseIsValid = true;
	pose.result = TrackingResult_Running_OK;
	pose.deviceIsConnected = true;

	std::chrono::time_point<std::chrono::high_resolution_clock> timeNow = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = (timeNow- lastPoseTime);
	float deltaT = duration.count();
	lastPoseTime = timeNow;

	HmdQuaternion_t quat;
	quat.w = 1;
	quat.x = 0;
	quat.y = 0;
	quat.z = 0;

	// Transform driver world space to app world space
	pose.qWorldFromDriverRotation = quat;
	// Transform driver local space to HMD head space
	pose.qDriverFromHeadRotation = quat;

	//Simple change yaw, pitch, roll with numpad keys
	// "quote" = aircraft coordinate system; unquote = openVR coordinate system
	// "pitch" is roll
	// "yaw" is pitch
	// "roll" is yaw
	if ((GetAsyncKeyState(KEY_E) & 0x8000) != 0) {
		yaw_ += 1.0f * deltaT;
	}
	if ((GetAsyncKeyState(KEY_T) & 0x8000) != 0) {
		yaw_ -= 1.0f * deltaT;
	}

	if ((GetAsyncKeyState(KEY_D) & 0x8000) != 0) {
		pitch_ += 1.0f * deltaT;
	}
	if ((GetAsyncKeyState(KEY_G) & 0x8000) != 0) {
		pitch_ -= 1.0f * deltaT;
	}

	if ((GetAsyncKeyState(KEY_R) & 0x8000) != 0) {
		roll_ += 1.0f * deltaT;
	}
	if ((GetAsyncKeyState(KEY_F) & 0x8000) != 0) {
		roll_ -= 1.0f * deltaT;
	}

	if ((GetAsyncKeyState(KEY_I) & 0x8000) != 0) {
		pZ_ -= 1.0f * deltaT;
	}
	if ((GetAsyncKeyState(KEY_K) & 0x8000) != 0) {
		pZ_ += 1.0f * deltaT;
	}

	if ((GetAsyncKeyState(KEY_J) & 0x8000) != 0) {
		pX_ -= 1.0f * deltaT;
	}
	if ((GetAsyncKeyState(KEY_L) & 0x8000) != 0) {
		pX_ += 1.0f * deltaT;
	}

	if ((GetAsyncKeyState(KEY_O) & 0x8000) != 0) {
		pY_ += 1.0f * deltaT;
	}
	if ((GetAsyncKeyState(KEY_U) & 0x8000) != 0) {
		pY_ -= 1.0f * deltaT;
	}

	// Position of the driver tracking reference in driver world space
	pose.vecPosition[0] = pX_;
	pose.vecPosition[1] = pY_;
	pose.vecPosition[2] = pZ_;

	double t0, t1, t2, t3, t4, t5;
	//Convert yaw, pitch, roll to quaternion
	t0 = cos(yaw_ * 0.5);
	t1 = sin(yaw_ * 0.5);
	t2 = cos(roll_ * 0.5);
	t3 = sin(roll_ * 0.5);
	t4 = cos(pitch_ * 0.5);
	t5 = sin(pitch_ * 0.5);

	//Set head tracking rotation
	pose.qRotation.x = t0 * t3 * t4 - t1 * t2 * t5;
	pose.qRotation.y = t0 * t2 * t5 + t1 * t3 * t4;
	pose.qRotation.z = t1 * t2 * t4 - t0 * t3 * t5;
	pose.qRotation.w = t0 * t2 * t4 + t1 * t3 * t5;

	return pose;
}

void TrackerDriver::GetPoseThreaded()
{
	VRDriverLog()->Log("Starting thread for tracker_example pose");
	while (driverId_ != k_unTrackedDeviceIndexInvalid)
	{
		VRServerDriverHost()->TrackedDevicePoseUpdated(driverId_, GetPose(), sizeof(DriverPose_t));
	}

	VRDriverLog()->Log("Stopping thread for tracker_example pose");
}

void TrackerDriver::RunFrame()
{
	//// In a real driver, this should happen from some pose tracking thread.
	//// The RunFrame interval is unspecified and can be very irregular if some other
	//// driver blocks it for some periodic task.
	//if (driverId_ != vr::k_unTrackedDeviceIndexInvalid) {
	//	VRServerDriverHost()->TrackedDevicePoseUpdated(driverId_, GetPose(), sizeof(DriverPose_t));
	//}
}

void TrackerDriver::Deactivate()
{
	driverId_ = k_unTrackedDeviceIndexInvalid;

	// Wait for threads
	getPoseThread_.join();

	VRDriverLog()->Log("All threads exitted");
}

void* TrackerDriver::GetComponent(const char* pchComponentNameAndVersion)
{
	//I found that if this method just returns null always, it works fine. But I'm leaving the if statement in since it doesn't hurt.
	//Check out the IVRDriverInput_Version declaration in openvr_driver.h. You can search that file for other _Version declarations 
	//to see other components that are available. You could also put a log in this class and output the value passed into this 
	//method to see what OpenVR is looking for.
	if (strcmp(IVRDriverInput_Version, pchComponentNameAndVersion) == 0)
	{
		return this;
	}
	return NULL;
}

void TrackerDriver::EnterStandby() {}

void TrackerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
	{
		pchResponseBuffer[0] = 0;
	}
}