# Test for ringspt forward projection
# 
# The first run is a level 1 cube that tests all default parameters.
#
# The second run is a level 1 cube that also tests the following combinations of 
# parameters:
#     SAMPLE is user entered and LINE defaults to center
#     TO is user entered, FORMAT=FLAT, and APPEND=YES for new results file
#     ALLOWOUTSIDE=NO
#
# The third run is a level 1 cube that also tests the following combination of 
# parameters:
#     SAMPLE defaults to center and LINE is user entered
#     TO is user entered, FORMAT=FLAT, and APPEND=YES for existing results file
#
# The fourth run is a level 1 cube that also tests the following combination of 
# parameters:
#     SAMPLE is user entered and LINE is user entered
#
# The fifth run is a level 2 cube that also tests the following combinations of 
# parameters:
#     TO is user entered, FORMAT=PVL, and APPEND=NO
#
APPNAME = ringspt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.cub > $(OUTPUT)/redirectedOutput.txt;
	echo -e "Group = Results" &> $(OUTPUT)/centersample_centerline_lev1.pvl;
	$(CAT) $(OUTPUT)/redirectedOutput.txt | grep -v "Processed" | grep -v "ringspt" \
	  >> $(OUTPUT)/centersample_centerline_lev1.pvl;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.cub \
	  to=$(OUTPUT)/appendedOutput_tmp.txt \
	  format=flat \
	  append=yes \
	  sample=300 \
	  allowoutside=no > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.cub \
	  to=$(OUTPUT)/appendedOutput_tmp.txt \
	  format=flat \
	  append=yes \
	  line=300 > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.cub \
	  to=$(OUTPUT)/sample300_line300_lev1_tmp.txt \
	  format=flat \
	  append=no \
	  sample=300 \
	  line=300 > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  to=$(OUTPUT)/sample1024_line1024_lev2.pvl \
	  format=pvl \
	  append=no \
	  sample=1024 \
	  line=1024 > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/appendedOutput_tmp.txt > $(OUTPUT)/appendedOutput.txt;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/sample300_line300_lev1_tmp.txt > $(OUTPUT)/sample300_line300_lev1.txt;
	$(RM)  $(OUTPUT)/appendedOutput_tmp.txt;
	$(RM)  $(OUTPUT)/sample300_line300_lev1_tmp.txt;
	$(RM)  $(OUTPUT)/redirectedOutput.txt;
