APPNAME = hist
histTruth.txt.IGNORELINES = Cube
TEMP = $(OUTPUT)/histTruth.txt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to= $(TEMP) > /dev/null;
