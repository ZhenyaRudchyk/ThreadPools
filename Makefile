.PHONY: all clean

BIN_DIR = ${REST_SERVICE_ROOT}/bin

EXECUTABLE = ${BIN_DIR}/rest_service

OBJS_OUTDIR = ${BIN_DIR}/objs

# headers lookup
#INCLUDES =

# sources lookup
#VPATH =

LIBS = -lpthread


SOURCES_CPP = \
	main.cpp \
	DynamicPool.cpp \
	

OBJECTS_CPP = $(SOURCES_CPP:%.cpp=$(OBJS_OUTDIR)/%.o)


all: ${EXECUTABLE}

${EXECUTABLE}: ${OBJECTS_CPP}
	$(info Linking executable $(@F) ...)
	@${CXX} ${OBJECTS_CPP} ${LIBS} -o $@
	$(info Executable $(@F) created !!!)

${OBJS_OUTDIR}/%.o: %.cpp
	@mkdir -p '$(@D)'
	@${CXX} ${CXXFLAGS} ${INCLUDES} -g -c $< -o $@
	$(info $(<F) --> $(@F))

clean:
	rm -rf ${EXECUTABLE} ${OBJS_OUTDIR}/*