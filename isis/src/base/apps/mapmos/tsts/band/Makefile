APPNAME  = mapmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(CP) $(INPUT)/bandPriorityInput.cub $(OUTPUT)/bandGreaterThanMosaic.cub; \
	$(CP) $(INPUT)/bandPriorityInput.cub $(OUTPUT)/bandLessThanMosaic.cub;    \
	$(APPNAME) from=$(INPUT)/secondInput.cub                                  \
	  mosaic=$(OUTPUT)/bandGreaterThanMosaic.cub                              \
	  priority=band                                                           \
	  type=bandnumber \
	  number=2                                                                \
	  criteria=greater > /dev/null;                                           \
	$(APPNAME) from=$(INPUT)/secondInput.cub                                  \
	  mosaic=$(OUTPUT)/bandLessThanMosaic.cub                                 \
	  priority=band                                                           \
	  number=2                                                                \
	  null=true                                                               \
	  criteria=lesser > /dev/null;
