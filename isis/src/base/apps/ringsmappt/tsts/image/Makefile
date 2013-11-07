# Test for ringsmappt image (interprets coordinate as (sample,line) ) - defaults
APPNAME = ringsmappt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.lev2.cub \
	  sample=1024 \
	  line=1024 > $(OUTPUT)/redirectedOutput.txt;
	$(CAT) $(OUTPUT)/redirectedOutput.txt | grep -v "Processed" \
	  >& $(OUTPUT)/userEnteredSampleLine.pvl;
	$(RM) $(OUTPUT)/redirectedOutput.txt;

