APPNAME = stats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/tmp.txt format=flat > /dev/null;
	
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/tmp.txt format=flat append=true > /dev/null;
	$(CAT) $(OUTPUT)/tmp.txt | \
	  $(SED) 's+.*\/[^,]*isisTruth.cub+PATH+' | \
	  $(SED) 's/\([0-9]*\.[0-9]\{11\}\)[0-9]*/\1/g' \
	  > $(OUTPUT)/stats.txt;
	$(RM) $(OUTPUT)/tmp.txt;
	
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	 to=$(OUTPUT)/tmp2.txt format=flat append=false > /dev/null;
	$(CAT) $(OUTPUT)/tmp2.txt | \
	  $(SED) 's+.*\/[^,]*isisTruth.cub+PATH+' | \
	  $(SED) 's/\([0-9]*\.[0-9]\{11\}\)[0-9]*/\1/g' \
	  > $(OUTPUT)/stats2.txt;
	$(RM) $(OUTPUT)/tmp2.txt;
