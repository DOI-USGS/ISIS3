APPNAME = mvic2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	# Test for existance of the MISSION keyword in the main FITS image label
	LC_CTYPE=C && LANG=C && $(SED) 's+MISSION = '"'"'New Horizons+MISSIOx = '"'"'New Horizons+' \
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
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
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_mission-value.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the existance of the INSTR keyword in the main FITS image label
	LC_CTYPE=C && LANG=C && $(SED) 's+INSTRU  = '"'"'mvi+INSTRz  = '"'"'mvi+' \
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_instru-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for the value of the INSTRU keyword in the main FITS image label
	LC_CTYPE=C && LANG=C && $(SED) 's+INSTRU  = '"'"'mvi+INSTRU  = '"'"'mvY+' \
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_instru-value.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for throwing if the undistorted extension is not what we are expecting
	LC_CTYPE=C && LANG=C && $(SED) 's+COMMENT This is the bias-subtracted+COMzENT This is the bias-subtracted+' \
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  undistorted=true \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_comment-undistorted-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for throwing if the undistorted extension is not what we are expecting
	LC_CTYPE=C && LANG=C && $(SED) 's+COMMENT This is the bias-subtracted+COMMENT XYZs is the bias-subtracted+' \
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  undistorted=true \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_comment-undistorted-value.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for throwing if the error extension is not what we are expecting
	LC_CTYPE=C && LANG=C && $(SED) 's+COMMENT+CO-MENT+g' \
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  error=true \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_comment-error-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for throwing if the error extension is not what we are expecting
	LC_CTYPE=C && LANG=C &&$(SED) 's+COMMENT  1-sigma error per pixel for the image in extension 1+COMMENT  1-sigXY error per pixel for the image in extension 1+' \
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  error=true \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_comment-error-value.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for throwing if the quality extension is not what we are expecting
	LC_CTYPE=C && LANG=C && $(SED) 's+COMMENT+CO-MENT+g' \
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  quality=true \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_comment-quality-key.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for throwing if the quality extension is not what we are expecting
	LC_CTYPE=C && LANG=C && $(SED) 's+COMMENT Data quality flag for the image in extension 1+COMMENT Data quality flag for the image in exXYZsion 1+' \
	    $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  quality=true \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_comment-quality-value.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;
	# Test for missing NAIF ID
	LC_CTYPE=C && LANG=C && $(SED) 's+SPCSCID =+SPCxCID =+' $(INPUT)/mpf_0035126517_0x539_sci_1.fit > $(OUTPUT)/temp.fit
	if [ `$(APPNAME) \
	  from=$(OUTPUT)/temp.fit \
	  to=$(OUTPUT)/junk.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(temp.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_spcscid.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.fit;




