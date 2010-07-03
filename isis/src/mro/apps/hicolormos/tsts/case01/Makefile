APPNAME = hicolormos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from1=$(INPUT)/test_hicubeitOUT_sinu_070419.cub \
	           to=$(OUTPUT)/test_hicolormos.cub > /dev/null;
	catlab from=$(OUTPUT)/test_hicolormos.cub > $(OUTPUT)/hicolormos.pvl;  
