APPNAME = cnetcombinept

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.net > $(OUTPUT)/networks.lis;
	$(APPNAME) cnetlist=$(OUTPUT)/networks.lis \
	           onet=$(OUTPUT)/Equi065S355ED10.net \
	           networkid=Equi065S355ED10 \
	           description="Messenger data test" \
	           imagetol=3000.0 \
	           > /dev/null;
	cnetbin2pvl from=$(OUTPUT)/Equi065S355ED10.net \
				   to=$(OUTPUT)/Equi065S355ED10.pvl \
				   > /dev/null;

	$(RM) $(OUTPUT)/networks.lis;
	$(RM) $(OUTPUT)/*.net
