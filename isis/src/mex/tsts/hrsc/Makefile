# @history 2019-05-21 Kaitlyn Lee - Original version

include $(ISISROOT)/make/isismake.tsts

commands:
	hrsc2isis $(TSTARGS) from=$(INPUT)/h0528_0000_re2.img to=$(OUTPUT)/h0528_0000_re2.cub > /dev/null

	spiceinit $(TSTARGS) from=$(OUTPUT)/h0528_0000_re2.cub ckpredicted=true  > /dev/null

	campt $(TSTARGS) from=$(OUTPUT)/h0528_0000_re2.cub \
		to=$(OUTPUT)/h0528_0000_re2.img--finalOutput.pvl > /dev/null

	echo -e "\nOutput of getsn..." > $(OUTPUT)/h0528_0000_re2.img--getsnOutput.txt 
	getsn $(TSTARGS) from=$(OUTPUT)/h0528_0000_re2.cub  \
		>> $(OUTPUT)/h0528_0000_re2.img--getsnOutput.txt
	
	rm $(OUTPUT)/h0528_0000_re2.cub > /dev/null
	rm $(OUTPUT)/../print.prt > /dev/null