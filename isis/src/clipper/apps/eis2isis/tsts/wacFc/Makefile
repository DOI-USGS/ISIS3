APPNAME = eis2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	# To test proper run
	$(APPNAME) $(TSTARGS) from=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter.xml \
	  to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter.cub > /dev/null;
	catlab from=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter.cub \
	  to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter_labels.pvl > /dev/null;
