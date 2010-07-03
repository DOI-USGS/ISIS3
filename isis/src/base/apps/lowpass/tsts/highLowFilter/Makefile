APPNAME = lowpass

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to= $(OUTPUT)/lowTruth2.cub \
	  lines=7 \
	  samples=9 \
	  low=0 \
	  high=255 \
	  filter=outside > /dev/null;
