APPNAME = cnetadd

include $(ISISROOT)/make/isismake.tsts

outnet.pvl.IGNORELINES = DateTime
modifiednet.pvl.IGNORELINES = DateTime

commands:
	$(LS) $(INPUT)/*M.cub > $(OUTPUT)/add.lis;
	$(LS) $(INPUT)/*cal.cub > $(OUTPUT)/files.lis;
	$(APPNAME) fromlist=$(OUTPUT)/files.lis \
        innet=$(INPUT)/input.net addlist=$(OUTPUT)/add.lis \
	outnet=$(OUTPUT)/outnet.bin log=$(OUTPUT)/outlog.pvl \
	> /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/files.lis \
        innet=$(INPUT)/input.net addlist=$(OUTPUT)/add.lis \
	extract=modified modifiedpoints=$(OUTPUT)/modified.txt \
	outnet=$(OUTPUT)/modifiednet.bin log=$(OUTPUT)/modifiedlog.pvl \
	> /dev/null;
	cnetbin2pvl from=$(OUTPUT)/outnet.bin \
	to=$(OUTPUT)/outnet.pvl > /dev/null;
	cnetbin2pvl from=$(OUTPUT)/modifiednet.bin \
	to=$(OUTPUT)/modifiednet.pvl > /dev/null;
	$(RM) $(OUTPUT)/add.lis;
	$(RM) $(OUTPUT)/files.lis;
	$(RM) $(OUTPUT)/modifiednet.bin;
	$(RM) $(OUTPUT)/outnet.bin;
