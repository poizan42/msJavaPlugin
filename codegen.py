#!/usr/bin/env python
import sys,re

f = open(sys.argv[1], 'r');
ncode = open('ncode', 'w');
jcode = open('jcode', 'w');
enum = open('enum', 'w');
lines = f.readlines()
i = 0
while i < len(lines):
	m = re.search('^struct\s+(\w+)_pointer_struct$', lines[i])
	if m == None:
		i+=1
		continue
	struct = m.group(1)
	print "struct: "+struct
	i+=1
	m = None
	while True:
		if re.search('};', lines[i]) != None:
			i+=1
			break
		if re.search('#else', lines[i]) != None:
			i+=1
			while re.search('#endif', lines[i]) == None:
				i+=1
		m = re.search('^\s*(\w(\w|\s)+\*?)\s+\(\*(\w+)\)\s*\((.*)\);.*$', lines[i])
		if m != None:
			ret = m.group(1)
			name = m.group(3)
			args = [' '.join(x.strip().split()) for x in m.group(4).split(',')]
			print 'struct="%s", ret="%s", name="%s", args=%s' % (struct,ret,name,args)
			enum.write(struct+'_'+name+',\n')
		i+=1
