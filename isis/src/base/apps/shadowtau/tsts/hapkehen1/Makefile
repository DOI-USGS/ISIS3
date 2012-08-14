APPNAME = shadowtau

include $(ISISROOT)/make/isismake.tsts

# This runs the program and strips accuracy from the 2nd to last column of the
#   816A02 row (removes 1 place).
commands:
	$(APPNAME) from=$(INPUT)/TAU_INPUT_A.LIS to=$(OUTPUT)/hapkehen1.txt \
	phtname=hapkehen wh=0.52 hh=0.17 b0=0.025 theta=30 hg1=-0.213 hg2=0 \
	zerob0standard=true atmname=hapkeatm1 wha=0.95 hga=0.68 hnorm=0.003 \
	> /dev/null;                                                        \
	$(CAT) $(OUTPUT)/hapkehen1.txt | sed 's/\(816A02,.*\)\(35312\)\([0-9]\)/\1\2/' \
	  > $(OUTPUT)/hapkehen1.sed.txt;                                                         \
	$(MV) $(OUTPUT)/hapkehen1.sed.txt $(OUTPUT)/hapkehen1.txt;

