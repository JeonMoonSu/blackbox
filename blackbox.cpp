#pragma once
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>


using namespace cv;
using namespace std;

char* command_call(char* path)
{
        FILE *read_fp;
        int chars_read;
        char buffer[BUFSIZ +1];
        char *str = (char*)malloc(BUFSIZ +1);

        memset(buffer, '\0', sizeof(buffer));
        read_fp = popen(path, "r");
        chars_read = fread(buffer, sizeof(char), BUFSIZ, read_fp);

        strcpy(str ,buffer);

        pclose(read_fp);

        return str;
}
void remove_dir()
{
        char path[BUFSIZ + 1] = "ls -rt ./user/blackbox/";
        char *sArr[BUFSIZ + 1] = { NULL , };
        char rmdir[BUFSIZ + 1] = "rm -r ./user/blackbox/";
        int i = 0;
        int byte = 0;

        char buffer[BUFSIZ +1];
        strcpy(buffer, command_call(path));

        char* ptr = strtok(buffer, "\n ");

        while(ptr != NULL)
        {
                sArr[i] = ptr;
                i++;

                ptr = strtok(NULL, " ");
        }

        printf("remove directory = %s\n", sArr[0]);
        strcat(rmdir ,sArr[0]);
        system(rmdir);

}
int check_disk()
{
        char path[BUFSIZ + 1] = "du -s ./user/blackbox/";
	char size[BUFSIZ + 1] = "df -k";
        char *sArr[BUFSIZ + 1] = { NULL , };
	char *sArr2[BUFSIZ + 1] = { NULL , };
        int i = 0;
        int byte = 0;
	int a_size = 0;

        char buffer[BUFSIZ +1];
        strcpy(buffer, command_call(path));
	
	char buffer2[BUFSIZ + 1];
	strcpy(buffer2, command_call(size));

        char* ptr = strtok(buffer, "\0");
	char* ptr2 = strtok(buffer2, "\n");
	for(int j = 0 ; j < 4 ; ++ j)
	{
		ptr2 = strtok(NULL," ");
	}
        while(ptr != NULL)
        {
                sArr[i] = ptr;
                i++;

                ptr = strtok(NULL, " ");
        }

	a_size = atoi(ptr2);
        byte = atoi(sArr[0]);
        printf("current size = %dkb , available size = %dkb\n", byte , a_size);

        return byte;
}

void getfiletime(time_t org_time, char *time_str)
{
    struct tm *tm_ptr;
    tm_ptr = localtime(&org_time);

    sprintf(time_str, "%02d%02d%02d_%02d%02d%02d",
                    tm_ptr->tm_year+1900,
                    tm_ptr->tm_mon+1,
                    tm_ptr->tm_mday,
                    tm_ptr->tm_hour,
		    tm_ptr->tm_min,
		    tm_ptr->tm_sec);
}
void getdirtime(time_t org_time, char *time_str)
{
    struct tm *tm_ptr;
    tm_ptr = localtime(&org_time);

    sprintf(time_str, "%02d%02d%02d_%02d",
                    tm_ptr->tm_year+1900,
                    tm_ptr->tm_mon+1,
                    tm_ptr->tm_mday,
                    tm_ptr->tm_hour
                    );
}
int main(int argc, char* argv[])
{
	Mat img_color;
	pid_t ppid;
	time_t the_time;
	char v_buffer[100];
	char d_buffer[100];
	string videoName;
	string folderName;
	int s_time;
	bool first=true;

	ppid = getppid();
	// for jetson onboard camera
	string gst = "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)500, height=(int)300,format=(string)NV12, framerate=(fraction)24/1 ! nvvidconv flip-method=0 ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
	
	VideoCapture cap(gst);

	//video open failed
	if(!cap.isOpened())
	{
		cout << "errororor" <<endl;
		return 1;
	}
	
	VideoWriter video;
	system("mkdir ./user");
	system("mkdir ./user/blackbox");

	while(1)
	{
		cap >> img_color;
		
		time(&the_time);
		getdirtime(the_time,d_buffer);
		//record starting first = check start time, make new directory
		if(first)
		{
			s_time = the_time;
			folderName = d_buffer;
			string str = "mkdir ./user/blackbox/"+folderName;
			const char* command = str.c_str();
			system(command);
		}
		//hour divided folder,make new folder
		if(folderName != d_buffer)
		{
			folderName = d_buffer;
			string str = "mkdir ./user/blackbox/"+folderName;
			const char* command = str.c_str();
			system(command);
		}
		//make new record file 
		if(the_time >= s_time+30 || first) //first or after 10seconds
		{
			if(check_disk() > 9300000)
			{
				printf("buffer over\n");
				remove_dir();
			}
			s_time = the_time;
			getfiletime(the_time,v_buffer);
			videoName = "./user/blackbox/"+folderName+"/";
			videoName += v_buffer;
			videoName += ".avi";
			
			video.open(videoName,CV_FOURCC('D','I','V','X'),24,Size(500,300));
			kill(ppid,SIGUSR1);
			first = false;
		}

		//read,write
		video.write(img_color);
		imshow("Color", img_color);

		if (waitKey(25) >= 0)
			break;
	}
	return 0;
}
