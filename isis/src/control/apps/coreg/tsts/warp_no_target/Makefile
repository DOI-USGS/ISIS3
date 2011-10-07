APPNAME = coreg

# Test warp option

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mal_3095LE.cub \
	  match=$(INPUT)/mal_7456LE.cub \
	  to=$(OUTPUT)/mal_3095LE_coreg_warp.cub \
	  deffile=$(INPUT)/warp_no_target.def \
	  onet=$(OUTPUT)/warp_no_target.net \
          transform=warp \
	  rows=11 columns=11 > /dev/null;

	$(RM) $(OUTPUT)/warp_no_target.net

