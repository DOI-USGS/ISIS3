APPNAME = mdiscal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/EW0217134599G.cub to=$(OUTPUT)/pre_contamination.cub iof=true > /dev/null;
	catlab FROM=$(OUTPUT)/pre_contamination.cub > $(OUTPUT)/pre_contamination_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0217136166F.cub to=$(OUTPUT)/contamination_day_1.cub iof=true > /dev/null;
	catlab FROM=$(OUTPUT)/contamination_day_1.cub > $(OUTPUT)/contamination_day_1_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0217310049F.cub to=$(OUTPUT)/contamination_jun_2011_iof.cub iof=true > /dev/null;
	catlab FROM=$(OUTPUT)/contamination_jun_2011_iof.cub > $(OUTPUT)/contamination_jun_2011_iof_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0217310055G.cub to=$(OUTPUT)/contamination_jun_2011_ra.cub iof=false > /dev/null;
	catlab FROM=$(OUTPUT)/contamination_jun_2011_ra.cub > $(OUTPUT)/contamination_jun_2011_ra_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0234069364C.cub to=$(OUTPUT)/contamination_jan_2012_last_day.cub iof=true > /dev/null;
	catlab FROM=$(OUTPUT)/contamination_jan_2012_last_day.cub > $(OUTPUT)/contamination_jan_2012_last_day_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EW0234155897G.cub to=$(OUTPUT)/post_contamination.cub iof=true > /dev/null;
	catlab FROM=$(OUTPUT)/post_contamination.cub > $(OUTPUT)/post_contamination_label.pvl;
	$(APPNAME) FROM=$(INPUT)/EN0228199579M.cub to=$(OUTPUT)/contamination_NAC.cub iof=true > /dev/null;
	catlab FROM=$(OUTPUT)/contamination_NAC.cub > $(OUTPUT)/contamination_NAC_label.pvl;


