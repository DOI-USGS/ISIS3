APPNAME = gauss

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)//isisTruth.cub \
	  to=$(OUTPUT)/gaussTruth2.cub \
	  stddev = 2.0 \
	  size = 5 > /dev/null;
