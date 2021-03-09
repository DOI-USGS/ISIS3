# history 2017-11-28 Jeannie Backer - Updated truth data for corrected_iof_st_2459265790_w.cub.
#                        Cube dn differences up to 0.08 difference after update of solar flux value.
#                        (See amicacal history).  
APPNAME = amicacal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
	    FROM=$(INPUT)/st_2459265790_w.cub \
	    TO=$(OUTPUT)/corrected_iof_st_2459265790_w.cub \
	    > /dev/null;
	$(APPNAME) \
	    FROM=$(INPUT)/st_2459265790_w.cub \
	    TO=$(OUTPUT)/corrected_noiof_st_2459265790_w.cub \
	    units=dn \
	    > /dev/null;

	# Test spiceinited data
	$(APPNAME) \
	    FROM=$(INPUT)/st_2459265790_w.spice.cub \
	    TO=$(OUTPUT)/corrected_iof_st_2459265790_w.spice.cub \
	    > /dev/null;

