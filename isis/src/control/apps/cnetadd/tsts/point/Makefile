APPNAME = cnetadd

include $(ISISROOT)/make/isismake.tsts

outnet.pvl.IGNORELINES = DateTime

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/add.lis;
	$(APPNAME) innet=$(INPUT)/seedGrid.net addlist=$(OUTPUT)/add.lis \
	outnet=$(OUTPUT)/outnet.bin log=$(OUTPUT)/outlog.pvl \
	retrieval=POINT \
	> /dev/null;
	cnetbin2pvl from=$(OUTPUT)/outnet.bin \
	to=$(OUTPUT)/outnet.pvl > /dev/null;
	$(RM) $(OUTPUT)/add.lis $(OUTPUT)/outnet.bin;
