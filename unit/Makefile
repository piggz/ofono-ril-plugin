# -*- Mode: makefile-gmake -*-

all:
%:
	@$(MAKE) -C ril_ecclist $*
	@$(MAKE) -C ril_config $*
	@$(MAKE) -C ril_util $*
	@$(MAKE) -C ril_vendor $*

clean: unitclean
	rm -f *~
	rm -f common/*~
	rm -f coverage/*~
	rm -f coverage/*.gcov
	rm -fr coverage/report
