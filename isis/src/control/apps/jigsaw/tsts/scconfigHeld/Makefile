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
	$(CAT) bundleout_images_MARS_ODYSSEY_THEMIS_IR.csv \
	   | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2> /dev/null \
	   > $(OUTPUT)/bundleout_scconfig_images_themis.csv;
	$(CAT) bundleout_images_VIKING_ORBITER_1_VISUAL_IMAGING_SUBSYSTEM_CAMERA_B.csv \
	   | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2> /dev/null \
	   > $(OUTPUT)/bundleout_scconfig_images_viking_1.csv;
	$(CAT) bundleout_images_VIKING_ORBITER_2_VISUAL_IMAGING_SUBSYSTEM_CAMERA_A.csv \
          | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2> /dev/null \
          > $(OUTPUT)/bundleout_scconfig_images_viking_2.csv;
	$(CAT) bundleout_images_held.csv \
          | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2> /dev/null \
          > $(OUTPUT)/bundleout_scconfig_images_held.csv;
	$(RM) $(OUTPUT)/cubes.lis > /dev/null;
	$(RM) $(OUTPUT)/hold.lis > /dev/null;
	$(RM) $(OUTPUT)/*.net > /dev/null;
	$(RM) *.csv > /dev/null;
