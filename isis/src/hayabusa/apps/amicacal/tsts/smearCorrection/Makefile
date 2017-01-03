APPNAME = amicacal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) \
	FROM=$(INPUT)/st_2433548889_w.cub \
	TO=$(OUTPUT)/smearCorrected_nsubimages2_st_2433548889_w.cub \
	applysmear=yes nullpolarpix=no >/dev/null;\
	$(APPNAME) \
	FROM=$(INPUT)/st_2539482843_v.cub \
	TO=$(OUTPUT)/smearCorrected_nsubimages1_st_2539482843_v.cub.cub \
	nullpolarpix=no > /dev/null;
