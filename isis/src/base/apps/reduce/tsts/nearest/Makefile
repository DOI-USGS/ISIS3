APPNAME = reduce

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/isisTruth.cub \
	  to= $(OUTPUT)/reduceTruth2.cub \
	  algorithm=nearest \
	  sscale=3 \
	  lscale=4 > /dev/null;
