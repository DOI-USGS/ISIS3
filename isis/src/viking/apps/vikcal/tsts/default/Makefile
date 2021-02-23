APPNAME = vikcal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/f387a06.cub \
	  to=$(OUTPUT)/vikcalTruth.cub > /dev/null;
	$(APPNAME) from=$(INPUT)/f319b18.cub \
	  to=$(OUTPUT)/vikcalCameraTruth.cub > /dev/null;
