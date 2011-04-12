APPNAME = cnetadd

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*M.cub > $(OUTPUT)/add.lis;
	$(LS) $(INPUT)/*cal.cub > $(OUTPUT)/files.lis;
	$(APPNAME) fromlist=$(OUTPUT)/files.lis \
	  cnet=$(INPUT)/input.net addlist=$(OUTPUT)/add.lis \
	  onet=$(OUTPUT)/outnet.net log=$(OUTPUT)/outlog.pvl \
	  tolist=$(OUTPUT)/outlist.txt > /dev/null;
	$(APPNAME) fromlist=$(OUTPUT)/files.lis \
	  cnet=$(INPUT)/input.net addlist=$(OUTPUT)/add.lis \
	  extract=modified modifiedpoints=$(OUTPUT)/modified.txt \
	  onet=$(OUTPUT)/modifiednet.net log=$(OUTPUT)/modifiedlog.pvl \
	  > /dev/null;
	$(CAT) $(OUTPUT)/outlist.txt | sed 's#.*/##' > $(OUTPUT)/temp.txt;
	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/outlist.txt;
	$(RM) $(OUTPUT)/add.lis;
	$(RM) $(OUTPUT)/files.lis;
