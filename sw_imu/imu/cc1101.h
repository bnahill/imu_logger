/*!
 @file cc1101.h
 @brief Declaration of \ref CC1101 class
 @author Ben Nahill <bnahill@gmail.com>
 */

#ifndef __IMU_CC1101_H_
#define __IMU_CC1101_H_

#include "ch.h"
#include "imu/imu.h"
#include "imu/spi.h"
#include "array"

//! @addtogroup RF
//! @{

class CC1101 {
public:
	typedef enum {
		REG_IOCFG2 = 0x0000,
		REG_IOCFG1 = 0x0001,
		REG_IOCFG0 = 0x0002,
		REG_FIFOTHR = 0x0003,
		REG_SYNC1 = 0x0004,
		REG_SYNC0 = 0x0005,
		REG_PKTLEN = 0x0006,
		REG_PKTCTRL1 = 0x0007,
		REG_PKTCTRL0 = 0x0008,
		REG_ADDR = 0x0009,
		REG_CHANNR = 0x000A,
		REG_FSCTRL1 = 0x000B,
		REG_FSCTRL0 = 0x000C,
		REG_FREQ2 = 0x000D,
		REG_FREQ1 = 0x000E,
		REG_FREQ0 = 0x000F,
		REG_MDMCFG4 = 0x0010,
		REG_MDMCFG3 = 0x0011,
		REG_MDMCFG2 = 0x0012,
		REG_MDMCFG1 = 0x0013,
		REG_MDMCFG0 = 0x0014,
		REG_DEVIATN = 0x0015,
		REG_MCSM2 = 0x0016,
		REG_MCSM1 = 0x0017,
		REG_MCSM0 = 0x0018,
		REG_FOCCFG = 0x0019,
		REG_BSCFG = 0x001A,
		REG_AGCCTRL2 = 0x001B,
		REG_AGCCTRL1 = 0x001C,
		REG_AGCCTRL0 = 0x001D,
		REG_WOREVT1 = 0x001E,
		REG_WOREVT0 = 0x001F,
		REG_WORCTRL = 0x0020,
		REG_FREND1 = 0x0021,
		REG_FREND0 = 0x0022,
		REG_FSCAL3 = 0x0023,
		REG_FSCAL2 = 0x0024,
		REG_FSCAL1 = 0x0025,
		REG_FSCAL0 = 0x0026,
		REG_RCCTRL1 = 0x0027,
		REG_RCCTRL0 = 0x0028,
		REG_FSTEST = 0x0029,
		REG_PTEST = 0x002A,
		REG_AGCTEST = 0x002B,
		REG_TEST2 = 0x002C,
		REG_TEST1 = 0x002D,
		REG_TEST0 = 0x002E,
		REG_PARTNUM = 0x0030,
		REG_VERSION = 0x0031,
		REG_FREQEST = 0x0032,
		REG_LQI = 0x0033,
		REG_RSSI = 0x0034,
		REG_MARCSTATE = 0x0035,
		REG_WORTIME1 = 0x0036,
		REG_WORTIME0 = 0x0037,
		REG_PKTSTATUS = 0x0038,
		REG_VCO_VC_DAC = 0x0039,
		REG_TXBYTES = 0x003A,
		REG_RXBYTES = 0x003B,
		REG_RCCTRL1_STATUS = 0x003C,
		REG_RCCTRL0_STATUS = 0x003D,
		REG_TX_FIFO = 0x3F, //!< Write bytes to transmit here!
		REG_RX_FIFO = 0xBF, //!< Read bytes from here!
		// Command strobes
		CMD_SRES = 0x30,
		CMD_SFSTXON = 0x31,
		CMD_SFSTXOFF = 0x32,
		CMD_SCAL =  0x33,
		CMD_SRX = 0x34,
		CMD_STX = 0x35,
		CMD_SIDLE = 0x36,
		CMD_SWOR = 0x38,
		CMD_SPWD = 0x39,
		CMD_SFRX = 0x3A,
		CMD_SFTX = 0x3B,
		CMD_SWORRST = 0x3C,
		CMD_SNOP = 0x3D,
		CMD_PATABLE = 0x3E,
	} reg_t;
	
	typedef enum {
		GDO0 = REG_IOCFG0,
		GDO1 = REG_IOCFG1, // This is used for SPI...
		GDO2 = REG_IOCFG2
	} gdo_t;
	
	typedef uint8_t gdo_mode_t;
	
	typedef struct {reg_t reg; uint8_t val;} reg_config_t;
	
	CC1101(SPI &spi, SPI::slave_config_t slave_config);
	
	void config(reg_config_t const * const initial_config,
	            uint_fast8_t const init_config_len);
	
	void early_init();
	
	bool init();
	
	//! Go to low power mode
	void sleep();
	
	void set_gdo_mode(gdo_t gdo, gdo_mode_t gdo_mode);
	
	typedef enum {
		ST_IDLE      = 0,
		ST_RX        = 1,
		ST_TX        = 2,
		ST_FSTXON    = 3,
		ST_CALIBRATE = 4,
		ST_SETTLING  = 5,
		ST_RX_OFLOW  = 6,
		ST_TX_OFLOW  = 7
	} state_t;
	
	typedef uint8_t status_t;
	
	static state_t inline get_state(status_t stat){
		return static_cast<state_t>((stat & 0x70) >> 4);
	}
	
	static bool inline chip_rdy(status_t stat){
		return (stat & 0x80) != 0;
	}
	
	static uint8_t inline bytes_avail(status_t stat){
		return (stat & 0x0F);
	}
	
	status_t write_reg(reg_t reg, uint8_t value);
	
	uint8_t read_reg(reg_t reg);
	
	status_t get_status(){return strobe(CMD_SNOP);}
	
	/*!
	 @brief Do a single-word command strobe
	 */
	status_t strobe(reg_t reg);
	
	static void cb_rx_data_ready(CC1101 * rf);
	
	static constexpr uint8_t MASK_READ  = 0x80;
	static constexpr uint8_t MASK_BURST = 0x40;
protected:
	status_t transmit_data(uint_fast8_t const * data, uint_fast8_t len);
	
	status_t receive_data(uint_fast8_t * dst, uint_fast8_t len);
	
	SPI &spi;
	SPI::slave_config_t const slave_config;
};

//! @}

#endif // __IMU_CC1101_H_
