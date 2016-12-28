APPNAME =hrsc2isis

include $(ISISROOT)/make/isismake.tsts

commands:#    TEST A: Check that a projected HRSC image fails to be read
	echo -e "Test hrsc2isis with an image that has SRC data:" > $(OUTPUT)/error_message_temp.txt;
	if [ `$(APPNAME) \
	  FROM=$(INPUT)/h2862_0006_sr2.img \
	  TO=$(OUTPUT)/temp.cub \
	  2>> $(OUTPUT)/error_message_temp.txt > /dev/null` ]; \
	  then true; \
	fi;
	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_message_temp.txt > $(OUTPUT)/error_message.txt;
	$(RM)  $(OUTPUT)/error_message_temp.txt;
