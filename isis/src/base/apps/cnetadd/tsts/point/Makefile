APPNAME = cnetadd

include $(ISISROOT)/make/isismake.tsts

outnet.pvl.IGNORELINES = DateTime

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/add.lis;
	$(APPNAME) innet=$(INPUT)/seedGrid.net addlist=$(OUTPUT)/add.lis \
	outnet=$(OUTPUT)/outnet.pvl log=$(OUTPUT)/outlog.pvl \
	retrieval=POINT \
	> /dev/null;
	$(RM) $(OUTPUT)/add.lis;
