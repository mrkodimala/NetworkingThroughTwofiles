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

int startClientServer(char *filename)
{
	FILE *server;
	server = fopen(filename, "rb+");
	struct SyncObject s;
	s.isOpen = 1;
	s.isRead = 0;
	s.isWrite = 0;
	fseek(server, 0, SEEK_SET);
	fwrite(&s, sizeof(struct SyncObject), 1, server);
	fclose(server);
	return 1;
}

int checkIsServerStarted(char *filename)
{
	FILE *server;
	server = fopen(filename, "rb");
	struct SyncObject s;
	fseek(server, 0, SEEK_SET);
	fread(&s, sizeof(struct SyncObject), 1, server);
	fclose(server);
	//printf("sopen=%d\n", s.isOpen);
	if (s.isOpen == 1)
		return 1;
	return 0;
}

int setClientWrote(char *filename)
{
	FILE *client;
	client = fopen(filename, "rb+");
	struct SyncObject s;
	s.isOpen = 1;
	s.isRead = 1;
	s.isWrite = 1;
	fseek(client, 0, SEEK_SET);
	fwrite(&s, sizeof(struct SyncObject), 1, client);
	fclose(client);
	return 0;
}

int checkIfServerRead(char *filename)
{
	FILE *client;
	client = fopen(filename, "r+b");
	struct SyncObject s;
	fseek(client, 0, SEEK_SET);
	fread(&s, sizeof(struct SyncObject), 1, client);
	if (s.isRead == 0)
	{
		return 1;
	}
	return 0;
}


int chechifServerWrote(char *filename)
{
	FILE *server;
	server = fopen(filename, "r+b");
	struct SyncObject s;
	fseek(server, 0, SEEK_SET);
	fread(&s, sizeof(struct SyncObject), 1, server);
	fclose(server);
	if (s.isWrite == 1)
	{
		return 1;
	}
	return 0;
}

int main()
{
	char server[100] = "f:\server.bin";
	char client[100] = "f:\client.bin";
	startClientServer(client);
	int Openflag;
	int Writeflag;
	int Readflag;
	int count = 0;
	while (1)
	{
		Sleep(100);
		if (checkIsServerStarted(server))
		{
			if (count == 0)
			{
				printf("Connected to the Server\n");
				count++;
			}
			if (chechifServerWrote(server))
			{
				FILE *file = fopen(server, "r+b");
				struct Message msg;
				struct SyncObject s;
				fseek(file, 32, SEEK_SET);
				fread(&msg, sizeof(struct Message), 1, file);
				fseek(file, 0, SEEK_SET);
				fread(&s, sizeof(struct SyncObject), 1, file);
				s.isRead = 0;
				s.isWrite = 0;
				fseek(file, 0, SEEK_SET);
				fwrite(&s, sizeof(struct SyncObject), 1, file);
				fclose(file);
				printf("Server: %s \n", msg.message);
			}
			if (checkIfServerRead(client))
			{
				
				FILE *file = fopen(client, "r+b");
				//printf("%d", file);
				struct Message msg;
				printf("Enter the message : ");
				gets(msg.message);
				fseek(file, 32, SEEK_SET);
				fwrite(&msg, sizeof(struct Message), 1, file);
				setClientWrote(client);
				fclose(file);
			}
			
		}
	}
	getchar();
	return 0;
}