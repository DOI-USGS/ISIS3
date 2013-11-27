APPNAME = mappt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/I00824006RDR.lev2.cub \
	  to=$(OUTPUT)/tmp.csv \
	  format=flat \
	  append=false \
	  type=ground \
	  coordsys=userdefined \
	  lattype=planetographic \
	  londir=positivewest \
	  londom=180 \
	  latitude=-1.527843650235 \
	  longitude=7.8016320748916 \
	  > /dev/null; \
	  `sed 's/\(^Filename\,\)//g' $(OUTPUT)/tmp.csv > $(OUTPUT)/tmp2.csv`; \
	  `sed 's/\(^.*\/I00824006RDR\.lev2\.cub\,\)//g' $(OUTPUT)/tmp2.csv > $(OUTPUT)/mappt.csv` \
	  $(RM) $(OUTPUT)/tmp.csv;                                                             \
	  $(RM) $(OUTPUT)/tmp2.csv;
