APPNAME = lronac2isis
FILE=nacl00015d79Edit

include $(ISISROOT)/make/isismake.tsts

commands:
#    TEST A: Check that lronac2isis is returning the correct error messages
	if [ `$(APPNAME) from=$(INPUT)/$(FILE).img \
	  to=$(OUTPUT)/$(FILE).cub > /dev/null \
	  2>> $(OUTPUT)/error_message_temp.txt > /dev/null` ]; \
	  then true; \
	fi;