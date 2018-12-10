this directory contains all libraries used in the project
the cmake file compile the libraries



Implemented libraries:

sensor_camera_class

implements the class that operates a camera
- implements the device specific initialization
- implements the grabbing and decoding of a camera frame
- adds the management of the camera frame (most recent, valid/invalid)

sensor_base_class.h

implements the base class for sensors

basic functions common to all sensors

1. constructor init
2. init sensor - read parameter file and startup the device
3. acquire sensor data
4. create sensormessage
5. queue sensor message

from this class several sensortypes can be derived

camera_sensor_class.h

for example a camera sensor class:
1. read the specific parameter file
2. initializes the sensor accordingly
3. acquires camera images
4. creates and enqueues a sensor data object (type camera image) and puts it into the message queue
5. processes the camera image
6. creates and enqueues a sensor data object (type object positions) and puts it into the message queue
7. shuts down sensor
