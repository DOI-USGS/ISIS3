APPNAME = ascii2isis

include $(ISISROOT)/make/isismake.tsts

commands:
#   TEST A: Broken stream when not skipping header.
	echo -e "Error Test A: " > $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/input.txt \
	  to=$(OUTPUT)/output.cub \
	  lines=3 \
	  samples=3 \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error_temp.txt;
	$(RM) $(OUTPUT)/output.cub;
