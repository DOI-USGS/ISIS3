APPNAME = histeq

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	to=$(OUTPUT)/histeqTruth.cub \
	minper=0.5 \
	maxper=99.5 \
	increment=1 > /dev/null;
