APPNAME = vdcomp

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/f387a06.imq  $(OUTPUT)/f387a06.imq;
	$(APPNAME) $(OUTPUT)/f387a06.imq $(OUTPUT)/f387a06.img > /dev/null;
	vik2isis from=$(OUTPUT)/f387a06.img to=$(OUTPUT)/f387a06.cub > /dev/null;
