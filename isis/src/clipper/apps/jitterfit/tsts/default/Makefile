APPNAME = jitterfit

include $(ISISROOT)/make/isismake.tsts

commands:
	# To test proper run
	cp $(INPUT)/simulated_clipper_eis_nac_rolling_shutter.cub $(OUTPUT) > /dev/null;
	$(APPNAME) \
		from=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter.cub \
		from2=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter_checkline.cub \
		deffile=$(INPUT)/S046mos1400x2250.def \
		coefficientto=$(OUTPUT)/coefficients.txt  \
		scale=1.0 > /dev/null; 
	catlab from=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter.cub \
		to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter_labels.pvl > /dev/null;
