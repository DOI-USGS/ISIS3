APPNAME = cnetnewradii

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) CNET=$(INPUT)/AS15_Rev4_Reg_wLOLA.edited.net \
	MODEL=$(INPUT)/ldem_8ppd_LATneg30toneg18_LON142to178.cub \
	ONET=$(OUTPUT)/outputcnet.net getlatlon=apriori > /dev/null;
	mv failures.log $(OUTPUT)/failures.pvl;
