# Test for ringsmappt errors
.IGNORE:
APPNAME = ringsmappt

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "Error Test A:" > $(OUTPUT)/error_tmp.txt;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  sample=1024 \
	  line=1024 \
	  format=flat \
	  2>> $(OUTPUT)/error_tmp.txt > /dev/null;
	echo -e "Error Test B:" >> $(OUTPUT)/error_tmp.txt;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  type=ground \
	  ringradius=0 \
	  ringlongitude=0 \
	  coordsys=map \
	  map=$(INPUT)/W1591159850_1_cal.cub \
	  2>> $(OUTPUT)/error_tmp.txt > /dev/null;
	$(CAT) $(OUTPUT)/error_tmp.txt | grep -v "Processed" >& $(OUTPUT)/error_tmp2.txt;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/error_tmp2.txt > $(OUTPUT)/errors.txt;
	$(RM)  $(OUTPUT)/error_tmp.txt;
	$(RM)  $(OUTPUT)/error_tmp2.txt;
