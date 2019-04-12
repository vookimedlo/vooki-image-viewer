#
# Regular cron jobs for the vookiimageviewer package
#
0 4	* * *	root	[ -x /usr/bin/vookiimageviewer_maintenance ] && /usr/bin/vookiimageviewer_maintenance
