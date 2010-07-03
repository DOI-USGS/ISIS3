APPNAME = mask

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  mask=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/maskTruth3.cub \
	  minimum=-20 \
	  maximum=10 \
	  preserve=outside \
	  spixels=none > /dev/null;
