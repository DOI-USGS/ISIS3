APPNAME = mask

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/input.cub \
	  to=$(OUTPUT)/maskTruth.cub \
	  minimum=-1 \
	  maximum=300 \
	  preserve=inside \
	  spixels=all> /dev/null; \
	$(APPNAME) from=$(INPUT)/input.cub \
	  mask=$(INPUT)/input.cub+2 \
	  to=$(OUTPUT)/maskTruth2.cub \
	  minimum=-1 \
	  maximum=300 \
	  preserve=inside \
	  spixels=all> /dev/null;
