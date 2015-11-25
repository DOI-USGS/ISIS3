APPNAME = desmile

include $(ISISROOT)/make/isismake.tsts

# Test using the same 2D spectral definition for the input and output.
# This should result in an output with no changes
commands:
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1_sm-proj.cub \
	           to=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_sm-proj_nochange.cub \
	           smiledef='$$NewHorizons/calibration/NHSmileDefinitionNew.cub' \
	           objective='$$NewHorizons/calibration/NHSmileDefinitionNew.cub' > /dev/null;
	catlab from=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_sm-proj_nochange.cub \
	       to=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_sm-proj_nochange.pvl > /dev/null;

