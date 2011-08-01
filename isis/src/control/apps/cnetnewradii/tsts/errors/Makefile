.IGNORE:

APPNAME = cnetnewradii

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) CNET=$(INPUT)/AS15_Rev4_Reg_wLOLA.edited.net \
	  MODEL=$(INPUT)/ldem_8ppd_LATneg15to0_LON140to180.cub \
	  ONET=$(OUTPUT)/outputcnet.pvl getlatlon=adjusted \
	  errors=errors.pvl \
	  2>> $(OUTPUT)/error1.txt > /dev/null;
	cat $(OUTPUT)/error1.txt | sed s+/usgs.*errors/++g > $(OUTPUT)/error.txt;
	rm -f $(OUTPUT)/error1.txt;
	rm -f $(OUTPUT)/outputcnet.pvl;
