# This test will print errors thrown by the applicationName application.
APPNAME = cam2map

include $(ISISROOT)/make/isismake.tsts

commands:
#   TEST A: Test that an exception is thrown when targetnames of cube and map do not match.
	echo -e "Error Test A:" > $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/vesta.cub \
	  to=$(OUTPUT)/output.cub \
	  map=$(INPUT)/marsEquirectangular.map \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/\(input/.*\]\)\(.*\)\[/.*/\(input/.*\]\)+\[\1\2\[\3+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;

	$(RM) $(OUTPUT)/error_temp.txt;
	$(RM) $(OUTPUT)/output.cub;
