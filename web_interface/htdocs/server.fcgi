#!/home/ben/usr/bin/python

from flup.server.fcgi import WSGIServer
import os
import sys

root = os.path.realpath(__file__).rpartition('/')[0].rpartition('/')[0]

sys.path += [ root, root + '/../logger/sw/parser' ]

from motion_upload import app

if __name__ == '__main__':
    WSGIServer(app).run()

