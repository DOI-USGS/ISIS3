APPNAME = eis2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	# To test proper run with just the main image and an associated times file
	$(APPNAME) $(TSTARGS) from=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter.xml \
	  to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter2.cub \
	  mainreadout=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter_times.csv;
	catlab from=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter2.cub \
		to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter2_labels.pvl > /dev/null;
	
	# To test proper run with a checkline image and a checkline times file added
	$(APPNAME) from=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter.xml \
	  from2=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter_checkline.xml \
	  to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter1.cub \
	  to2=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter_checkline1.cub \
	  mainreadout=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter_times.csv \
	  checklinereadout=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter_checkline_times.csv;
	catlab from=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter1.cub \
		to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter1_labels.pvl > /dev/null;
	catlab from=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter_checkline1.cub \
		to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter_checkline1_labels.pvl > /dev/null;
	tabledump from=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter1.cub \
		name="Normalized Main Readout Line Times" to=$(OUTPUT)/main_readout.csv > /dev/null;
	tabledump from=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter1.cub \
		name="Checkline Readout Line Times" to=$(OUTPUT)/checkline_readout.csv > /dev/null;
	tabledump from=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter1.cub \
		name="Normalized Checkline Readout Line Times" to=$(OUTPUT)/checkline_readout.csv > /dev/null;

	# To test failed run with no times file for main image
	if [[ `$(APPNAME) \
			from=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter.xml \
		  to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter.cub \
			2>> $(OUTPUT)/error_temp.txt > /dev/null` \
	]]; \
	then \
		true; \
	fi;
	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	
	# To test failed run with a checkline image but no checkline times file
	if [[ `$(APPNAME) \
			from=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter.xml \
			from2=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter_checkline.xml \
			to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter.cub \
			to2=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter_checkline.cub \
			mainreadout=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter_times.csv \
			2>> $(OUTPUT)/error_temp.txt > /dev/null` \
	]]; \
	then \
		true; \
	fi;
	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	
	$(RM) $(OUTPUT)/error_temp.txt;
	
