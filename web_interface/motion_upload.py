# all the imports
import os
import sqlite3
from flask import Flask, request, session, g, redirect, url_for, \
                  abort, render_template, flash
from werkzeug import secure_filename
import random
import string
from datetime import datetime

# configuration
#DATABASE = '/tmp/flaskr.db'
DEBUG = True
SECRET_KEY = 'itsbob'
USERNAME = 'admin'
PASSWORD = 'flipper'


APP_ROOT = os.path.realpath(__file__).rpartition('/')[0]

UPLOAD_DIR = os.path.join(APP_ROOT, 'uploads')
STATIC_DIR = os.path.join(APP_ROOT, 'htdocs/static')

app = Flask(__name__)

@app.route('/')
def default():
	session['file_key'] = (datetime.now().strftime("%Y%m%d-%H%M%S-") +
	                       ''.join(random.choice(string.ascii_uppercase + string.digits) for x in range(6)))
	return render_template('main.html')

@app.route('/chunk_handler/<fname>', methods=['GET', 'POST'])
def chunk_handler(fname):
	if request.method == 'POST':
		for key, f in request.files.iteritems():
			filename = '-'.join([session['file_key'], secure_filename(fname)])
			with open(os.path.join(UPLOAD_DIR, filename), 'aw') as write_f:
				write_f.write(f.read())
	return str(request.files)

@app.route('/upload_complete')
def upload_complete():
	from parse import parse, check_file
	from mkmat import mkmat

	files = os.listdir(UPLOAD_DIR)
	files = filter(lambda x: x.rpartition('-')[0] == session['file_key'], files)

	file_list = []
	err = []
	for fname in files:
		path = os.path.join(UPLOAD_DIR, fname)
		with open(path) as f:
			try:
				res = check_file(f)
			except:
				res = False
			
			if not res:
				message = "This file does not appear to be a valid log"
				link = ""
				err.append("File '{}' is not in a valid format".format(fname))
			else:
				matfile = 'mat/{}.mat'.format(fname)
				message = "This file has been uploaded and converted"
				link = matfile
				try:
					mkmat(path, os.path.join(APP_ROOT, matfile))
				except AssertionError as e:
					message = "Assertion Error: " + e.message
					link = ''
					err.append("File '{}' hit an error: {}".format(fname,
						e.message))
					res = False
				except:
					message = 'Exception thrown in parsing'
					err.append("File '{}' hit an unknown error:"
						"{}".format(fname))
					link = ''
					res = False
			file_list.append((fname, res, message, link))
		os.unlink(path)

	err = ''.join(map(lambda x: "<p class=\"error\">"+x+"</p>\n", err))
	return render_template('upload_complete.html', files=file_list, message=err)
		
def sizeof_fmt(num):
	for x in ['bytes','KB','MB','GB']:
		if num < 1024.0:
			return "%3.1f%s" % (num, x)
		num /= 1024.0
	return "%3.1f%s" % (num, 'TB')

@app.route('/uploaded')
def uploaded():
	matdir = os.path.join(APP_ROOT, 'mat')
	files = os.listdir(matdir)
	# Filter out any non-mat files
	files = filter(lambda x: x.rpartition('.')[2] == 'mat', files)
	
	files.sort(reverse=True)

	flist = []
	for fname in files:
		link = '/mat/' + fname
		size = sizeof_fmt(os.path.getsize(os.path.join(matdir,fname)))
		flist.append((fname, link, size))
	
	return render_template('uploaded.html', files=flist)

app.debug=True
app.secret_key='6\xb9\x04\xefHG\xca;\xb5\x85\x9f\x04x\x0b\x0f_Vj/\xd71\x95\x13K'
