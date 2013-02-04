# This tests the following conditions:
#
# -- RationaleDescription value is entered by the user.
# -- Version Number is entered by the user.
# -- ImageJitterCorrected keyword exists in the input cube
# -- The input has been cropped (An AlphaCube group exists)
# -- Center and Length keywords in the BandBin group have no units
#
# To verify the image data, use pds2hideal to reimport to Isis. The reimported 
# cube hideal2pds2hideal.cub should be identical to input.cub
#
# To verify the table data, use table dump on the reimported cube.
#
# To verify the label data, change the lbl extension to pvl.
APPNAME = hideal2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/input.cub \
	  to=$(OUTPUT)/output.img \
	  rationale_desc="user entered" \
	  version="2.0" > /dev/null;
	pds2hideal from=$(OUTPUT)/output.lbl \
	  to=$(OUTPUT)/hideal2pds2hideal.cub > /dev/null;
	tabledump from=$(OUTPUT)/hideal2pds2hideal.cub \
	  to=$(OUTPUT)/output_INSTRUMENT_POINTING_TABLE.txt \
	  name=InstrumentPointing > /dev/null;
	tabledump from=$(OUTPUT)/hideal2pds2hideal.cub \
	  to=$(OUTPUT)/output_INSTRUMENT_POSITION_TABLE.txt \
	  name=InstrumentPosition > /dev/null;
	tabledump from=$(OUTPUT)/hideal2pds2hideal.cub \
	  to=$(OUTPUT)/output_BODY_ROTATION_TABLE.txt \
	  name=BodyRotation > /dev/null;
	tabledump from=$(OUTPUT)/hideal2pds2hideal.cub \
	  to=$(OUTPUT)/output_SUN_POSITION_TABLE.txt \
	  name=SunPosition > /dev/null;
	$(MV) $(OUTPUT)/output.lbl $(OUTPUT)/output.pvl;
	$(RM) $(OUTPUT)/output.img;
	$(RM) $(OUTPUT)/output_INSTRUMENT_POINTING_TABLE.dat;
	$(RM) $(OUTPUT)/output_INSTRUMENT_POSITION_TABLE.dat;
	$(RM) $(OUTPUT)/output_BODY_ROTATION_TABLE.dat;
	$(RM) $(OUTPUT)/output_SUN_POSITION_TABLE.dat;
