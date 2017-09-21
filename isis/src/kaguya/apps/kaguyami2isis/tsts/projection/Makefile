APPNAME = kaguyami2isis
FILE=MIA_3C5_03_01351S791E0024SC

include $(ISISROOT)/make/isismake.tsts

commands:
#    TEST A: Check that lronac2isis is returning the correct error messages
	if [ `$(APPNAME) from=$(INPUT)/$(FILE) \
	  to=$(OUTPUT)/$(FILE).cub > /dev/null \
	  2>> $(OUTPUT)/error_message_temp.txt > /dev/null` ]; \
	  then true; \
	fi;
	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_message_temp.txt > $(OUTPUT)/error_message.txt;
	$(RM)  $(OUTPUT)/error_message_temp.txt;