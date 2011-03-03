APPNAME = cnetadd

include $(ISISROOT)/make/isismake.tsts

outnet.pvl.IGNORELINES = DateTime

commands:
	$(LS) $(INPUT)/*M.cub > $(OUTPUT)/add.lis;
	$(LS) $(INPUT)/*cal.cub > $(OUTPUT)/files.lis;
	$(APPNAME) fromlist=$(OUTPUT)/files.lis \
	  innet=$(INPUT)/input.net addlist=$(OUTPUT)/add.lis \
	  outnet=$(OUTPUT)/outnet.net log=$(OUTPUT)/outlog.pvl \
	  deffile=$(INPUT)/deffile.def > /dev/null;
	$(RM) $(OUTPUT)/add.lis;
	$(RM) $(OUTPUT)/files.lis;
