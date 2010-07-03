APPNAME = mdisedrinfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from='$$messenger/testData/EN0089576657M.cub' \
	  keylist=$(INPUT)/mdispdskeys.lis \
	  to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EN0089576657M.pvl > /dev/null;
	$(APPNAME) from='$$messenger/testData/EW0031592574E.cub' \
	  to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EW0031592574E.pvl  > /dev/null;
	$(APPNAME) from='$$messenger/testData/EW0061073619F.IMG' \
	  to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EW0061073619F.pvl > /dev/null;
	$(APPNAME) from='$$messenger/testData/EW0089570936I.cub' \
	  to=$(OUTPUT)/mdisedrinfo.txt \
	  pvl=$(OUTPUT)/EW0089570936I.pvl > /dev/null;
