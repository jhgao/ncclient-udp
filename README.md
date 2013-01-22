ncclient-udp
============

simple Qt client using UDP to transfer data

NOTICE:
	member of DataHandler and Connections Classes should always
	be pointer, in order to work normally in case being moved to
	other QThread.