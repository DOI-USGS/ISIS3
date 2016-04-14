# This tests the exceptions thrown by cnet2dem
#   A: "Must enter a control net in CNET or a list in CNETLIST"
APPNAME = cnet2dem

include $(ISISROOT)/make/isismake.tsts

commands:
	# TEST A: Not providing a control net
	echo -e "Error Test A: " > $(OUTPUT)/error_caseA.txt;
	if [[ `$(APPNAME) \
	   to=someToFile \
	   map=$(INPUT)/truthMap.map \
	   algorithm=AverageRadius \
	  2>> $(OUTPUT)/error_caseA.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_caseA.txt > $(OUTPUT)/errorA.txt;

	$(RM) $(OUTPUT)/error_caseA.txt;
	# TEST B: Not providing a map containing output map specifications
	echo -e "Error Test B: " > $(OUTPUT)/error_caseB.txt;
	if [[ `$(APPNAME) \
	   to=someToFile \
	   map=someMapFile \
	   algorithm=AverageRadius \
	  2>> $(OUTPUT)/error_caseB.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_caseB.txt > $(OUTPUT)/errorB.txt;

	$(RM) $(OUTPUT)/error_caseB.txt;
	# TEST C: Not providing a correct algorithm
	echo -e "Error Test C: " > $(OUTPUT)/error_caseC.txt;
	if [[ `$(APPNAME) \
	   to=someToFile \
	   map=$(INPUT)/truthMap.map \
	   algorithm=someAlgorithm\
	  2>> $(OUTPUT)/error_caseC.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_caseC.txt > $(OUTPUT)/errorC.txt;

	$(RM) $(OUTPUT)/error_caseC.txt;
