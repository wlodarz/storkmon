PROJ	=	storkmon
CC	=	m68k-palmos-gcc
CFLAGS  = -Iinclude -I/opt/palmdev/sdk/include \
	-I/opt/palmdev/sdk-4/include/Core/Hardware \
	-I/opt/palmdev/sdk-4/include/Dynamic \
	-I/opt/palmdev/sdk-4/include/Libraries \
	-I/opt/palmdev/sdk-4/include/Core \
	-I/opt/palmdev/sdk-4/include/Core/UI \
	-I/opt/palmdev/sdk-4/include/Core/System

$(PROJ).prc: code0001.$(PROJ).grc tfrm03e8.bin
	build-prc $(PROJ).prc "StorkMon" ReCh *.grc *.bin
	ls -l *.prc

$(PROJ):    obj/main.o obj/draw.o obj/comm.o obj/zbparse.o
	$(CC) $^ -o $@

code0001.$(PROJ).grc: $(PROJ)
	m68k-palmos-obj-res $(PROJ)

obj/draw.o:  src/draw.c include/draw.h
	$(CC) -c $< -o $@ $(CFLAGS)

obj/comm.o:  src/comm.c include/comm.h include/storkmon.h
	$(CC) -c $< -o $@ $(CFLAGS)

obj/zbparse.o:  src/zbparse.c
	$(CC) -c $< -o $@ $(CFLAGS)

obj/main.o:  src/main.c include/draw.h include/comm.h include/storkmon.h
	$(CC) -c $< -o $@ $(CFLAGS)

tfrm03e8.bin: src/$(PROJ).rcp include/$(PROJ).h src/$(PROJ).txt
	txt2bitm src/$(PROJ).txt
	pilrc src/$(PROJ).rcp

test:	test/t

test/t:
	gcc -o test/t test/main.c src/zbparse.c -Iinclude

clean:
	rm -f obj/* *.grc *.bin *.bmp $(PROJ).prc test/t

mrproper: clean
	rm -f $(PROJ)
