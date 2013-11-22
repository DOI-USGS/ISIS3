APPNAME = mappt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/I00824006RDR.lev2.cub \
	  to=$(OUTPUT)/mappt.csv \
	  format=flat \
	  append=false \
          type=ground \
	  coordsys=userdefined \
	  lattype=planetographic \
	  londir=positivewest \
          londom=180 \
	  latitude=-1.527843650235 \
          longitude=7.8016320748916 \
          > /dev/null;
