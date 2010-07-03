APPNAME = median

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/medianTruth.cub \
	  samples=3 \
	  lines=3> /dev/null;
