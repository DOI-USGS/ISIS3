# Test the IsisMain application code that is not covered by other tests.
APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	# GLOBALS entered.
	echo -e "TEST: GLOBALS provided." > $(OUTPUT)/output_temp.txt;
	$(APPNAME) config=$(INPUT)/mainProgram_test.conf \
	  parameters="inputdir:$(INPUT)" \
	  globals=$(INPUT)/globals.pvl  \
	  >> $(OUTPUT)/output_temp.txt;
	echo -e "===================================================" >> $(OUTPUT)/output_temp.txt;
	# PARAMETERS not entered.
	echo -e "TEST: No PARAMETERS string given." >> $(OUTPUT)/output_temp.txt; 
	$(APPNAME) config=$(INPUT)/mainProgram_test.conf \
	  parameters="inputdir:$(INPUT)" \
	  >> $(OUTPUT)/output_temp.txt; 
	echo -e "===================================================" >> $(OUTPUT)/output_temp.txt; 
	# PARAMETERS is ill-formed.
	echo -e "\nTEST Error: PARAMETERS string is ill-formed." >> $(OUTPUT)/output_temp.txt; 
	if [ `$(APPNAME) config=$(INPUT)/mainProgram_test.conf \
	  parameters="x" \
	  2>> $(OUTPUT)/output_temp.txt > /dev/null` ]; \
	  then true; \
	fi; 
	echo -e "===================================================" >> $(OUTPUT)/output_temp.txt; 
	# replace run times and elasped times in output and remove temp file.
	$(CAT) $(OUTPUT)/output_temp.txt \
	| $(SED) 's+[0-9][0-9]:[0-9][0-9]:[0-9][0-9].[0-9][0-9][0-9]+00:00:00.000+' \
	| $(SED) 's+):.[0-9].*+): 0+' \
	> $(OUTPUT)/output.txt; 
	$(RM) $(OUTPUT)/output_temp.txt;
