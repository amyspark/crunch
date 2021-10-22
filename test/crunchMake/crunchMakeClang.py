#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-3.0-or-later
from argparse import ArgumentParser
from os import name as osName
from subprocess import run, PIPE
from sys import exit

parser = ArgumentParser(
	description = 'Light-weight wrapper around crunchMake to assert the output matches expectation',
	allow_abbrev = False
)
parser.add_argument('-c', required = True, type = str, metavar = 'crunchMake',
	help = 'Path to crunchMake to use')
parser.add_argument('-q', action = 'store_true', help = 'Use quiet mode?')
parser.add_argument('-i', required = True, type = str, metavar = 'inputFile',
	help = 'File that crunchMake will use as input')
parser.add_argument('-o', required = True, type = str, metavar = 'outputFile',
	help = 'File that crunchMake will write output to')
parser.add_argument('params', type = str, nargs = '*', help = 'crunchMake parameters')
args = parser.parse_args()

if args.q:
	if osName == 'nt':
		intermediateFile = '.'.join((args.o.rsplit('.', maxsplit = 1)[0], 'obj'))
	else:
		intermediateFile = '.'.join((args.o.rsplit('.', maxsplit = 1)[0], 'o'))
	expectedOutput = [
		' CC    ' + args.i + ' => ' + intermediateFile,
		' CCLD  ' + intermediateFile + ' => ' + args.o
	]
	quiet = ['-q']
else:
	quiet = []

result = run([args.c] + quiet + [args.i, '-o', args.o] + args.params, stdout = PIPE)
if result.returncode != 0:
	exit(result.returncode)

stdout = result.stdout.decode('UTF-8')
lines = stdout.splitlines()
if args.q:
	exit(0 if len(lines) == 2 and lines == expectedOutput else 1)
# TODO: figure out how to test this output is correct.. for now, assume it's fine.
# This is mainly for code coverage purposes anyway
