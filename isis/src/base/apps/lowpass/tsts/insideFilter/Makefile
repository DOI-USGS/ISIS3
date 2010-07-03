APPNAME = lowpass

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/lowTruth4.cub \
	  lines=7 \
	  samples=9 \
	  filter=inside > /dev/null;
