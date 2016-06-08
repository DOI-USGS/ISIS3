APPNAME = cnetadd

include $(ISISROOT)/make/isismake.tsts

outnet.pvl.IGNORELINES = DateTime

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/add.lis;
	$(APPNAME) cnet=$(INPUT)/seedGrid.net addlist=$(OUTPUT)/add.lis \
	  onet=$(OUTPUT)/outnet.net log=$(OUTPUT)/outlog.pvl \
	  retrieval=POINT tolist=$(OUTPUT)/outlist.txt \
	  > /dev/null;
	# Pipe to sort since output is obtained from a QHash
	$(CAT) $(OUTPUT)/outlist.txt | sed 's#.*/##' | sort > $(OUTPUT)/temp.txt;
	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/outlist.txt;
	$(RM) $(OUTPUT)/add.lis;
