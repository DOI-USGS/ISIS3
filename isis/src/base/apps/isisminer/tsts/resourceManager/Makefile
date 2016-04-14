APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	# Testing normal behaviors
	$(CP) $(INPUT)/isisTruth.pvl . > /dev/null
	$(CP) $(INPUT)/pvls.lis . > /dev/null
	$(APPNAME) config=$(INPUT)/resourceManager_test.conf \
         parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" > /dev/null	
	$(RM) isisTruth.pvl > /dev/null
	$(RM) pvls.lis > /dev/null
	# Testing Debug=True
	$(APPNAME) config=$(INPUT)/resourceManager_debug.conf \
	 parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" \
	 > $(OUTPUT)/temp.txt;
	# Remove timestamp and time elapsed data
	$(CAT) $(OUTPUT)/temp.txt \
	 | $(SED) 's+[0-9][0-9]:[0-9][0-9]:[0-9][0-9].[0-9][0-9][0-9]+00:00:00.000+' \
	 | $(SED) 's+):.[0-9].*+): 0+' \
	 > $(OUTPUT)/resourceManager_debug.txt;
	$(RM) $(OUTPUT)/temp.txt > /dev/null;
	# Testing ill-formed exceptions
	if [ `$(APPNAME) config=$(INPUT)/resourceManager_exception_illformed.conf \
	  parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" \
	  2> $(OUTPUT)/resourceManager_exceptions.txt > /dev/null` ]; \
	  then true; \
	fi;
	# Testing exceptions by not providing assets to DeleteAsset or HasAsset
	if [ `$(APPNAME) config=$(INPUT)/resourceManager_exception_notargetasset.conf \
	  parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" \
	  2>> $(OUTPUT)/resourceManager_exceptions.txt > /dev/null` ]; \
	  then true; \
	fi;
