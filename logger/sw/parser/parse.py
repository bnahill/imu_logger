import struct
import os
import numpy as np
import sys

LOG_SENSOR_GYR1   = 0
LOG_SENSOR_GYR2   = 1
LOG_SENSOR_ACC1   = 2
LOG_SENSOR_ACC2   = 3
LOG_SENSOR_MAG1   = 4
LOG_SENSOR_MAG2   = 5
LOG_SENSOR_PRESS1 = 6
LOG_SENSOR_PRESS2 = 7
LOG_SENSOR_TEMP1  = 8
LOG_SENSOR_TEMP2  = 9

def parse(f):
	
	key = struct.unpack_from('<I', f.read(4))[0]
	assert key == 0xDEFEC8ED, "This is not a valid log file: Key = 0x%08X" % key
	print("Key: 0x%08X" % key)
	
	log_version = struct.unpack_from('<I', f.read(4))[0]
	assert (log_version == 1), "This log version is not supported"
	print("Log Version: %u" % log_version)
	
	device_id = struct.unpack_from('B' * 16, f.read(16))
	device_id_string = ":".join(map(lambda x: "%02X" % x, device_id))
	print("Device ID: " + device_id_string)
	
	page_size = struct.unpack_from('<I', f.read(4))[0]
	print("Page Size: %u" % page_size)
	
	master_fs = struct.unpack_from('<I', f.read(4))[0]
	print("Master f_s: %u" % master_fs)
	
	sub_rates = struct.unpack_from('B'*10, f.read(10))
	gyro1_rate = sub_rates[LOG_SENSOR_GYR1]
	gyro2_rate = sub_rates[LOG_SENSOR_GYR2]
	acc1_rate = sub_rates[LOG_SENSOR_ACC1]
	acc2_rate = sub_rates[LOG_SENSOR_ACC2]
	mag1_rate = sub_rates[LOG_SENSOR_MAG1]
	mag2_rate = sub_rates[LOG_SENSOR_MAG2]
	press1_rate = sub_rates[LOG_SENSOR_PRESS1]
	press2_rate = sub_rates[LOG_SENSOR_PRESS2]
	temp1_rate = sub_rates[LOG_SENSOR_TEMP1]
	temp2_rate = sub_rates[LOG_SENSOR_TEMP2]
	
	header = {'key':key, 'log_version':log_version, 'device_id':device_id,
	          'page_size':page_size, 'sample_rate':master_fs}
	
	"""
	Done with header
	"""
	
	# Advance to beginning of first data frame
	f.seek(page_size)
	
	# Concatenation of formatted framesets
	result_stream = []
	pagenum = 0
	
	while True:
		# Read a page
		page_buffer = f.read(page_size)
		if(len(page_buffer) != page_size):
			print("Page done!")
			# Done!
			break
		page = np.frombuffer(page_buffer, dtype=np.uint8)
		
			
		print("Page %u" % pagenum)
		pagenum += 1
		
		# Create in iterator to move around page
		page_remaining = page
		
		frameset_num = 0
		
		while True:
			if len(page_remaining) == 0:
				# print("Frameset done!")
				break

			frameset_header = struct.unpack_from('<I', page_remaining)[0]
			print("Frameset header (int): %u, %f" % (frameset_header & 0x7FFFFFFF, struct.unpack_from('<f', page_remaining)[0]))
			if frameset_header == 0:
				# print("Frameset done!")
				# Page is done
				break
			
			page_remaining = page_remaining[4:]
			
			# print("Frameset %u" % frameset_num)
			frameset_num += 1
			
			if frameset_header & 0x80000000 == 0:
				print("Silent frameset identified")
				# Idle frameset
				result_stream.append(int(frameset_header))
				page_remaining = page_remaining[4:]
				continue
			
			if gyro1_rate:
				gyro1p = []
				gyro1r = []
				gyro1y = []
			if gyro2_rate:
				gyro2p = []
				gyro2r = []
				gyro2y = []
			if acc1_rate:
				acc1x = []
				acc1y = []
				acc1z = []
			if acc2_rate:
				acc2x = []
				acc2y = []
				acc2z = []
			if mag1_rate:
				mag1x = []
				mag1y = []
				mag1z = []
			if mag2_rate:
				mag2x = []
				mag2y = []
				mag2z = []
			if press1_rate:
				press1 = []
			if press2_rate:
				press2 = []
			if temp1_rate:
				temp1 = []
			if temp2_rate:
				temp2 = []

			if int(frameset_header & 0x7FFFFFFF) > 10000:
				sys.exit("TOO BIG!!!")
			for sample in range(frameset_header & 0x7FFFFFFF):
				if gyro1_rate and (sample % gyro1_rate == 0):
					(p,r,y) = struct.unpack_from('<fff', page_remaining)
					gyro1p.append(p)
					gyro1r.append(r)
					gyro1y.append(y)
					page_remaining = page_remaining[12:]
				if gyro2_rate and (sample % gyro2_rate == 0):
					(p,r,y) = struct.unpack_from('<fff', page_remaining)
					gyro2p.append(p)
					gyro2r.append(r)
					gyro2y.append(y)
					page_remaining = page_remaining[12:]
				if acc1_rate and (sample % acc1_rate == 0):
					(x,y,z) = struct.unpack_from('<fff', page_remaining)
					acc1x.append(x)
					acc1y.append(y)
					acc1z.append(z)
					page_remaining = page_remaining[12:]
				if acc2_rate and (sample % acc2_rate == 0):
					(x,y,z) = struct.unpack_from('<fff', page_remaining)
					acc2x.append(x)
					acc2y.append(y)
					acc2z.append(z)
					page_remaining = page_remaining[12:]
				if mag1_rate and (sample % mag1_rate == 0):
					(x,y,z) = struct.unpack_from('<fff', page_remaining)
					mag1x.append(x)
					mag1y.append(y)
					mag1z.append(z)
					page_remaining = page_remaining[12:]
				if mag2_rate and (sample % mag2_rate == 0):
					(x,y,z) = struct.unpack_from('<fff', page_remaining)
					mag2x.append(x)
					mag2y.append(y)
					mag2z.append(z)
					page_remaining = page_remaining[12:]
				if press1_rate and (sample % press1_rate == 0):
					press1.append(struct.unpack_from('<f', page_remaining)[0])
					page_remaining = page_remaining[4:]
				if press2_rate and (sample % press2_rate == 0):
					press2.append(struct.unpack_from('<f', page_remaining)[0])
					page_remaining = page_remaining[4:]
				if temp1_rate and (sample % temp1_rate == 0):
					temp1.append(struct.unpack_from('<f', page_remaining[:4])[0])
					page_remaining = page_remaining[4:]
				if temp2_rate and (sample % temp2_rate == 0):
					temp2.append(struct.unpack_from('<f', page_remaining[:4])[0])
					page_remaining = page_remaining[4:]
			
			d = dict()
			
			if gyro1_rate:
				d['gyr1'] = np.array([gyro1p, gyro1r, gyro1y])
			if gyro2_rate:
				d['gyr2'] = np.array([gyro1p, gyro1r, gyro1y])
			if acc1_rate:
				d['acc1'] = np.array([acc1x, acc1y, acc1z])
			if acc2_rate:
				d['acc2'] = np.array([acc2x, acc2y, acc2z])
			if mag1_rate:
				d['mag1'] = np.array([mag1x, mag1y, mag1z])
			if mag2_rate:
				d['mag2'] = np.array([mag2x, mag2y, mag2z])
			if press1_rate:
				d['press1'] = np.array(press1, dtype=np.float32)
			if press2_rate:
				d['press2'] = np.array(press2, dtype=np.float32)
			if temp1_rate:
				d['temp1'] = np.array(temp1, dtype=np.float32)
			if temp2_rate:
				d['temp2'] = np.array(temp2, dtype=np.float32)
				
			result_stream.append(d)
		
		# Advance to the next page
		f.seek(page_size, os.SEEK_CUR)
	f.close()
	
	current_silence_stream = None
	current_data_stream = None
	
	new_result_stream = []
	
	for frameset in result_stream:
		if frameset.__class__ == dict:
			if current_silence_stream:
				# Commit silence stream
				new_result_stream.append(current_silence_stream)
				current_silence_stream = None
				
			if current_data_stream:
				current_data_stream = dict_append([current_data_stream, frameset])
			else:
				current_data_stream = frameset
		elif frameset.__class__ == int:
			if current_data_stream:
				new_result_stream.append(current_data_stream)
				current_data_stream = None
			
			if current_silence_stream:
				current_silence_stream += frameset
			else:
				current_silence_stream = frameset

	# Commit whatever is left over
	if current_data_stream:
		new_result_stream.append(current_data_stream)
	elif current_silence_stream:
		new_result_stream.append(current_silence_stream)
	
	"""
	Interpolate to get all to the same sample rate
	"""
	
	for frameset in new_result_stream:
		if frameset.__class__ != dict:
			# Only looking for those that contain data
			continue
		for key in frameset.keys():
			if key == 'gyr1':
				frameset[key] = interpolate3(frameset[key],sub_rates[LOG_SENSOR_GYR1]),
			if key == 'gyr2':
				frameset[key] = interpolate3(frameset[key],sub_rates[LOG_SENSOR_GYR2]),
			if key == 'acc1':
				frameset[key] = interpolate3(frameset[key],sub_rates[LOG_SENSOR_ACC1]),
			if key == 'acc2':
				frameset[key] = interpolate3(frameset[key],sub_rates[LOG_SENSOR_ACC2]),
			if key == 'mag1':
				frameset[key] = interpolate3(frameset[key],sub_rates[LOG_SENSOR_MAG1]),
			if key == 'mag2':
				frameset[key] = interpolate3(frameset[key],sub_rates[LOG_SENSOR_MAG2]),	
			elif key == 'press1':
				frameset[key] = interpolate(frameset[key],sub_rates[LOG_SENSOR_PRESS1])
			elif key == 'press2':
				frameset[key] = interpolate(frameset[key],sub_rates[LOG_SENSOR_PRESS2])
			elif key == 'temp1':
				frameset[key] = interpolate(frameset[key],sub_rates[LOG_SENSOR_TEMP1])
			elif key == 'temp2':
				frameset[key] = interpolate(frameset[key],sub_rates[LOG_SENSOR_TEMP2])
	
	return (header, new_result_stream)
		
def dict_append(dicts):
	ret = dict()
	for k in dicts[0].keys():
		ret[k] = dicts[0][k]
		for d in dicts[1:]:
			ret[k] = np.concatenate((ret[k].T, d[k].T)).T
			#ret[k] += d[k
	return ret

def interpolate3(mat, factor):
	if factor == 1:
		return mat
	return np.array([interpolate(mat[0], factor), interpolate(mat[1], factor), interpolate(mat[2], factor)])

def interpolate(arr, factor):
	if factor == 1:
		return arr
	x = range(len(arr) * factor)
	xp = np.linspace(0,len(arr)-1,len(arr)) * factor
	return np.interp(x, xp, arr)
