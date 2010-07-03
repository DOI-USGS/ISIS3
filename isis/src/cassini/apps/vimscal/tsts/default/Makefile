APPNAME = vimscal

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
