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


#APP_ROOT = '/home/bnahill/webapps/motion_upload'
APP_ROOT = os.path.realpath(__file__).rpartition('/')[0]

UPLOAD_DIR = os.path.join(APP_ROOT, 'uploads')
STATIC_DIR = os.path.join(APP_ROOT, 'htdocs/static')

# create our little application :)
app = Flask(__name__)
#app.config.from_object(__name__)

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
	files = filter(lambda x: x.partition('-')[0] == session['file_key'], os.listdir(UPLOAD_DIR))
	from parse import parse, check_file
	from mkmat import mkmat

	file_list = []	
	for fname in files:
		path = os.path.join(UPLOAD_DIR, fname)
		with open(path) as f:
			res = check_file(f)
			if not res:
				message = "This file does not appear to be a valid log"
				link = ""
			else:
				matfile = 'mat/{}.mat'.format(fname)
				message = "This file has been uploaded and converted"
				link = url_for('static', filename=matfile)
				mkmat(path, os.path.join(STATIC_DIR, matfile))
			os.unlink(path)
			file_list.append((fname, res, message, link))

	return render_template('uploaded.html', files=file_list)
	

app.debug=True
app.secret_key='6\xb9\x04\xefHG\xca;\xb5\x85\x9f\x04x\x0b\x0f_Vj/\xd71\x95\x13K'
