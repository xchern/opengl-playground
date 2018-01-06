str_inc=\
shader/phong.frag.inc\
shader/phong.vert.inc\


default: $(str_inc)
	cd build; make;

clean:
	rm $(str_inc)

%.inc: %
	xxd -i < $< > $@