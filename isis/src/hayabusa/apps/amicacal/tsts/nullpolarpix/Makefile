APPNAME = amicacal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
        FROM=$(INPUT)/st_2459265790_w.cub \
        TO=$(OUTPUT)/corrected_polarpixnulled_st_2459265790_w.cub >/dev/null;\
        $(APPNAME) \
        FROM=$(INPUT)/st_2459265790_w.cub \
        TO=$(OUTPUT)/corrected_polarpixnotnulled_st_2459265790_w.cub \
        nullpolarpix=no > /dev/null;
