APPNAME = desmile

include $(ISISROOT)/make/isismake.tsts

# Test using the same 2D spectral definition for the input and output.
# This should result in an output with no changes. Some camera failurs near the edges are
# causing the first and last couple of samples to be different
commands: nochange nosmile

nochange:
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1_wide-proj.cub \
	           to=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_wide-proj_nochange.cub \
	           smiledef='$$NewHorizons/calibration/NHSmileDefinitionNew.cub' \
	           objective='$$NewHorizons/calibration/NHSmileDefinitionNew.cub' > /dev/null;
	catlab from=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_wide-proj_nochange.cub \
	       to=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_wide-proj_nochange.pvl > /dev/null;
nosmile:
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1_wide-proj.cub \
	           to=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_wide-proj_nosmile.cub \
		   smiledef='$$NewHorizons/calibration/NHSmileDefinitionNew.cub' \
		   objective='$$NewHorizons/calibration/target-wavelengths-expanded.csv' > /dev/null;
	catlab from=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_wide-proj_nosmile.cub \
	       to=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_wide-proj_nosmile.pvl > /dev/null;

