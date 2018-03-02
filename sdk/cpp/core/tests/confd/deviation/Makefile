
usage:
	@echo "See README files for more instructions"
	@echo "make all     Build all example files"
	@echo "make clean   Remove all built and intermediary files"
	@echo "make start   Start CONFD daemon and example agent"
	@echo "make stop    Stop any CONFD daemon and example agent"

######################################################################
# Where is ConfD installed? Make sure CONFD_DIR points it out
CONFD_DIR ?= ../../../..

# Include standard ConfD build definitions and rules
include $(CONFD_DIR)/src/confd/build/include.mk

# In case CONFD_DIR is not set (correctly), this rule will trigger
$(CONFD_DIR)/src/confd/build/include.mk:
	@echo 'Where is ConfD installed? Set $$CONFD_DIR to point it out!'
	@echo ''

######################################################################
# Example specific definitions and rules

CONFD_FLAGS = --addloadpath $(CONFD_DIR)/etc/confd --addloadpath $(CONFD_DIR)/src/confd/yang
START_FLAGS ?=


all: openconfig-bgp.deviation ydktest-sanity.deviation \
	openconfig-bgp.fxs openconfig-routing-policy.fxs openconfig-policy-types.fxs openconfig-bgp-types.fxs \
	openconfig-types.fxs openconfig-bgp-operational.fxs openconfig-bgp-multiprotocol.fxs \
	ydktest-types.fxs \
		$(CDB_DIR) ssh-keydir
	@echo "Build complete"

openconfig-bgp.deviation: cisco-xr-openconfig-bgp-deviations.yang \
 		openconfig-bgp.yang openconfig-bgp-multiprotocol.yang openconfig-bgp-types.yang openconfig-policy-types.yang openconfig-routing-policy.yang
	$(CONFDC) $(FXS_WERR) $(EXTRA_LINK_FLAGS) -o $(basename $@).fxs -c $(basename $@).yang \
		--deviation $(word 1, $^)

ydktest-sanity.deviation: ydktest-sanity-deviations-types.yang ydktest-sanity-deviations-another.yang ydktest-sanity.yang
	$(CONFDC) $(FXS_WERR) $(EXTRA_LINK_FLAGS) -o $(basename $@).fxs -c $(basename $@).yang \
		--deviation $(word 1, $^) --deviation $(word 2, $^)

openconfig-bgp-multiprotocol.yang:
	cp ../../../../../../yang/deviation/openconfig-bgp-multiprotocol.yang openconfig-bgp-multiprotocol.yang

openconfig-bgp-operational.yang:
	cp ../../../../../../yang/deviation/openconfig-bgp-operational.yang openconfig-bgp-operational.yang

openconfig-bgp-types.yang:
	cp ../../../../../../yang/deviation/openconfig-bgp-types.yang openconfig-bgp-types.yang

openconfig-bgp.yang: openconfig-bgp-multiprotocol.yang openconfig-routing-policy.yang openconfig-bgp-types.yang \
		openconfig-bgp-operational.yang openconfig-types.yang openconfig-extensions.yang openconfig-policy-types.yang
	cp ../../../../../../yang/deviation/openconfig-bgp.yang openconfig-bgp.yang

openconfig-extensions.yang:
	cp ../../../../../../yang/deviation/openconfig-extensions.yang openconfig-extensions.yang

openconfig-policy-types.yang:
	cp ../../../../../../yang/deviation/openconfig-policy-types.yang openconfig-policy-types.yang

openconfig-routing-policy.yang:
	cp ../../../../../../yang/deviation/openconfig-routing-policy.yang openconfig-routing-policy.yang

openconfig-types.yang:
	cp ../../../../../../yang/deviation/openconfig-types.yang openconfig-types.yang

cisco-xr-openconfig-bgp-deviations.yang:
	cp ../../../../../../yang/deviation/cisco-xr-openconfig-bgp-deviations.yang cisco-xr-openconfig-bgp-deviations.yang

ydktest-types.yang:
	cp ../../models/ydktest-types@2016-05-23.yang ydktest-types.yang

ydktest-sanity.yang: ydktest-types.yang
	cp ../../models/ydktest-sanity@2015-11-17.yang ydktest-sanity.yang
	cp ../../models/ydktest-sanity-submodule@2016-04-25.yang ydktest-sanity-submodule.yang

ydktest-sanity-deviations-types.yang:
	cp ../../models/ydktest-sanity-deviations-types@2015-11-17.yang ydktest-sanity-deviations-types.yang

ydktest-sanity-deviations-another.yang:
	cp ../../models/ydktest-sanity-deviations-another@2015-11-17.yang ydktest-sanity-deviations-another.yang


######################################################################
clean:  iclean
	-rm -rf *_ns.py *.pyc *.yang 2> /dev/null || true

######################################################################
start:  stop start_confd

start_confd:
	$(CONFD) -c confd.conf $(CONFD_FLAGS)


######################################################################



######################################################################
stop:
	$(CONFD) --stop &> /dev/null   || true

######################################################################
