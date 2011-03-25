APPNAME = jigsaw
#This test exercises the held image option, SVD solution method, and spsolve=position.

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cub.lis;
	$(LS) -1 $(INPUT)/e0400710.lev1.cub > $(OUTPUT)/held.lis;

	$(APPNAME) fromlist=$(OUTPUT)/cub.lis  \
	heldlist=$(OUTPUT)/held.lis  \
	cnet=$(INPUT)/mocVik.net \
	onet=$(OUTPUT)/case2OutNet.bin \
	maxits=10 \
	method=sparse \
	sigma0=1.e-5  \
	update=no \
	spsolve=position | grep -v "100% Processed" | \
	grep -v "jigsaw" > $(OUTPUT)/case2OutLog.pvl;
	cnetbin2pvl from=$(OUTPUT)/case2OutNet.bin to=$(OUTPUT)/case2OutNet.pvl > /dev/null;
	$(RM) $(OUTPUT)/cub.lis $(OUTPUT)/held.lis $(OUTPUT)/case2OutNet.bin > /dev/null;
