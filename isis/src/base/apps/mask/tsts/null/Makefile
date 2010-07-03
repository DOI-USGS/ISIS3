APPNAME = mask

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  mask=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/maskTruth1.cub \
	  minimum=0 \
	  maximum=1000 \
	  preserve=inside> /dev/null;
