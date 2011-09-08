APPNAME = phoemplocal

include $(ISISROOT)/make/isismake.tsts

commands: 
	$(APPNAME) to=$(OUTPUT)/henyey_lunlam_noatm.pvl append=no wh=.05 hh=.074 \
	  b0=3.18 theta=20 hg1=-.4 hg2=0 model=lunarlambert \
	  emission=0 phase=45 incidence=45 rms_slope=10 \
	  seed_number=0 seed=yes > /dev/null;
	cat $(OUTPUT)/henyey_lunlam_noatm.pvl | \
	  sed 's/\([0-9]*\.[0-9]\{5\}\)[0-9]*/\1/g' \
	  > $(OUTPUT)/temp.txt;
	$(MV) $(OUTPUT)/temp.txt $(OUTPUT)/henyey_lunlam_noatm.pvl;
