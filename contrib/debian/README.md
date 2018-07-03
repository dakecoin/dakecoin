
Debian
====================
This directory contains files used to package DakeCoind/DakeCoin-qt
for Debian-based Linux systems. If you compile DakeCoind/DakeCoin-qt yourself, there are some useful files here.

## DakeCoin: URI support ##


DakeCoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install DakeCoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your DakeCoin-qt binary to `/usr/bin`
and the `../../share/pixmaps/DakeCoin128.png` to `/usr/share/pixmaps`

DakeCoin-qt.protocol (KDE)

