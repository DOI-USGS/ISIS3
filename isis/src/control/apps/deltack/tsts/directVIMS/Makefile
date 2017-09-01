APPNAME  = deltack

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/v1637463908_1-ir_cal.cub $(OUTPUT);
	$(APPNAME) from=$(OUTPUT)/v1637463908_1-ir_cal.cub \
	  method=direct                   \
	  samp1=13.0                      \
	  line1=5.0                       \
	  lat1=-75.402977742875279        \
	  lon1=302.102959325449717 > /dev/null;         
	campt from=$(OUTPUT)/v1637463908_1-ir_cal.cub \
	  sample=13.0 line=5.0 \
          to=$(OUTPUT)/adjusted_campt.pvl > /dev/null;
	$(RM) $(OUTPUT)/v1637463908_1-ir_cal.cub;

	  
