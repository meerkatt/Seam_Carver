CXX=clang++
CXX_FLAGS=-std=c++20 -stdlib=libc++ -g -O0 -Wall -Wextra -Werror -Iincludes/

.PHONY: seam_carver test clean

seam_carver: src/main.cc src/seam_carver.cc src/image_ppm.cc includes/image_ppm.hpp includes/seam_carver.hpp
	${CXX} ${CXX_FLAGS} src/main.cc src/seam_carver.cc src/image_ppm.cc -o bin/seam_carver

test: tests/test.cc src/seam_carver.cc src/image_ppm.cc includes/image_ppm.hpp includes/seam_carver.hpp
	${CXX} ${CXX_FLAGS} tests/test.cc src/seam_carver.cc src/image_ppm.cc -o bin/test

clean:
	rm bin/seam_carver bin/test
