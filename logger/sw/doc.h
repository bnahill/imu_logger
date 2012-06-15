/*!
 @mainpage
 
 @section mp_intro Introduction
 @p
 The IMU logger device is intended to provide a long-running logging device
 for inertial measurements. It does so using ST's
 @ref lsm "LSM303DLH accelerometer and magnetometer",
 @ref lpry "LPR430AL and LY330ALH gyroscopes", and
 @ref lps "LPS001DL pressure and temperature sensor".
 @ref logger "Logging" is done using a MicroSD card and the FatFS FAT32
 implementation.
 
 @section mp_operation Operation
 @sa application
 @sa main.c
 
 @section mp_parsing Parsing
 The data logs are packed into a minimal @ref logformat "logging format" and
 so a @ref parser "parser" is included to convert to MATLAB files.
 @sa logger
 
 */

/*!
 @addtogroup sensors Sensors
 @{
	 @defgroup lsm
	 @defgroup lpry
	 @defgroup lps

 @}
 */