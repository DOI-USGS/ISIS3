APPNAME = ascii2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	# Test an input file with not enough data
	if [ `$(APPNAME) \
	  from=$(INPUT)/ascii2isis_input.txt \
	  to=$(OUTPUT)/temp.cub >& $(OUTPUT)/temp.txt \
	  order=bsq \
	  lines=100 \
	  samples=100 \
	  bands=100 \
	  skip=0` ]; then \
	  true; \
	fi;
	$(SED) 's/\[.*\/\(ascii2isis_input.txt\)\]/[\1]/' < $(OUTPUT)/temp.txt > $(OUTPUT)/not_enough_data.txt;
	$(RM) $(OUTPUT)/temp.txt $(OUTPUT)/temp.cub > /dev/null;
