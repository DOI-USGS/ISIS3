APPNAME = leisa2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1.fit \
          to=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_Truth.cub \
          > /dev/null;
	catlab from=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_Truth.cub > \
	  $(OUTPUT)/lsb_0034933739_0x53c_sci_1_Truth.pvl;
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1.fit \
	  to=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_Truth.cub \
	  error=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_Error.cub \
	  > /dev/null;
	catlab from=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_Error.cub > \
	  $(OUTPUT)/lsb_0034933739_0x53c_sci_1_Error.pvl;
	$(APPNAME) from=$(INPUT)/lsb_0034933739_0x53c_sci_1.fit \
	  to=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_Truth.cub \
	  quality=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_Quality.cub\
	  > /dev/null;
	catlab from=$(OUTPUT)/lsb_0034933739_0x53c_sci_1_Quality.cub > \
	  $(OUTPUT)/lsb_0034933739_0x53c_sci_1_Quality.pvl;
	# Test for the calibration error image name
	LC_CTYPE=C && LANG=C && $(SED) 's+XTENSION+XTEN-ION+g' \
	  $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  errormap=$(OUTPUT)/errormap.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_error-xtension-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the calibration error image extension name
	LC_CTYPE=C && LANG=C && $(SED) 's+EXTNAME+EXzNAME+g' \
	  $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  errormap=$(OUTPUT)/errormap.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_error-extname-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the calibration error image extension name incorrect
	LC_CTYPE=C && LANG=C && $(SED) 's+EXTNAME = '"'"'ERRORMAP+EXTNAME = '"'"'ERRxRMAP+g' \
	  $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  errormap=$(OUTPUT)/errormap.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_error-extname-value.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the calibration quality image name
	LC_CTYPE=C && LANG=C && $(SED) 's+XTENSION+XTEN-ION+g' \
	  $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  quality=$(OUTPUT)/quality.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_quality-xtension-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the calibration quality image extension name
	LC_CTYPE=C && LANG=C && $(SED) 's+EXTNAME+EXzNAME+g' \
	  $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  quality=$(OUTPUT)/quality.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_quality-extname-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the calibration quality image extension name incorrect
	LC_CTYPE=C && LANG=C && $(SED) 's+EXTNAME = '"'"'QUALITY+EXTNAME = '"'"'QUAxITY+g' \
	  $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  quality=$(OUTPUT)/quality.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_quality-extname-value.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;

