all : gamshighs

gamshighs : main.o gamshighs.o gmomcc.o gevmcc.o optcc.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o gamshighs

%.c : gams/apifiles/C/api/%.c
	cp $< $@

IFLAGS = -Igams/apifiles/C/api -Ihighs/include
WFLAGS = -Wall -Wextra -Wno-unused-parameter
CFLAGS = $(IFLAGS) $(WFLAGS) -g -std=c99
CXXFLAGS = $(IFLAGS) $(WFLAGS) -g -std=c++11

LDFLAGS = -ldl -Wl,-rpath,\$$ORIGIN -Wl,-rpath,$(realpath gams)
