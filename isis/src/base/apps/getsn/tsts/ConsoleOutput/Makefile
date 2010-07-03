APPNAME = getsn

peaks.txt.IGNORELINES = Filename
hirise.txt.IGNORELINES = Filename

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/peaks.cub \
	  FILE=false OBSERVATION=false \
          >& $(OUTPUT)/peaks.txt;
	$(APPNAME) FROM=$(INPUT)/hirise.cub \
	FILE=FALSE OBSERVATION=TRUE >& $(OUTPUT)/hirise.txt;
