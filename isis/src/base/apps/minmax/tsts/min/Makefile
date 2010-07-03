APPNAME = minmax

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to= $(OUTPUT)/minmaxTruth1.cub \
	  filter=min \
	  samples=3 \
	  lines=3 > /dev/null;
