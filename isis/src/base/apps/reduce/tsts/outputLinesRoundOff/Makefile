APPNAME = reduce

# This particular test is testing the calulation of scale and output line and sample.  This is a 
# result of a sporadic bug that was found (Mantis #1385) which only occurs at certain scales and
# user selected output number of lines.  For certain ONL values, the scale was calculated in
# the reduce application and passed into the Reduce object, which then calculated the ONL/ONS.  For
# certain scales there would be a round off error and the ONL/ONS would be 1 greater than what the
# user had entered.

NOPREFERENCES = true

labels.txt.IGNORELINES = ByteOrder StartByte Bytes TileSamples TileLines
results.pvl.SKIPLINES=2

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/testRoundOff.cub \
	  to=$(OUTPUT)/reduceTruth.cub \
	  algorithm=nearest \
	  mode=total \
	  ons=80 \
	  onl=483 > $(OUTPUT)/results.pvl;
	catlab from=$(OUTPUT)/reduceTruth.cub > $(OUTPUT)/labels.txt;

