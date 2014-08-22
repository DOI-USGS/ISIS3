# This test will print errors thrown by the application.
.IGNORE:
APPNAME = thmvistrim

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "Error Test: input cube is not THEMIS VIS " > $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(INPUT)/I00831002RDR.cropped.cub \
	TO=$(INPUT)/I00831002RDR.trimmed.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error_temp.txt;
