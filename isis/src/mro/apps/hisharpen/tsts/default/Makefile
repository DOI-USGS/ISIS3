APPNAME = hisharpen

ir.cub.TOLERANCE = 9.8e-4
bg.cub.TOLERANCE = 4.9e-4

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/PSP_008530_1560_BG12_0.cub TO=$(OUTPUT)/bg.cub > /dev/null;
	$(APPNAME) FROM=$(INPUT)/PSP_008530_1560_IR10_0.cub TO=$(OUTPUT)/ir.cub > /dev/null;
