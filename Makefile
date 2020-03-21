CXX=g++
AR=ar
CXXFLAGS=-W -Wall
LDFLAGS=

SRC=ADS1X15_TLA2024.cpp
OUT=libads1x15_tla2024.a
OBJ=$(SRC:.cpp=.o)

all: examples

lib: $(OUT)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(AR) rcs $(OUT) $(OBJ)

examples: $(OUT)
	@(cd examples/multiDeviceOnSameBus && $(MAKE))
	@(cd examples/singleEnded && $(MAKE))
	@(cd examples/differential && $(MAKE))
	@(cd examples/comparator && $(MAKE))

help:
	@echo "Usage: all, examples, lib, clean, mrproper"

clean:
	rm -f $(OBJ)
	@(cd examples/multiDeviceOnSameBus && $(MAKE) $@)
	@(cd examples/singleEnded && $(MAKE) $@)
	@(cd examples/differential && $(MAKE) $@)
	@(cd examples/comparator && $(MAKE) $@)

mrproper: clean
	rm -f $(OUT)
	@(cd examples/multiDeviceOnSameBus && $(MAKE) $@)
	@(cd examples/singleEnded && $(MAKE) $@)
	@(cd examples/differential && $(MAKE) $@)
	@(cd examples/comparator && $(MAKE) $@)