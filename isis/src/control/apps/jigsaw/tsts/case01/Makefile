APPNAME = jigsaw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/*.cub $(OUTPUT) > /dev/null;
	$(LS) -1 $(OUTPUT)/*.cub > $(OUTPUT)/cube.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cube.lis \
	cnet=$(INPUT)/redPntreg.net \
	onet=$(OUTPUT)/case01OutNet.pvl \
	observations=yes \
	method=qrd \
	tol=1.64742389 \
	solvedegree=3 \
	camsolve=all \
	twist=no | grep -v "100% Processed" | \
	grep -v "jigsaw" > $(OUTPUT)/case01OutLog.pvl;
	$(RM) $(OUTPUT)/cube.lis > /dev/null;
	cathist from=$(OUTPUT)/PSP_002733_1880_RED4.crop.cub > $(OUTPUT)/PSP4.pvl;										    
	cathist from=$(OUTPUT)/PSP_002733_1880_RED5.crop.cub > $(OUTPUT)/PSP5.pvl;
