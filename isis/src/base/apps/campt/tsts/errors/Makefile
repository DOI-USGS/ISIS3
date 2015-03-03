# test for exceptions thrown
# the coordinate list is invalid if number of rows and columns don't match
# the coordinate list is invalid if there are not 2 columns
# no output file is provided when specifying the FORMAT as FLAT
APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	-$(APPNAME) from=$(INPUT)/ab102401.cub \
	to=$(OUTPUT)/output.pvl \
	coordlist=$(INPUT)/badlist.csv \
	coordtype=IMAGE 2> $(OUTPUT)/errorsTruth.txt > /dev/null;
	-$(APPNAME) from=$(INPUT)/ab102401.cub \
	to=$(OUTPUT)/output.pvl \
	coordlist=$(INPUT)/badcolumns.csv \
	coordtype=IMAGE 2>> $(OUTPUT)/errorsTruth.txt > /dev/null;
	-$(APPNAME) from=$(INPUT)/ab102401.cub \
	format=FLAT 2>> $(OUTPUT)/errorsTruth.txt > /dev/null;
