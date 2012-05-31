from parse import parse
from scipy.io import savemat
import sys

if __name__ == '__main__':
	if len(sys.argv) != 3:
		sys.exit("Please specify an input file and an output file")
	
	with open(sys.argv[1], 'r') as f:
		(header, data) = parse(f)
		mat = header
		for i in range(len(data)):
			mat["data"+str(i)] = data[i]
		savemat(sys.argv[2], mat, oned_as='column')