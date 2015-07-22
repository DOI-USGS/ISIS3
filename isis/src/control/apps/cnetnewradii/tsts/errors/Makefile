# This test will print errors thrown by cnetnewradii
# There are four errors that can be thrown by the application. This tests one.
APPNAME = cnetnewradii

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "Valid Radii Test: " > $(OUTPUT)/error1.txt;
	if [ `$(APPNAME) \
	  CNET=$(INPUT)/AS15_Rev4_Reg_wLOLA.edited.net \
	  MODEL=$(INPUT)/ldem_8ppd_LATneg15to0_LON140to180.cub \
	  ONET=$(OUTPUT)/outputcnet.pvl \
	  getlatlon=adjusted \
	  errors=errors.pvl \
	  2>> $(OUTPUT)/error1.txt` ]; then \
	  true; \
	fi;
	$(SED) 's+/.*errors/++' $(OUTPUT)/error1.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error1.txt;
	$(RM) $(OUTPUT)/outputcnet.pvl;