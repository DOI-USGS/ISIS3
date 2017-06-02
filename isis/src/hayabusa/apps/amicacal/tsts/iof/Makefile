APPNAME = amicacal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
        FROM=$(INPUT)/st_2459265790_w.cub \
        TO=$(OUTPUT)/corrected_iof_st_2459265790_w.cub > /dev/null;\
        $(APPNAME) \
        FROM=$(INPUT)/st_2459265790_w.cub \
        TO=$(OUTPUT)/corrected_noiof_st_2459265790_w.cub \
        units=dn > /dev/null;
