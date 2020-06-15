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
	  to=$(OUTPUT)/output16.img \
	  bits=16 > /dev/null;	  
	  pds2hideal from=$(OUTPUT)/output16.lbl \
	  to=$(OUTPUT)/hideal2pds2hideal16bit.cub > /dev/null;	
	$(APPNAME) FROM=$(INPUT)/input.cub \
	  to=$(OUTPUT)/output8.img \
	  bits=8 > /dev/null;	  
	  pds2hideal from=$(OUTPUT)/output8.lbl \
	  to=$(OUTPUT)/hideal2pds2hideal8bit.cub > /dev/null;
	$(APPNAME) FROM=$(INPUT)/input.cub \
	  to=$(OUTPUT)/output10.img \
	  bits=10 > /dev/null;	  
	  pds2hideal from=$(OUTPUT)/output10.lbl \
	  to=$(OUTPUT)/hideal2pds2hideal10bit.cub > /dev/null;	    
	$(MV) $(OUTPUT)/output16.lbl $(OUTPUT)/output16.pvl;
	$(MV) $(OUTPUT)/output8.lbl $(OUTPUT)/output8.pvl;
	$(MV) $(OUTPUT)/output10.lbl $(OUTPUT)/output10.pvl;
	$(RM) $(OUTPUT)/output8.img;
	$(RM) $(OUTPUT)/output10.img;
	$(RM) $(OUTPUT)/output16.img;
	$(RM) $(OUTPUT)/output8_INSTRUMENT_POINTING_TABLE.dat;
	$(RM) $(OUTPUT)/output10_INSTRUMENT_POINTING_TABLE.dat;
	$(RM) $(OUTPUT)/output16_INSTRUMENT_POINTING_TABLE.dat;
	$(RM) $(OUTPUT)/output8_INSTRUMENT_POSITION_TABLE.dat;
	$(RM) $(OUTPUT)/output10_INSTRUMENT_POSITION_TABLE.dat;
	$(RM) $(OUTPUT)/output16_INSTRUMENT_POSITION_TABLE.dat;
	$(RM) $(OUTPUT)/output8_BODY_ROTATION_TABLE.dat;
	$(RM) $(OUTPUT)/output10_BODY_ROTATION_TABLE.dat;
	$(RM) $(OUTPUT)/output16_BODY_ROTATION_TABLE.dat;
	$(RM) $(OUTPUT)/output8_SUN_POSITION_TABLE.dat;
	$(RM) $(OUTPUT)/output10_SUN_POSITION_TABLE.dat;
	$(RM) $(OUTPUT)/output16_SUN_POSITION_TABLE.dat;


