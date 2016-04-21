# Test the StrategyFactory code that is not covered by other Strategy tests.
APPNAME = isisminer

include $(ISISROOT)/make/isismake.tsts

commands:
	# Config file containing an unknown strategy and a non-strategy object.
	# The non-strategy object will be skipped over when buildRun() is called.
	# An exception should be thrown when create() is called on the unknown strategy
	echo -e "Error Test: Unknown Strategy" > $(OUTPUT)/error_tmp.txt;
	if [ `$(APPNAME) config=$(INPUT)/error_unknownStrategy.conf \
	  parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" \
	  2>> $(OUTPUT)/error_tmp.txt > /dev/null` ]; \
	  then true; \
	fi;
	# Config file with no IsisMiner object.
	# Exception should be thrown when buildRun() is called
	echo -e "\nError Test: Missing IsisMiner Object" >> $(OUTPUT)/error_tmp.txt;
	if [ `$(APPNAME) config=$(INPUT)/error_noIsisMinerObject.conf \
	  parameters="inputdir:$(INPUT)@outputdir:$(OUTPUT)" \
	  2>> $(OUTPUT)/error_tmp.txt > /dev/null` ]; \
	  then true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_tmp.txt > $(OUTPUT)/strategyfactory_error.txt;

	$(RM) $(OUTPUT)/error_tmp.txt;
