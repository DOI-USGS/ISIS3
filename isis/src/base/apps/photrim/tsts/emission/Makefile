APPNAME = photrim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102401.cub \
	  to= $(OUTPUT)/PhotrimTruth3.cub \
	  minphase=0.0 \
	  maxphase=180.0 \
	  minemission=22.5 \
	  maxemission=67.5 \
	  minincidence=0.0 \
	  maxincidence=90.0 > /dev/null;
