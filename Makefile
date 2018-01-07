str_inc=\
shader/smooth.vert.inc\
shader/smooth.frag.inc\
shader/phong.frag.inc\
shader/marble.frag.inc\


default: $(str_inc)
	cd build; make;

clean:
	rm $(str_inc)

%.inc: %
	xxd -i < $< > $@