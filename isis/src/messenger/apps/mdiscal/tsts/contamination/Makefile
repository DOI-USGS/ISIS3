APPNAME = mdiscal

include $(ISISROOT)/make/isismake.tsts

commands:
	# pre contamination iof=true
	$(APPNAME) \
	from=$(INPUT)/EW0217134599G.cub \
	to=$(OUTPUT)/pre_contamination.cub \
	iof=true > /dev/null;
	catlab \
	from=$(OUTPUT)/pre_contamination.cub \
	to=$(OUTPUT)/pre_contamination_label.pvl > /dev/null;
	# contamination day 1, iof=true
	$(APPNAME) from=$(INPUT)/EW0217136166F.cub \
	to=$(OUTPUT)/contamination_day_1.cub \
	iof=true > /dev/null;
	catlab from=$(OUTPUT)/contamination_day_1.cub \
	to=$(OUTPUT)/contamination_day_1_label.pvl > /dev/null;
	# more contamination, iof=true
	$(APPNAME) \
	from=$(INPUT)/EW0217310049F.cub \
	to=$(OUTPUT)/contamination_jun_2011_iof.cub \
	iof=true > /dev/null;
	catlab \
	from=$(OUTPUT)/contamination_jun_2011_iof.cub \
	to=$(OUTPUT)/contamination_jun_2011_iof_label.pvl > /dev/null;
	# more contamination, iof=false
	$(APPNAME) \
	from=$(INPUT)/EW0217310055G.cub \
	to=$(OUTPUT)/contamination_jun_2011_ra.cub \
	iof=false > /dev/null;
	catlab \
	from=$(OUTPUT)/contamination_jun_2011_ra.cub \
	to=$(OUTPUT)/contamination_jun_2011_ra_label.pvl > /dev/null;
	# even more contamination, iof=true
	$(APPNAME) \
	from=$(INPUT)/EW0234069364C.cub \
	to=$(OUTPUT)/contamination_jan_2012_last_day.cub \
	iof=true > /dev/null;
	catlab \
	from=$(OUTPUT)/contamination_jan_2012_last_day.cub \
	to=$(OUTPUT)/contamination_jan_2012_last_day_label.pvl > /dev/null;
	# post contamination, iof=true
	$(APPNAME) \
	from=$(INPUT)/EW0234155897G.cub \
	to=$(OUTPUT)/post_contamination.cub \
	iof=true > /dev/null;
	catlab \
	from=$(OUTPUT)/post_contamination.cub \
	to=$(OUTPUT)/post_contamination_label.pvl > /dev/null;
	# contamination day 1, iof=true, no empirical correction performed
	$(APPNAME) from=$(INPUT)/EW0217136166F.cub \
	to=$(OUTPUT)/contamination_day_1_no_ec.cub \
	iof=true \
	ecf=false > /dev/null;
	catlab from=$(OUTPUT)/contamination_day_1_no_ec.cub \
	to=$(OUTPUT)/contamination_day_1_no_ec_label.pvl > /dev/null;
	# post contamination, iof=true, no empirical correction performed
	$(APPNAME) \
	from=$(INPUT)/EW0234155897G.cub \
	to=$(OUTPUT)/post_contamination_no_ec.cub \
	iof=true \
	ecf=false > /dev/null;
	catlab \
	from=$(OUTPUT)/post_contamination_no_ec.cub \
	to=$(OUTPUT)/post_contamination_no_ec_label.pvl > /dev/null;
	# NAC contamination, iof=true, no empirical correction (because NAC)
	$(APPNAME) \
	from=$(INPUT)/EN0228199579M.cub \
	to=$(OUTPUT)/contamination_NAC.cub \
	iof=true > /dev/null;
	catlab \
	from=$(OUTPUT)/contamination_NAC.cub \
	to=$(OUTPUT)/contamination_NAC_label.pvl > /dev/null;


