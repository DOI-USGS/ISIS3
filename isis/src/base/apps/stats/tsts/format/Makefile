APPNAME = stats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/tmp.txt format=flat > /dev/null;
	cat $(OUTPUT)/tmp.txt | \
	  sed 's/\/[^,]*isisTruth.cub/PATH/g' | \
	  sed 's/\([0-9]*\.[0-9]\{13\}\)[0-9]*/\1/g' \
	  > $(OUTPUT)/stats.txt;
	$(RM) $(OUTPUT)/tmp.txt;
