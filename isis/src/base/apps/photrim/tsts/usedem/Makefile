APPNAME = photrim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/AS15-M-1499_msk-L2_1250mpp.cub \
	  to= $(OUTPUT)/PhotrimTruth5.cub \
	  usedem=true \
	  minphase=0.0 \
	  maxphase=180.0 \
	  minemission=0.0 \
	  maxemission=85.0 \
	  minincidence=0.0 \
	  maxincidence=90.0 > /dev/null;