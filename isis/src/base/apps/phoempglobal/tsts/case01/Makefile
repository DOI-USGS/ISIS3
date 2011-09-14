APPNAME = phoempglobal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) to=$(OUTPUT)/henyey_minnaert_noatm.pvl phtname=hapkehen \
	  wh=0.05 hh=0.074 b0=3.18 theta=20 hg1=-.4 hg2=0 model=minnaert \
	  > /dev/null;
	cat $(OUTPUT)/henyey_minnaert_noatm.pvl | \
	  sed 's/\([0-9]*\.[0-9]\{5\}\)[0-9]*/\1/g' \
          > $(OUTPUT)/temp.txt;
	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/henyey_minnaert_noatm.pvl;
