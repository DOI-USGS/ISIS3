# Test for ringspt errors
.IGNORE:
APPNAME = ringspt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.cub \
	  format=flat \
	  2> $(OUTPUT)/errors.txt > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.cub \
	  sample=-1 \
	  line=-1 \
	  format=pvl \
	  allowoutside=no \
	  2>> $(OUTPUT)/errors.txt > /dev/null;
	$(APPNAME) from=$(INPUT)/W1591159850_1_cal.cub \
	  sample=-1000000000 \
	  line=-1000000000 \
	  format=pvl \
	  allowoutside=no \
	  2>> $(OUTPUT)/errors.txt > /dev/null;
