# @history 2019-05-22 Kaitlyn Lee - Original version

include $(ISISROOT)/make/isismake.tsts

commands:
	thm2isis $(TSTARGS) from=$(INPUT)/I00831002RDR.QUB to=$(OUTPUT)/I00831002RDR.cub > /dev/null

	spiceinit $(TSTARGS) from=$(OUTPUT)/I00831002RDR.cub  > /dev/null

	campt $(TSTARGS) from=$(OUTPUT)/I00831002RDR.cub \
		to=$(OUTPUT)/I00831002RDR.QUB--finalOutput.pvl > /dev/null

	echo -e "\nOutput of getsn..." > $(OUTPUT)/I00831002RDR.QUB--getsnOutput.txt
	getsn $(TSTARGS) from=$(OUTPUT)/I00831002RDR.cub \
		>> $(OUTPUT)/I00831002RDR.QUB--getsnOutput.txt

	rm $(OUTPUT)/I00831002RDR.cub > /dev/null
	rm $(OUTPUT)/../print.prt > /dev/null
