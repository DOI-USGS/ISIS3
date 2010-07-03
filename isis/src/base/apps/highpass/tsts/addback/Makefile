APPNAME = highpass

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/highTruth1.cub \
	  samples=5 \
	  lines=9 \
	  addback=.5 > /dev/null;
