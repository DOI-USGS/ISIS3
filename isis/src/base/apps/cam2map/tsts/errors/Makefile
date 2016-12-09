# This test will print errors thrown by the cam2map application.
APPNAME = cam2map

include $(ISISROOT)/make/isismake.tsts

commands:
#   TEST A: Testing for an error when list file is empty.
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
#   TEST B: Test for an error if cube has instrument targetName as Sky.
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
