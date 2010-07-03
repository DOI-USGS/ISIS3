APPNAME = minmax

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to= $(OUTPUT)/minmaxTruth2.cub \
	  filter=max \
	  samples=4 \
	  lines=5 > /dev/null;
