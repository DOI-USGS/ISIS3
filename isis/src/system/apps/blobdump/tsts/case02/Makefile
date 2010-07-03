APPNAME = blobdump

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/blobTruth.cub \
	  to=$(OUTPUT)/blobdump2.txt \
	  name="HiRISE Calibration Image" \
	  type=Table > /dev/null;
