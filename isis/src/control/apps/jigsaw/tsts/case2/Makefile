APPNAME = jigsaw
#This test exercises the held image option, SVD solution method, and spsolve=position.

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > $(OUTPUT)/cub.lis;
	$(LS) -1 $(INPUT)/e0400710.lev1.cub > $(OUTPUT)/held.lis;

	$(APPNAME) fromlist=$(OUTPUT)/cub.lis  \
	heldlist=$(OUTPUT)/held.lis  \
	cnet=$(INPUT)/mocVik.net \
	onet=$(OUTPUT)/case2OutNet.pvl \
	maxits=10 \
	tol=0.908  \
	update=no \
	spsolve=position | grep -v "100% Processed" | \
	grep -v "jigsaw" > $(OUTPUT)/case2OutLog.pvl;
	$(RM) $(OUTPUT)/cub.lis $(OUTPUT)/held.lis > /dev/null;
