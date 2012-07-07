#ifndef __LOGGER_H_
#define __LOGGER_H_

#include "jitter_buffer.h"

/*!
 @addtogroup log
 @{
 @addtogroup log_pub Public Interface
 @{
 */

/*!
 @brief Initialize the logger for a file with a given prefix
 @param prefix The filename prefix
 @return The filename used or NULL if there is an error
 */
const char *logger_init(const char *prefix);

/*!
 @brief Add data from a jitter buffer frame to the current log file
 @param frame The jitter buffer frame to write
 This may involve an SD card write.
 */
void logger_update(jb_frame_t const *frame);

/*!
 @brief Finalize current log page and write it
 
 This should only be called by other modules when closing down the log
 */
void logger_sync(void);

/*!
 @brief Close the current log file
 */
void logger_close(void);

int logger_is_open(void);


//! @}
//! @}

#endif
