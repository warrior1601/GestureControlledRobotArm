//-----------Remote.cpp---------//
// Remote.cpp
// Gesture_Controlled_Robot_Arm
// Created By Jedidiah Paterson 11/12/2020.
// Copyright @ 2020 Jedidiah Paterson. All right reserved.
// This file combines the basic concepts socket communication
// On a local network and the GIOP and PWM conrols of the
// Raspberry Pi to remotely contol the Robot Arm from a nother
// device on the network.

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>

#include <wiringPi.h> //Required for GIOP defines
#include <softPwm.h>  //Required for Pulse Width Modulation controls
#include <iostream>   //Required for "cout"

#define PORT 1948
/*
 * These defines will help keep track of what finger we are trying to move
 */
#define Pinky 0  // Pin 11 /////// 14 extends   24 curls
#define Ring 2   // Pin 13 /////// 14 extends   25 curls
#define Middle 3 // Pin 15 /////// 13 extends   25 curls 
#define Index 4  // Pin 16 /////// 14 extends   25 curls
#define Thumb 5  // Pin 18 /////// 23 extends   14 curls

#define Elbow 6             // Pin 22 /////// 13 extends   12 curls 
#define ElbowLimitSwitch 27 // Pin 36
#define Wrist 7             // Pin 7  /////// 3 positions 
#define Shoulder 13         // Pin 21 

/*
 * >=13 extends arm
 * <=12 curls the arm 
 * 12 and 13 are slowest movements. You can only switch from one direction
 * to the the slowest movement of the other direction
 * the you can move faster, i.e. 09 or 16 
 */

char message[4096]; //This will change when the Message formating is finished

 int counter = 0;
 int Test = 0;
 bool interruptStatusExtededStop = false;
 bool interruptStatusCurledStop  = false;
 bool TimeDelay = false;

using namespace std;

/*
 * This fuction accepts two interagers and processes
 * Them as finger movements. A "cout" function is
 * Also provided to ensure the command sent is the
 * Same as what is being issued. in final product this
 * Will be removed
 */
 void myInterupt(void)
{	
    if(TimeDelay == true)
    {}
    else
    {	
    // This will prohibit movement in one direction
	if(Test >= 13)
	{
	interruptStatusExtededStop = true;
	cout << "At the Exteded Stop" << endl;
	}
	else if(Test <= 12)
	{
	interruptStatusCurledStop = true;
	cout << "At the Curled Stop" << endl;		
	}
	else
	{}
	
	softPwmWrite(Elbow, 0); //stops the movement
	delay(50);
	TimeDelay = true;
	
	counter++;
	cout << "Counter: " << counter << endl;
    }
}

void MoveFinger(int Finger, int Position)
{
    //cout<< "Finger: " << Finger << "Postion: " << Position << endl;
    softPwmWrite(Finger, Position );
    delay(50); //This delay will need to be reduced as far as possible
}

void MoveElbow(int Finger, int Position)
{
	if((Finger >= 13)&&(interruptStatusExtededStop == false))
	{
	softPwmWrite(Finger, Position );
	delay(50);
	interruptStatusCurledStop = false;
	}
	else if((Finger <= 12)&&(interruptStatusCurledStop == false))
	{
	softPwmWrite(Finger, Position );
	delay(50);
	interruptStatusExtededStop = false;
	}
	else
	{}
	
	TimeDelay = false;
	cout << "Position: " << Position << endl;
}

//Going to Change
void CheckMessageContent()
{
	//cout <<"recieved message: " <<message<<endl;
	vector<int> Finger_Number = {Pinky,Ring,Middle,Index,Thumb,Wrist,Elbow,Shoulder};
	vector<int> Finger_CMD;
	
	string Commands(message); // convert from char pointer to string
	for(int i=4; i<int(Commands.size()); i+=6)
	{
		string CMD_Finger = Commands.substr(i, 2);
		Finger_CMD.push_back(stoi(CMD_Finger));
	}
	for(int i=0; i<int(Finger_CMD.size()); i++)
	{
	  //  if(Finger_Number[i] == Elbow)
	   // {
	//	MoveElbow(Elbow,Finger_CMD[i] );
	   // }
	   // else
	  //  {
		MoveFinger(Finger_Number[i],Finger_CMD[i] );
	   // }
	}
    //format fo rthe message will be FingerPxx
    //F = finger number
    //P = postition
    //string Commands(message); // convert from char point to string
    //size_t finger = Commands.find_last_of("P");
    //string CMD_Finger = Commands.substr(1, finger-1);
    //string CMD_Position = Commands.substr(finger+1, finger+2);
    //MoveFinger(stoi(CMD_Finger),stoi(CMD_Position) );
}

int main (int argc, char const *argv[])
{
    wiringPiSetup () ; //Standard setup for declaring GIOPs
    /*
     * pinMode sets the Pin as an output or an input
     */
    pinMode(Pinky,  OUTPUT);
    pinMode(Ring,   OUTPUT);
    pinMode(Middle, OUTPUT);
    pinMode(Index,  OUTPUT);
    pinMode(Thumb,  OUTPUT);
    pinMode(Elbow,  OUTPUT);
    pinMode(Shoulder,  OUTPUT);
    pinMode(Wrist,   OUTPUT);
    
    pullUpDnControl(ElbowLimitSwitch, PUD_UP);
    if(wiringPiISR (ElbowLimitSwitch, INT_EDGE_FALLING, &myInterupt) < 0 )
    {cout << stderr << "Unable to setup ISR: " << strerror(errno) <<endl;}

    /*
     * softPwmCreate make a GIOP pin a Pulse Width Modulation pin
     * the two other inputs are (int)s and the seconds is the frequency
     * in Hz, what this should be is on the Servo Data sheet
     */
    softPwmCreate(Pinky,  0, 50);
    softPwmCreate(Ring,   0, 50);
    softPwmCreate(Middle, 0, 50);
    softPwmCreate(Index,  0, 50);
    softPwmCreate(Thumb,  0, 50);
    softPwmCreate(Wrist,  0, 50);
    softPwmCreate(Elbow,  0, 50);
    softPwmCreate(Shoulder,  0, 50);
    
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    //creating socket file descriptor
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
		perror("socket failed");
	}

	cout << "Creating a socket" << endl;

    //Forcefully attaching socket to the port 1948
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    cout << "Assigning Port ID" << endl;

    //Forcefully attaching socket to the port 1948
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

	cout << "Binding socket and Port" << endl;

    if(listen(server_fd,3) <0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    cout << "Listening....." << endl;
    
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
    {
        perror("accepted failed");
        exit(EXIT_FAILURE);
    }
    
    cout << "Accepted Communication" <<endl;
    
    while(1)
    {
        valread = read (new_socket, message, 4096);

        if(valread >0)
        {
            CheckMessageContent();
        }
    }
}
