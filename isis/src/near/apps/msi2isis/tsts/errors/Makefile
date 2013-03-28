# This test will print errors thrown by the msi2isis application.
# 
#   TEST A: pass in a fits file without label
#   TEST B: Invalid Instrument ID
#   TEST C: Invalid size (number of lines)
#   TEST D: Invalid size (number of samples)
#   TEST E: Invalid Sample Display Direction
#   TEST F: Invalid Line Display Direction
#   TEST G: Unable to import projected images
#   TEST H: Unable to translate labels
.IGNORE:
APPNAME = msi2isis

include $(ISISROOT)/make/isismake.tsts

commands:
#   TEST A: pass in a fits file without label
	echo -e "Error Test A:" > $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/image.fit \
	to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST B: Invalid Instrument ID
	echo -e "\nError Test B:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/label1.lbl \
	to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST C: Invalid size
	echo -e "\nError Test C:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/label2.lbl \
	to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST D: Invalid size
	echo -e "\nError Test D:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/label3.lbl \
	to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST E: Invalid Sample Display Direction
	echo -e "\nError Test E:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/label4.lbl \
	to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST F: Invalid Line Display Direction
	echo -e "\nError Test F:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/label5.lbl \
	to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST G: Unable to import projected images
	echo -e "\nError Test G:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/label6.lbl \
	to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;
#   TEST H: Unable to translate labels
	echo -e "\nError Test H:" >> $(OUTPUT)/error_temp.txt;
	$(APPNAME) from=$(INPUT)/label7.lbl \
	to=$(OUTPUT)/output.cub 2>> $(OUTPUT)/error_temp.txt > /dev/null;

	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	rm -f $(OUTPUT)/error_temp.txt;
	rm -f $(OUTPUT)/input.cub;
	rm -f $(OUTPUT)/output.cub;
