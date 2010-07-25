jxmms:
	gcc -Wall -o jxmms.so -shared -O1 `pkg-config --cflags --libs gtk+-2.0` -I/usr/include/xmms/ -lnotify -lxmms jxmms.c -fPIC

install: 
	cp jxmms.so ${HOME}/.xchat2/

install-global:
	cp jxmms.so /usr/lib/xchat/plugins/

clean:
	rm jxmms.so
