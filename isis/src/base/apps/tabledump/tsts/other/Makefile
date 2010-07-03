APPNAME =tabledump

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/blobTruth.cub \
	  to=$(OUTPUT)/tabledump2.txt \
	  delimit=OTHER \
	  custom="!TEST!" \
	  name="HiRISE Calibration Image" > /dev/null;
