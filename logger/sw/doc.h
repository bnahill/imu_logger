/*!
 @mainpage
 
 @section mp_intro Introduction
 The IMU logger device is intended to provide a long-running logging device
 for inertial measurements. It does so using ST's
 @ref lsm "LSM303DLH accelerometer and magnetometer",
 @ref lpry "LPR430AL and LY330ALH gyroscopes", and
 @ref lps "LPS001DL pressure and temperature sensor".
 @ref logger "Logging" is done using a MicroSD card and the FatFS FAT32
 implementation.
 
 @section mp_operation Operation
 Upon reset, the device will initialize sensors and go to sleep until the
 button is pressed. Once activated, the device will flash 4 times and then be
 idle, waiting to be started by either pressing the button or an accelerometer
 interrupt (motion). Once started, the LED will flash once every two seconds
 while it records. An activity measurement is kept on 8 second frames to
 shut off the device when idle. When recording first starts, a single 8 second
 inactive frame is enough to cancel recording. After this initial period
 passes, it will require three 8 second periods of being idle to shut off.
 If cancelled on the first 8 second period, no data will be recorded to the SD
 card.
 
 @subsection mp_card_removal SD Card Removal
 You may safely remove the SD card any time the device is not recording.
 In addition to being stopped by inactivity, you can stop the device by
 pressing the button once. For extra safety, hold down the button and remove
 the card while the light is solid. As soon as the LED lights up, all writes
 are complete and nothing new can start.
 
 
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