APPNAME = mdisedrinfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from='$$ISISTESTDATA/isis/src/messenger/unitTestData/EW0031509051D.cub' \
	  keylist=$(INPUT)/mdispdskeys.lis to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EW0031509051D.pvl > /dev/null;
