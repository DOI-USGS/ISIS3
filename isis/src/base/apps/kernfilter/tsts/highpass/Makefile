APPNAME = kernfilter

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/kernfilterTruth2.cub \
	  kernel=$(INPUT)/highpassCircle5x5.txt > /dev/null;
