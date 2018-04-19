#Student Name: Yue (Frieda) Tang
#Student ID: V00796163

include ../Makefile.config

.PHONY : clean

all: encode_mesh decode_mesh

CXX = g++

# NOTE: Frieda the leak and address sanitizers are causing the decoder to fail.
SANITIZER_CXXFLAGS += -fsanitize=leak # FAILING
SANITIZER_CXXFLAGS += -fsanitize=address # FAILING
SANITIZER_CXXFLAGS += -fsanitize=undefined
SANITIZER_CXXFLAGS += -fsanitize=null
SANITIZER_CXXFLAGS += -fsanitize=return
SANITIZER_CXXFLAGS += -fsanitize=signed-integer-overflow
SANITIZER_CXXFLAGS += -fsanitize=bounds
SANITIZER_CXXFLAGS += -fsanitize=float-divide-by-zero
SANITIZER_CXXFLAGS += -ftrapv
#OTHER_CXXFLAGS += $(SANITIZER_CXXFLAGS)

OPT_CXXFLAGS = -O3
#OPT_CXXFLAGS = -O3 -NDEBUG
#OPT_CXXFLAGS = -O0

# version of C++ standard
STD_CXXFLAGS = -std=c++14

# base compiler flags
BASE_CXXFLAGS = -g $(OPT_CXXFLAGS)

# compiler flags for CGAL
CGAL_CXXFLAGS = -frounding-math
SPL_CXXFLAGS = -I$(SPL_TOPDIR)/include

# linker flags for CGAL
CGAL_LDFLAGS = -lCGAL -lgmp
SPL_LDFLAGS = -L$(SPL_TOPDIR)/lib -lSPL

CXXFLAGS = $(BASE_CXXFLAGS) $(STD_CXXFLAGS) \
  $(SPL_CXXFLAGS) $(CGAL_CXXFLAGS) $(OTHER_CXXFLAGS)
LDFLAGS = $(CGAL_LDFLAGS) $(SPL_LDFLAGS)

encode_mesh: encode_mesh.o encoder.o Utility.o Context_selector.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
    
decode_mesh: decode_mesh.o decoder.o Utility.o Context_selector.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o encode_mesh decode_mesh
