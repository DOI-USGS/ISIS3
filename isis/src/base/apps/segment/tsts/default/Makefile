APPNAME = segment

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/peaks.cub $(OUTPUT)/out.cub;
	$(APPNAME) from=$(OUTPUT)/out.cub \
	NL=256 OVERLAP=64 > /dev/null;
