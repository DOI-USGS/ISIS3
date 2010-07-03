APPNAME =tabledump

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/blobTruth.cub \
	  to=$(OUTPUT)/tabledump1.txt \
	  name="HiRISE Ancillary" > /dev/null;
	$(APPNAME) from=$(INPUT)/input.cub \
	to=$(OUTPUT)/stringcheck.txt \
	name=CameraStatistics > /dev/null;
