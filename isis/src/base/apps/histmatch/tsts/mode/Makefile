APPNAME = histmatch

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	match=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/histmatchTruth2.cub \
	stretch=mode \
	minper=0.5 \
	maxper=99.5 > /dev/null;
