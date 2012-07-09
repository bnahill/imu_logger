from parse import parse
from scipy.io import savemat
import sys

def mkmat(infile, outfile):
	with open(infile, 'r') as f:
		(header, data) = parse(f)
		mat = header
		for i in range(len(data)):
			mat["data"+str(i)] = data[i]
		savemat(outfile, mat, oned_as='column')

if __name__ == '__main__':
	if len(sys.argv) != 3:
		sys.exit("Please specify an input file and an output file")
	mkmat(sys.argv[1], sys.argv[2])

