APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) cnet=$(INPUT)/cnet.net onet=$(OUTPUT)/newNet.net \
	networkid=new description=new prefix=$(OUTPUT)/out \
	noignore=true nomeasureless=true > /dev/null;
