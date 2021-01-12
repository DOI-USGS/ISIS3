# run spice init with default parameters only
APPNAME = spiceinit

include $(ISISROOT)/make/isismake.tsts

commands:
#    TEST A: Check that an isis2 cube fails to be read
	echo -e "Test Spiceinit with an isis2 cube:" > $(OUTPUT)/error_message_temp.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/isis2Truth.cub \
	  2>> $(OUTPUT)/error_message_temp.txt > /dev/null` ]; \
	  then true; \
	fi;
	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_message_temp.txt > $(OUTPUT)/error_message.txt;
	$(RM)  $(OUTPUT)/error_message_temp.txt;