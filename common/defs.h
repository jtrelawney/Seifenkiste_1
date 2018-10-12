/*
 * defs.h
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

unsigned long get_timestamp();

//enums
enum sender_type_def {undefined_platform, arduino, rpi, jetson, pc};
enum sensor_type_def {undefined_sensor, imu, usonic1, camera1, time_snapshot};

#endif /* COMMON_DEFS_H_ */
