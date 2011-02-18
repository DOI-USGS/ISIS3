APPNAME = vimscal

v1570021638_1.ir.iof.cal.cub.TOLERANCE = 4e-9
v1570021638_1.ir.spec.cal.cub.TOLERANCE = 7e-8

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/v1570021638_1.ir.cub \
	TO=$(OUTPUT)/v1570021638_1.ir.iof.cal.cub > /dev/null;
	$(APPNAME) FROM=$(INPUT)/v1570021638_1.vis.cub \
	TO=$(OUTPUT)/v1570021638_1.vis.iof.cal.cub > /dev/null;
	$(APPNAME) FROM=$(INPUT)/v1570021638_1.ir.cub \
	TO=$(OUTPUT)/v1570021638_1.ir.spec.cal.cub units=specenergy > /dev/null;
	$(APPNAME) FROM=$(INPUT)/v1570021638_1.vis.cub \
	TO=$(OUTPUT)/v1570021638_1.vis.spec.cal.cub units=specenergy > /dev/null;
