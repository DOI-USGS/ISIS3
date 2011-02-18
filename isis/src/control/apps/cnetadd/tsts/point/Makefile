APPNAME = cnetadd

include $(ISISROOT)/make/isismake.tsts

outnet.pvl.IGNORELINES = DateTime

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/add.lis;
	$(APPNAME) innet=$(INPUT)/seedGrid.net addlist=$(OUTPUT)/add.lis \
	outnet=$(OUTPUT)/outnet.bin log=$(OUTPUT)/outlog.pvl \
	retrieval=POINT tolist=$(OUTPUT)/outlist.txt \
	> /dev/null;
	cnetbin2pvl from=$(OUTPUT)/outnet.bin \
	to=$(OUTPUT)/outnet.pvl > /dev/null;
	$(CAT) $(OUTPUT)/outlist.txt | sed 's#.*/##' > $(OUTPUT)/temp.txt;
	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/outlist.txt;
	$(RM) $(OUTPUT)/add.lis $(OUTPUT)/outnet.bin;
