
Debian
====================
This directory contains files used to package groomd/groom-qt
for Debian-based Linux systems. If you compile groomd/groom-qt yourself, there are some useful files here.

## groom: URI support ##


groom-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install groom-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your groom-qt binary to `/usr/bin`
and the `../../share/pixmaps/groom128.png` to `/usr/share/pixmaps`

groom-qt.protocol (KDE)

