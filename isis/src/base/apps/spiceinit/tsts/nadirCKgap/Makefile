APPNAME = spiceinit

labels.txt.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/m1301260.cub $(OUTPUT)/nadirTruth.cub;
	$(APPNAME) from=$(OUTPUT)/nadirTruth.cub attach=no cknadir=yes \
	tspk='$$base/kernels/spk/de405.bsp' > /dev/null;
	catlab from=$(OUTPUT)/nadirTruth.cub > $(OUTPUT)/labels.txt;
	$(APPNAME) from=$(OUTPUT)/nadirTruth.cub attach=no \
	tspk='$$base/kernels/spk/de405.bsp' >& $(OUTPUT)/error.txt || true;
	rm $(OUTPUT)/nadirTruth.cub;
