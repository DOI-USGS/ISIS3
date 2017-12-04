# history 2017-11-28 Jeannie Backer - Updated truth data for 
#                        corrected_polarpixnulled_st_2459265790_w.cub.
#                        and corrected_polarpixnotnulled_st_2459265790_w.cub.
#                        Cube dn differences up to 0.08 difference after update of solar flux value.
#                        (See amicacal history).
APPNAME = amicacal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
	    FROM=$(INPUT)/st_2459265790_w.cub \
	    TO=$(OUTPUT)/corrected_polarpixnulled_st_2459265790_w.cub \
	    >/dev/null;
	$(APPNAME) \
	    FROM=$(INPUT)/st_2459265790_w.cub \
	    TO=$(OUTPUT)/corrected_polarpixnotnulled_st_2459265790_w.cub \
	    nullpolarpix=no \
	    > /dev/null;
