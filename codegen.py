#!/usr/bin/env python
import sys,re

ignoreCmds = {('logger', 'log'): True}

id = lambda x:x

ntypes = {
	'const char*': ('std::string', 'String', lambda x: x+'.c_str()'),
	'float': ('float', 'Float', id),
	'double': ('double', 'Double', id),
	'int': ('int32_t', 'Int32', id),
	'short': ('int16_t', 'Int16', id),
	'unsigned char': ('uint8_t', 'Int8', id),
	'char': ('int8_t', 'Int8', id),
	'bool': ('bool', 'Bool', id)
}

jtypes = {
	'std::string': ('String', 'String', 'String'),
	'float': ('float', 'Float', 'Float'),
	'double': ('double', 'Double', 'Double'),
	'int32_t': ('int', 'Int', 'Int'),
	'int16_t': ('short', 'Short', 'Short'),
	'uint8_t': ('short', 'Byte', 'UnsignedByte'),
	'int8_t': ('byte', 'Byte', 'Byte'),
	'bool': ('boolean', 'Bool', 'Bool')
}

def nCallArg(narg):
	if narg[2] == 'out':
		return '&'+narg[1]
	else:
		return narg[0][2](narg[1])
		
def jDecArg(jarg):
	type = jarg[0][0]
	if jarg[2] == 'out':
		type = 'ByRef_'+type
	return type+" "+jarg[1]

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
		m = re.search('^\s*(\w(\w|\s)+?\*?)\s+\(\*(\w+)\)\s*\((.*)\);.*$', lines[i])
		if m == None:
			i+=1
			continue
		ret = m.group(1)
		name = m.group(3)
		if ignoreCmds.has_key((struct, name)):
			i+=1
			continue
		args = [[x.strip() for x in aDec.split()] for aDec in m.group(4).split(',')]
		args = [(" ".join(x[:-1]), x[-1:][0]) for x in args]
		if args == [('', 'void')]:
			args = []
		args2 = []
		for arg in args:
			if arg[1][0] == '*':
				args2.append((arg[0]+'*', arg[1][1:]))
			else:
				args2.append(arg)
		args = args2
		
		print 'struct="%s", ret="%s", name="%s", args=%s' % (struct,ret,name,args)
		enumName = struct+'_'+name
		enum.write(enumName+',\n')
		nins = []
		nouts = []
		nargs = []
		err = False
		for arg in args:
			if ntypes.has_key(arg[0]):
				nins.append((ntypes[arg[0]], arg[1]))
				nargs.append((ntypes[arg[0]], arg[1], "in"))
			elif arg[0][-1:] == '*' and ntypes.has_key(arg[0][:-1]):
				nouts.append((ntypes[arg[0][:-1]], arg[1]))
				nargs.append((ntypes[arg[0][:-1]], arg[1], "out"))
			else:
				print 'Unknown argument type: "'+arg[0]+'" for argument "'+arg[1]+'". Not generating code!'
				i+=1
				err = True
				break
		if err == True:
			continue
		try:
			if ret != 'void':
				nret = ntypes[ret]
		except KeyError as e:
			print 'Unknown return type: "'+ret+'". Not generating code!'
			i+=1
			continue
		jins = [(jtypes[arg[0][0]], arg[1]) for arg in nins]
		jouts = [(jtypes[arg[0][0]], arg[1]) for arg in nouts]
		jargs = [(jtypes[arg[0][0]], arg[1], arg[2]) for arg in nargs]
		if ret != 'void':
			jret = jtypes[nret[0]]
		nc  = "      case ClientCommand::"+enumName+":\n"
		nc += "      {\n"
		for narg in nins:
		#int32_t type = readInt32();
			nc  += "        %s %s = read%s();\n" % (narg[0][0], narg[1], narg[0][1])
		for narg in nouts:
			nc  += "        %s %s;\n" % (narg[0][0], narg[1])
		ncc = ", ".join([nCallArg(narg) for narg in nargs])
		retdec = ""
		if ret != 'void':
			retdec = nret[0]+" retval = "
		nc += "        "+retdec+"ms->"+struct+"."+name+"("+ncc+");\n"
		for narg in nouts:
			nc += "        write%s(%s);\n" % (narg[0][1], narg[1])
		if ret != 'void':
			nc += "        write%s(retval);\n" % nret[1]
		nc += "        break;\n"
		nc += "      }\n"
		ncode.write(nc)
		jdec = ", ".join([jDecArg(jarg) for jarg in jargs])
		if ret != 'void':
			jretType = jret[0]
		else:
			jretType = 'void'
		jc  =  "	public "+jretType+" "+enumName+"("+jdec+")\n"
		jc +=  "	{\n"
		jc +=  "		writeInt(ClientCommand."+enumName+".ordinal());\n"
		for jarg in jins:
			jc +=  "		write%s(%s);\n" % (jarg[0][1], jarg[1])
		jc +=  "		flush();\n"
		for jarg in jouts:
			jc +=  "		%s.value = read%s();\n" % (jarg[1], jarg[0][2])
		if ret != 'void':
			jc +=  "		return read"+jret[2]+"();\n"
		jc +=  "	}\n\n"
		jcode.write(jc)
		i+=1
