APPNAME = cnetcombinept

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.net > $(OUTPUT)/networks.lis;
	$(APPNAME) cnetlist=$(OUTPUT)/networks.lis \
	           onet=$(OUTPUT)/Equi065S355ED10.net \
	           networkid=Equi065S355ED10 \
	           description="Messenger data test" \
	           imagetol=1000.0 \
	           setaprioribest=true \
	           > /dev/null;

	$(RM) $(OUTPUT)/networks.lis;
