APPNAME = cnetnewradii

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) CNET=$(INPUT)/AS15_Rev4_Reg_wLOLA.edited.net \
	DEM=$(INPUT)/ldem_8ppd_LATneg30toneg15_LON140to180.cub \
	TO=$(OUTPUT)/outputcnet.net \
	networkid=AllSuccessesTestCase_CnetNewRadii \
	errors=$(OUTPUT)/errors.pvl > /dev/null;
