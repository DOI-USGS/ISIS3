APPNAME = photrim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102401.cub \
	  to= $(OUTPUT)/PhotrimTruth2.cub \
	  minphase=45.0 \
	  maxphase=135.0 \
	  minemission=0.0 \
	  maxemission=90.0 \
	  minincidence=0.0 \
	  maxincidence=90.0 > /dev/null;
