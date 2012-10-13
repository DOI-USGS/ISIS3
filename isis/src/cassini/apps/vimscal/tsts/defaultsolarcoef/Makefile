APPNAME = vimscal

V1515948001_1.ir.cal1.cub.TOLERANCE = 0.001
V1515948001_1.ir.cal2.cub.TOLERANCE = 0.001
V1515948001_1.ir.cal3.cub.TOLERANCE = 0.001
V1515948001_1.ir.cal4.cub.TOLERANCE = 0.001
V1515948001_1.ir.cal5.cub.TOLERANCE = 0.001

include $(ISISROOT)/make/isismake.tsts

# This is testing a file where the default solar remove coefficient is used because none of
#   the standard points for testing work correctly.
commands:
	$(APPNAME) from=$(INPUT)/V1515948001_1.ir.cub \
	  to=$(OUTPUT)/V1515948001_1.ir.cal1.cub UNITS=IOF \
	  IRORIGDARK=false DARK=true > /dev/null; \
	$(APPNAME) from=$(INPUT)/V1515948001_1.ir.cub \
	  to=$(OUTPUT)/V1515948001_1.ir.cal2.cub UNITS=SPECENERGY \
	  IRORIGDARK=false DARK=true > /dev/null; \
	$(APPNAME) from=$(INPUT)/V1515948001_1.ir.cub \
	  to=$(OUTPUT)/V1515948001_1.ir.cal3.cub UNITS=IOF \
	  IRORIGDARK=true DARK=true > /dev/null; \
	$(APPNAME) from=$(INPUT)/V1515948001_1.ir.cub \
	  to=$(OUTPUT)/V1515948001_1.ir.cal4.cub UNITS=IOF \
	  IRORIGDARK=true DARK=false > /dev/null; \
	$(APPNAME) from=$(INPUT)/V1515948001_1.ir.cub \
	  to=$(OUTPUT)/V1515948001_1.ir.cal5.cub UNITS=IOF \
	  IRORIGDARK=false DARK=false > /dev/null;

