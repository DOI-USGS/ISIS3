APPNAME = stats

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/tmp.txt format=flat > /dev/null;
	sed -e 's:/[^,]*isisTruth.cub:PATH:g' $(OUTPUT)/tmp.txt > $(OUTPUT)/stats.txt;
	$(RM) $(OUTPUT)/tmp.txt;
