APPNAME = leisa2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	# Test for existance of the MISSION keyword in the main FITS image label
	LC_CTYPE=C && LANG=C && $(SED) 's+MISSION = '"'"'New Horizons+MISSIOx = '"'"'New Horizons+' \
	    $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_mission-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the value of the MISSION keyword in the main FITS image label
	LC_CTYPE=C && LANG=C && $(SED) 's+MISSION = '"'"'New Horizons+MISSION = '"'"'NewXHorizons+' \
	    $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_mission-value.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the existance of the INSTR keyword in the main FITS image label
	LC_CTYPE=C && LANG=C && $(SED) 's+INSTRU  = '"'"'lei+INSTRz  = '"'"'lei+' \
	    $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_instru-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the value of the INSTRU keyword in the main FITS image label
	LC_CTYPE=C && LANG=C && $(SED) 's+INSTRU  = '"'"'lei+INSTRU  = '"'"'leY+' \
	    $(INPUT)/lsb_0034933739_0x53c_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_instru-value.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;



