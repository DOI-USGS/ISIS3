APPNAME = reduce

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/peaks.cub \
	  to= $(OUTPUT)/reduceTruth5.cub \
	  algorithm=average \
	  sscale=7 \
	  lscale=7 > /dev/null;
