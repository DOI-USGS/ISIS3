APPNAME = segment

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/peaks.cub $(OUTPUT)/out.cub;
	$(APPNAME) from=$(OUTPUT)/out.cub \
	NL=531 OVERLAP=88 > /dev/null;
