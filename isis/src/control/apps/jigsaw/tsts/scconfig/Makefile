APPNAME = jigsaw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*cub > $(OUTPUT)/cubes.lis;
	$(APPNAME) fromlist=$(OUTPUT)/cubes.lis \
	  cnet=$(INPUT)/ground.net \
	  onet=$(OUTPUT)/ground_jig.net \
	  scconfig=$(INPUT)/config.pvl \
	  bundleout_txt=no \
	  output_csv=no \
	  residuals_csv=no > /dev/null;
	$(CAT) bundleout_images.csv \
	  | perl -pe 's/(^|,|: )([^,:]+\/)([^,\/:]*\.)(net|cub)/\1\3\4/g' 2> /dev/null \
	  > $(OUTPUT)/bundleout_scconfig_images.csv;
	$(RM) $(OUTPUT)/cubes.lis > /dev/null;
	$(RM) $(OUTPUT)/*.net > /dev/null;
	$(RM) *.csv > /dev/null;
