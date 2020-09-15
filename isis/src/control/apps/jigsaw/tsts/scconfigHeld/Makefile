APPNAME = jigsaw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*cub > $(OUTPUT)/cubes.lis;
	$(LS) $(INPUT)/I28234014RDR.cub > $(OUTPUT)/hold.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cubes.lis \
	  heldlist=$(OUTPUT)/hold.lis \
	  cnet=$(INPUT)/themis_dayir_VO_arcadia_extract_hand.net \
	  onet=$(OUTPUT)/themis_dayir_VO_arcadia_extract_hand_jig.net \
	  scconfig=$(INPUT)/themis_vo.pvl \
	  bundleout_txt=no \
	  output_csv=no \
	  residuals_csv=no > /dev/null;
	$(CAT) bundleout_images.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2> /dev/null \
	  > $(OUTPUT)/bundleout_scconfig_images.csv;
	$(RM) $(OUTPUT)/cubes.lis > /dev/null;
	$(RM) $(OUTPUT)/hold.lis > /dev/null;
	$(RM) $(OUTPUT)/*.net > /dev/null;
	$(RM) *.csv > /dev/null;
