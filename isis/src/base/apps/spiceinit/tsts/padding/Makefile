APPNAME = spiceinit

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/v1545949478_1.cub $(OUTPUT)/v1545949478_1.cub;
	$(APPNAME) FROM=$(OUTPUT)/v1545949478_1.cub \
	STARTPAD=1.1 ENDPAD=0.5 ATTACH=FALSE \
	fk='$$cassini/kernels/fk/cas_v40_usgs.tf' > /dev/null;
	catlab FROM=$(OUTPUT)/v1545949478_1.cub > $(OUTPUT)/labels.pvl;
	$(APPNAME) FROM=$(OUTPUT)/v1545949478_1.cub \
	STARTPAD=1.1 ENDPAD=0.5 ATTACH=FALSE \
	sclk='$$cassini/kernels/sclk/cas00132.tsc' \
	fk='$$cassini/kernels/fk/cas_v40_usgs.tf' > /dev/null;
	catlab FROM=$(OUTPUT)/v1545949478_1.cub > $(OUTPUT)/labelsTwice.pvl;
	rm $(OUTPUT)/v1545949478_1.cub;
