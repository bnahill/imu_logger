define r
	mon reset halt
end

define l
	r
	load
	r
	set language c
end

file imu_logger.elf
target remote :3333
monitor reset init