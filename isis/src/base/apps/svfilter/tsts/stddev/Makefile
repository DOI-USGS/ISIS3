APPNAME = svfilter

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/svFilTruth2.cub \
	  filter=stddev \
	  samples= 21 \
	  lines= 31 > /dev/null;
