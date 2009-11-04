xmms-control:
#	gcc -Wall -o xmms-control.so -shared -O1 `gtk-config --cflags` `glib-config --cflags` -I/usr/include/xmms/ -lxmms -lpthread xmms-control.c -fPIC
	gcc -Wall -o jxmms `gtk-config --cflags` `glib-config --cflags` -I/usr/include/xmms/ -lxmms -lpthread jxmms.c -fPIC

install: 
	cp jxmms.so ${HOME}/.xchat2/

install-global:
	cp jxmms.so /usr/lib/xchat/plugins/
