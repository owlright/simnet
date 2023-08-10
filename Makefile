PROJECT_NAME := simnet
PROJECT_SRC := src/simnet
#PROJECT_MAKEFILE := src/Makefile
PROJECT_TARGET := src/$(PROJECT_NAME)
PROJECT_TARGET_DBG := src/$(PROJECT_NAME)_dbg
# I don't want automatically find these files.
# PROJECT_SRC_FILES := $(shell find $(PROJECT_SRC) -name '*.cc' -or -name '*.h' -or -name '*.msg')
# check system
ifeq ($(shell uname),Darwin)
    CPU_COUNT = $(shell sysctl -n hw.logicalcpu)
    PROJECT_TARGET := src/lib$(PROJECT_NAME).dylib
    PROJECT_TARGET_DBG := src/lib$(PROJECT_NAME)_dbg.dylib
else
    CPU_COUNT = $(shell nproc)
endif

# default target
ifeq ($(MODE),debug)
	PROJECT_TARGET_DEFAULT = $(PROJECT_TARGET_DBG)
else
	PROJECT_TARGET_DEFAULT = $(PROJECT_TARGET)
endif

# targets don't have deps, list them here
.PHONY: makefiles checkmakefiles

all: checkmakefiles $(PROJECT_TARGET_DEFAULT)
	@echo 'compile ${PROJECT_TARGET_DEFAULT} finished!';\


clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '========================================================================'; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '========================================================================'; \
	echo; \
	exit 1; \
	fi

# make share library
makefiles:
	@cd src && opp_makemake --make-so -f --deep -o $(PROJECT_NAME) -O out -I.

$(PROJECT_TARGET): checkmakefiles
	@cd src && $(MAKE) -j $(CPU_COUNT)
#	touch $(PROJECT_TARGET)

$(PROJECT_TARGET_DBG): checkmakefiles
	@cd src && $(MAKE) MODE=debug -j $(CPU_COUNT)
#	touch $(PROJECT_TARGET_DBG)

# run simulation
MAKEFLAGS += --no-builtin-rules
.SECONDARY:
.SUFFIXES:

,:= ,
space:= $() $()

OPP_RUN_OPTIONS := -m \
--cmdenv-redirect-output=false \
--cmdenv-log-level=warn \
-n simulations \
-n $(PROJECT_SRC) -l $(PROJECT_TARGET)

OPP_RUN_DBG_OPTIONS := -m \
--cmdenv-redirect-output=false \
--cmdenv-log-level=warn \
--debug-on-errors=true \
-n simulations \
-n $(PROJECT_SRC) -l $(PROJECT_TARGET)

define SIM_template
$(eval SIM_INI_FILE := simulations/$(1)/omnetpp.ini)

$(1)-%: $(PROJECT_TARGET)
	opp_runall -j $(CPU_COUNT) \
	opp_run $(SIM_INI_FILE) -c $$* -u Cmdenv $(OPP_RUN_OPTIONS)

export-$(1)-%:
	opp_scavetool export -o simulations/$(1)/results/$$*.csv -F CSV-R --type vs --allow-nonmatching \
	simulations/$(1)/results/$$**.vec \
	simulations/$(1)/results/$$**.sca

cleanresults-$(1)-%:
	$(RM) \
	simulations/$(1)/results/$$**.vec \
	simulations/$(1)/results/$$**.vci \
	simulations/$(1)/results/$$**.sca

#	python3 simulations/$(1)/plot.py $$*

$(1)-%-plt:
	python3 simulations/$(1)/plot.py $$*

$(1)-%-qt: $(PROJECT_TARGET)
	opp_run $(SIM_INI_FILE) -c $$* -u Qtenv $(OPP_RUN_OPTIONS) --**.statistic-recording=false --output-scalar-file=/dev/null --output-scalar-file-append=true

$(1)-%-dbg: $(PROJECT_TARGET_DBG)
	@echo "\e[1;34m[\"$(subst $(space),\"$(,)$(space)\",$(SIM_INI_FILE) -c $$* -u Qtenv $(OPP_RUN_DBG_OPTIONS) --**.statistic-recording=false --output-scalar-file=/dev/null --output-scalar-file-append=true)\"]\e[0m"
	opp_run_dbg $(SIM_INI_FILE) -c $$* -u Qtenv $(OPP_RUN_DBG_OPTIONS) --**.statistic-recording=false --output-scalar-file=/dev/null --output-scalar-file-append=true

.PHONY: $(1)-% $(1)-%-plt $(1)-%-qt $(1)-%-dbg
endef

$(foreach p, $(shell find simulations/* -maxdepth 0 -type d ! -name __pycache__ -exec basename {} \;), \
  $(eval $(call SIM_template,$(p))) \
)
