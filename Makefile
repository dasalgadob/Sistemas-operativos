all:
	gcc p2-dogServer.c -o p2-dogServer -lpthread
	gcc p2-dogClient.c -o p2-dogClient 