/*
 * defs.h
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

typedef unsigned int time_format;

// enums
enum sender_type_def {undefined_platform, arduino, rpi, jetson, pc};
enum sensor_type_def {undefined_sensor, imu, usonic1, camera1, time_snapshot};

#endif /* COMMON_DEFS_H_ */
