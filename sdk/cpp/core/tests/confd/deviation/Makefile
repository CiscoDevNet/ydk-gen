
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


all:  bgp.deviation routing-policy.deviation bgp-policy.deviation ydktest-sanity.deviation \
		policy-types.fxs bgp-policy.fxs bgp-types.fxs bgp-operational.fxs bgp-multiprotocol.fxs \
		ydktest-types.fxs \
		$(CDB_DIR) ssh-keydir
	@echo "Build complete"

bgp.deviation: cisco-xr-bgp-deviations.yang \
 		bgp.yang bgp-multiprotocol.yang bgp-operational.yang bgp-types.yang policy-types.yang routing-policy.yang
	$(CONFDC) $(FXS_WERR) $(EXTRA_LINK_FLAGS) -o $(basename $@).fxs -c $(basename $@).yang \
		--deviation $(word 1, $^)

routing-policy.deviation:  cisco-xr-bgp-policy-deviations.yang cisco-xr-routing-policy-deviations.yang \
		bgp-policy.yang policy-types.yang routing-policy.yang
	$(CONFDC) $(FXS_WERR) $(EXTRA_LINK_FLAGS) -o $(basename $@).fxs -c $(basename $@).yang \
		--deviation $(word 1, $^) --deviation $(word 2, $^)

bgp-policy.deviation: cisco-xr-bgp-policy-deviations.yang \
		bgp-policy.yang bgp-types.yang policy-types.yang routing-policy.yang
	$(CONFDC) $(FXS_WERR) $(EXTRA_LINK_FLAGS) -o $(basename $@).fxs -c $(basename $@).yang \
		--deviation $(word 1, $^)

ydktest-sanity.deviation: ydktest-sanity-deviations-types.yang ydktest-sanity-deviations-another.yang \
		ydktest-sanity.yang
	$(CONFDC) $(FXS_WERR) $(EXTRA_LINK_FLAGS) -o $(basename $@).fxs -c $(basename $@).yang \
		--deviation $(word 1, $^) --deviation $(word 2, $^)

bgp.yang:
	cp ../../../../../../yang/deviation/bgp.yang bgp.yang

bgp-multiprotocol.yang:
	cp ../../../../../../yang/deviation/bgp-multiprotocol.yang bgp-multiprotocol.yang

bgp-operational.yang:
	cp ../../../../../../yang/deviation/bgp-operational.yang bgp-operational.yang

bgp-policy.yang:
	cp ../../../../../../yang/deviation/bgp-policy.yang bgp-policy.yang

bgp-types.yang:
	cp ../../../../../../yang/deviation/bgp-types.yang bgp-types.yang

cisco-xr-bgp-deviations.yang:
	cp ../../../../../../yang/deviation/cisco-xr-bgp-deviations.yang cisco-xr-bgp-deviations.yang

cisco-xr-bgp-policy-deviations.yang:
	cp ../../../../../../yang/deviation/cisco-xr-bgp-policy-deviations.yang cisco-xr-bgp-policy-deviations.yang

cisco-xr-routing-policy-deviations.yang:
	cp ../../../../../../yang/deviation/cisco-xr-routing-policy-deviations.yang cisco-xr-routing-policy-deviations.yang

policy-types.yang:
	cp ../../../../../../yang/deviation/policy-types.yang policy-types.yang

routing-policy.yang:
	cp ../../../../../../yang/deviation/routing-policy.yang routing-policy.yang

ydktest-types.yang:
	cp ../../../../../../yang/ydktest/ydktest-types@2016-05-23.yang ydktest-types.yang

ydktest-sanity.yang: ydktest-types.yang
	cp ../../../../../../yang/ydktest/ydktest-sanity@2015-11-17.yang ydktest-sanity.yang
	cp ../../../../../../yang/ydktest/ydktest-sanity-submodule@2016-04-25.yang ydktest-sanity-submodule.yang

ydktest-sanity-deviations-types.yang:
	cp ../../../../../../yang/ydktest/ydktest-sanity-deviations-types@2015-11-17.yang ydktest-sanity-deviations-types.yang

ydktest-sanity-deviations-another.yang:
	cp ../../../../../../yang/ydktest/ydktest-sanity-deviations-another@2015-11-17.yang ydktest-sanity-deviations-another.yang



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
    ### Killing any confd daemon or DHCP confd agents
	$(CONFD) --stop &> /dev/null   || true

######################################################################
