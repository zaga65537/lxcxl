out = lxcxl.so
o_files = prob_utils.o gp_utils.o utils.o
h_files = prob_utils.h gp_utils.h utils.h
debug_o = debug_utils.o $(o_files)
debug_h = debug_utils.h
$(out): $(o_files)
	cc -shared -fPIC -o $(out) $(o_files)
$(o_files): $(h_files)


.PHONY: clean clean_py debug clean_c

clean: clean_c clean_py

debug: debug_o
	cc -shared -fPIC -o $(out) $(debug_o)

clean_c:
	rm -r *.so *.o

clean_py:
	rm -r ./py/*.pyc ./py/out*.*
