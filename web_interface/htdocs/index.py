import os
import sys
root = os.path.realpath(__file__).rpartition('/')[0].rpartition('/')[0]
#sys.path += [ '/home/bnahill/webapps/motion_upload' ]
sys.path += [ root, root+'/imu_logger/logger/sw/parser' ]
#from motion_upload import app as application
