APPNAME = rolo2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mm185801.alx \
	  to=$(OUTPUT)/mm185801.cub > /dev/null;
	  catlab from=$(OUTPUT)/mm185801.cub >& $(OUTPUT)/labels.pvl;
