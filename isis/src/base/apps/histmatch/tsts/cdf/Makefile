APPNAME = histmatch

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	match=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/histmatchTruth1.cub \
	stretch=cdf \
	minper=0.5 \
	maxper=99.5 \
	increment=1 > /dev/null;

