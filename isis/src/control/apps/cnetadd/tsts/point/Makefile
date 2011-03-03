APPNAME = cnetadd

include $(ISISROOT)/make/isismake.tsts

outnet.pvl.IGNORELINES = DateTime

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/add.lis;
	$(APPNAME) innet=$(INPUT)/seedGrid.net addlist=$(OUTPUT)/add.lis \
	  outnet=$(OUTPUT)/outnet.net log=$(OUTPUT)/outlog.pvl \
	  retrieval=POINT tolist=$(OUTPUT)/outlist.txt \
	  > /dev/null;
	$(CAT) $(OUTPUT)/outlist.txt | sed 's#.*/##' > $(OUTPUT)/temp.txt;
	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/outlist.txt;
	$(RM) $(OUTPUT)/add.lis;
