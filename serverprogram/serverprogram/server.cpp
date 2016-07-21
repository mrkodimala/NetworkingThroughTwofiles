#include<stdio.h>
#include<Windows.h>


struct SyncObject{
	int isOpen;
	int isWrite;
	int isRead;
	char unused[20];
};

struct Message{
	char message[1024];
};

int setServerStarted(char *filename)
{
	FILE *server;
	server = fopen(filename, "rb+");
	struct SyncObject s;
	s.isOpen = 1;
	s.isRead = 0;
	s.isWrite = 0;
	fseek(server, 0, SEEK_SET);
	fwrite(&s, sizeof(struct SyncObject), 1, server);
	fseek(server, 0, SEEK_SET);
	memset(&s, 0, sizeof(struct SyncObject));
	fread(&s, sizeof(struct SyncObject), 1, server);
	printf("sopen=%d\n", s.isOpen);
	fclose(server);
	return 1;
}

int checkIsClientStarted(char *filename)
{
	FILE *client;
	client = fopen(filename, "rb");
	struct SyncObject s;
	fread(&s, sizeof(struct SyncObject), 1, client);
	fclose(client);
	if (s.isOpen == 1)
		return 1;
	return 0;
}

int checkIsClientWrote(char *filename)
{
	FILE *client;
	client = fopen(filename, "rb");
	struct SyncObject s;
	fread(&s, sizeof(struct SyncObject), 1, client);
	fclose(client);
	if (s.isWrite == 1)
		return 1;
	return 0;
}

int setServerWrote(char *filename)
{
	FILE *server;
	server = fopen(filename, "r+b");
	struct SyncObject s;
	s.isOpen = 1;
	s.isRead = 0;
	s.isWrite = 1;
	fseek(server, 0, SEEK_SET);
	fwrite(&s, sizeof(struct SyncObject), 1, server);
	fclose(server);
	return 0;
}


int main()
{
	char server[100] = "f:\server.bin";
	char client[100] = "f:\client.bin";
	setServerStarted(server);
	int Openflag;
	int Writeflag;
	int Readflag;
	int count = 0;
	getchar();
	printf("Waiting for connection....\n");
	
	while (1)
	{
		Sleep(1000);
		if (checkIsClientStarted(client))
		{
			if (count == 0)
			{
				printf("Connected to the Client\n");
				count++;
			}
			if (checkIsClientWrote(client))
			{
				struct Message msg;
				struct SyncObject s;
				FILE *file,*serverfile;
				file = fopen(client, "r+b");
				fseek(file, 32, SEEK_SET);
				fread(&msg, sizeof(struct Message), 1, file);
				s.isRead = 0;
				s.isWrite = 0;
				printf("Client:  %s   \n", msg.message);
				printf("Enter your message:");
				gets(msg.message);
				serverfile = fopen(server, "r+b");
				fseek(serverfile, 32, SEEK_SET);
				fwrite(&msg, sizeof(struct SyncObject), 1, serverfile);
				fclose(serverfile);
				//getchar();
				setServerWrote(server);
				Sleep(100);
				fseek(file, 0, SEEK_SET);
				fwrite(&s, sizeof(struct SyncObject), 1, file);
				fclose(file);
			}
		}
	}
	getchar();
	return 0;
}