APPNAME = cnetnewradii

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) CNET=$(INPUT)/AS15_Rev4_Reg_wLOLA.edited.net \
	MODEL=$(INPUT)/ldem_8ppd_LATneg30toneg15_LON140to180.cub \
	TO=$(OUTPUT)/outputcnet.net getlatlon=apriori \
	networkid=AllSuccessesTestCase_CnetNewRadii \
	errors=$(OUTPUT)/errors.pvl > /dev/null;
