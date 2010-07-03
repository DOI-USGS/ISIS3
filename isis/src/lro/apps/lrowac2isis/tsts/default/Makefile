APPNAME = lrowac2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/wac0000a1c4.img \
	  TO=$(OUTPUT)/out.cub > /dev/null;
	catlab FROM=$(OUTPUT)/out.vis.even.cub \
	  > $(OUTPUT)/out.vis.even.pvl > /dev/null;
	crop FROM=$(OUTPUT)/out.vis.even.cub \
	  to=$(OUTPUT)/small.even.cub > /dev/null;
	$(RM) $(OUTPUT)/out.vis.even.cub > /dev/null;
	crop FROM=$(OUTPUT)/out.vis.odd.cub \
	  to=$(OUTPUT)/small.odd.cub > /dev/null;
	catlab FROM=$(OUTPUT)/out.vis.odd.cub \
	  > $(OUTPUT)/out.vis.odd.pvl > /dev/null;
	$(RM) $(OUTPUT)/out.vis.odd.cub > /dev/null;
