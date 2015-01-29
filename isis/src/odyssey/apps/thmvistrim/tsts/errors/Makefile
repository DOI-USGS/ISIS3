# This test will print errors thrown by the application.
.IGNORE:
APPNAME = thmvistrim

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "Error Test: input cube is not THEMIS VIS " > $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(INPUT)/I00831002RDR.cropped.cub \
	TO=$(OUTPUT)/I00831002RDR.trimmed.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/I00831002RDR.trimmed.cube 2> /dev/null;
	$(RM) $(OUTPUT)/error_temp.txt 2> /dev/null;
