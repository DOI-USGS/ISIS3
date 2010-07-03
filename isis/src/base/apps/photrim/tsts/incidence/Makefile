APPNAME = photrim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102401.cub \
	  to= $(OUTPUT)/PhotrimTruth4.cub \
	  minphase=0.0 \
	  maxphase=180.0 \
	  minemission=0.0 \
	  maxemission=90.0 \
	  minincidence=22.5 \
	  maxincidence=67.5 > /dev/null;
