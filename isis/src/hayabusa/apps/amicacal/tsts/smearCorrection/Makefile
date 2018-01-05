# history 2017-11-28 Jeannie Backer - Updated truth data for 
#                        notSmearCorrected_nsubimages2_st_2433548889_w.cub.
#                        Cube dn differences up to 0.06 difference after update of solar flux value.
#                        (See amicacal history).  
APPNAME = amicacal

include $(ISISROOT)/make/isismake.tsts

commands:
	# Test application of smear removal for SubImageCount = 1
	$(APPNAME) \
	    FROM=$(INPUT)/st_2539482843_v.cub \
	    TO=$(OUTPUT)/smearCorrected_nsubimages1_st_2539482843_v.cub.cub \
	    nullpolarpix=no \
	    > /dev/null;

	# Test NON-application of smear removal for SubImageCount = 2
	$(APPNAME) \
	    FROM=$(INPUT)/st_2433548889_w.cub \
	    TO=$(OUTPUT)/notSmearCorrected_nsubimages2_st_2433548889_w.cub \
	    nullpolarpix=no \
	    >/dev/null;\

