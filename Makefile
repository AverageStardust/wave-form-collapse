dist/cmodule.js: src/cmain.c
	emcc -o public/cmain.js $^ -O3 -msimd128 -std=gnu11 -Wall -s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']" -s ASSERTIONS=1