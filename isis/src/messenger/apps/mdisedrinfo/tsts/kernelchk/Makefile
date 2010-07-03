APPNAME = mdisedrinfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from='$$messenger/testData/EN0089576657M.IMG' \
	  keylist=$(INPUT)/mdispdskeys.lis \
	  to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EN0089576657M.pvl > /dev/null;
	$(APPNAME) from='$$messenger/testData/EW0031592574E.IMG' \
	  to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EW0031592574E.pvl  > /dev/null;
	$(APPNAME) from='$$messenger/testData/EW0089570936I.IMG' \
	  to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EW0089570936I.pvl > /dev/null;
	$(APPNAME) from='$$messenger/testData/EW0108827588A.IMG' \
	  to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EW0108827588A.pvl > /dev/null;
	$(APPNAME) from='$$messenger/testData/EN0108828426M.IMG' \
	  to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EN0108828426M.pvl > /dev/null;
