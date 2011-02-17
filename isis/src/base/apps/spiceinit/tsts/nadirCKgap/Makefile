APPNAME = spiceinit

labels.txt.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/m1301260.cub $(OUTPUT)/nadirTruth.cub;
	$(APPNAME) from=$(OUTPUT)/nadirTruth.cub attach=no cknadir=yes \
	tspk='$$base/kernels/spk/de405.bsp' > /dev/null;
	catlab from=$(OUTPUT)/nadirTruth.cub > $(OUTPUT)/labels.pvl;
	rm $(OUTPUT)/nadirTruth.cub;
