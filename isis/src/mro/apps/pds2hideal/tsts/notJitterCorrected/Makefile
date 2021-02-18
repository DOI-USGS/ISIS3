# This tests the following conditions:
#
# -- RationaleDescription value is copied from the Original Labels object
# -- Default version number is used.
# -- ImageJitterCorrected keyword does not exist in the input cube
# -- No AlphaCube group exists (implying a full size image, but this is a 
#    cropped image with no AphaCube to save tesing time)
# -- Center and Length keywords in the BandBin group have NANOMETERS as units
#
# To verify the image data, use pds2hideal to reimport to Isis. The reimported 
# cube hideal2pds2hideal.cub should be identical to input.cub
#
# To verify the table data, use table dump on the reimported cube.
#
# To verify the label data, change the lbl extension to pvl.
APPNAME = pds2hideal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/exported.lbl \
	  to=$(OUTPUT)/reimported.cub \
	  shapemodelpath="base/dems" > /dev/null;
	tabledump from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimportedInstrumentPointingTable.txt \
	  name=InstrumentPointing > /dev/null;
	tabledump from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimportedInstrumentPositionTable.txt \
	  name=InstrumentPosition > /dev/null;
	tabledump from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimportedBodyRotationTable.txt \
	  name=BodyRotation > /dev/null;
	tabledump from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimportedSunPositionTable.txt \
	  name=SunPosition > /dev/null;
	catlab from=$(OUTPUT)/reimported.cub \
	  to=$(OUTPUT)/reimported.pvl > /dev/null;
