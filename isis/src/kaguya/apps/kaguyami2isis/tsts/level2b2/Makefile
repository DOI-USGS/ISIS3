APPNAME = kaguyami2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	# Visible
	$(APPNAME) from=$(INPUT)/MNA_2B2_01_01228N602E1354.lbl \
	  to=$(OUTPUT)/MNA_2B2_01_01228N602E1354.cub > /dev/null;
	catlab from=$(OUTPUT)/MNA_2B2_01_01228N602E1354.cub \
	  to=$(OUTPUT)/MNA_2B2_01_01228N602E1354.pvl > /dev/null;

        # IR
	$(APPNAME) from=$(INPUT)/MVA_2B2_01_01228N608E1354.lbl \
          to=$(OUTPUT)/MVA_2B2_01_01228N608E1354.cub > /dev/null;
	catlab from=$(OUTPUT)/MVA_2B2_01_01228N608E1354.cub \
	  to=$(OUTPUT)/MVA_2B2_01_01228N608E1354.pvl > /dev/null;


