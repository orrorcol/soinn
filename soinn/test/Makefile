cc=g++
#wall=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused
wall=
marco=-UDDUCH -Dno_debug_ -DUSE_THREAD -DDTYPE=float -DDDIM=2
cppflags=$(marco) -std=c++14 -Ofast -pthread $(wall)
h_path=-I../

test_soinn : test_soinn.cpp 
	$(cc) $(cppflags) $(h_path) $^ -o $@

all : test_soinn

clean :
	rm test_soinn
