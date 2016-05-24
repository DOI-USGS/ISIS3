APPNAME = coreg

# Test warp option

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/0027399_crop_equi.cub \
	  match=$(INPUT)/0166613_crop_equi.cub \
	  deffile=$(INPUT)/warp.def \
	  to=$(OUTPUT)/warp.cub \
	  onet=$(OUTPUT)/warp.net \
      transform=warp \
	  interp=cubic > /dev/null;

	$(RM) $(OUTPUT)/warp.net

