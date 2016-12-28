APPNAME = hrsc2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/h0279_0000_re2.img TO=$(OUTPUT)/h0279_0000_re2.cub > /dev/null;
	tabledump FROM=$(OUTPUT)/h0279_0000_re2.cub TO=$(OUTPUT)/h0279_0000_re2.txt NAME=LineScanTimes > /dev/null;
	catlab FROM=$(OUTPUT)/h0279_0000_re2.cub to=$(OUTPUT)/h0279_0000_re2.pvl > /dev/null;
	if [ `$(APPNAME) FROM=$(INPUT)/h1580_0008_sr3.img \
	  TO=$(OUTPUT)/junk.cub \
	   >& $(OUTPUT)/error_message_temp.txt` ]; \
	then true; \
	fi;
	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_message_temp.txt > $(OUTPUT)/error.txt;
	$(RM)  $(OUTPUT)/error_message_temp.txt;
