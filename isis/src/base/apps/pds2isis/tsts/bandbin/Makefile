APPNAME = pds2isis


include $(ISISROOT)/make/isismake.tsts

# Test translation of PDS BAND_BIN group with BAND_BIN prefixed keywords.
commands:
	$(APPNAME) from=$(INPUT)/gaspra_nims_hires_radiance.lbl \
	  to=$(OUTPUT)/gaspra_nims_hires_radiance.cub > /dev/null; \
	catlab from=$(OUTPUT)/gaspra_nims_hires_radiance.cub \
	  to=$(OUTPUT)/gaspra_nims_hires_radiance.pvl > /dev/null;
