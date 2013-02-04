# This test will print errors thrown by the hideal2pds application.
#
#   TEST : 
#     if INSTRUMENT_ID != HIRISE_IDEAL_CAMERA
.IGNORE:
APPNAME = pds2hideal

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "\nError Test :" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) FROM=$(INPUT)/invalidInstrumentId.lbl \
	  to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
	$(SED) s+\\[.*/input/+\\[input/+g $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error_temp.txt;
