APPNAME = lorri2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	if [ `$(APPNAME) \
	  from=$(INPUT)/lor_0034974380_0x630_sci_1_badimage.fit \
	  to=$(OUTPUT)/lor_0034974380_0x630_sci_1_Truth.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(lor_0034974380_0x630_sci_1_badimage.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_image.txt;
	$(RM) $(OUTPUT)/temp.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/lor_0034974380_0x630_sci_1_baderror.fit \
	  to=$(OUTPUT)/lor_0034974380_0x630_sci_1_Truth.cub \
	  error=$(OUTPUT)/lor_0034974380_0x630_sci_1_Error.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(lor_0034974380_0x630_sci_1_baderror.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_error.txt;
	$(RM) $(OUTPUT)/temp.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/lor_0034974380_0x630_sci_1_badquality.fit \
	  to=$(OUTPUT)/lor_0034974380_0x630_sci_1_Truth.cub \
	  quality=$(OUTPUT)/lor_0034974380_0x630_sci_1_Quality.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(lor_0034974380_0x630_sci_1_badquality.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_quality.txt;
	$(RM) $(OUTPUT)/temp.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/lor_0034974380_0x630_sci_1_badimageinstr.fit \
	  to=$(OUTPUT)/lor_0034974380_0x630_sci_1_Truth.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(lor_0034974380_0x630_sci_1_badimageinstr.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_image_instr.txt;
	$(RM) $(OUTPUT)/temp.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/lor_0034974380_0x630_sci_1_baderrorimage.fit \
	  to=$(OUTPUT)/lor_0034974380_0x630_sci_1_Truth.cub \
	  error=$(OUTPUT)/lor_0034974380_0x630_sci_1_Error.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(lor_0034974380_0x630_sci_1_baderrorimage.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_errorimage.txt;
	$(RM) $(OUTPUT)/temp.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/lor_0034974380_0x630_sci_1_badqualityimage.fit \
	  to=$(OUTPUT)/lor_0034974380_0x630_sci_1_Truth.cub \
	  quality=$(OUTPUT)/lor_0034974380_0x630_sci_1_Quality.cub \
	  >& $(OUTPUT)/temp.txt` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(lor_0034974380_0x630_sci_1_badqualityimage.fit\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/error_qualityimage.txt;
	$(RM) $(OUTPUT)/temp.txt;

