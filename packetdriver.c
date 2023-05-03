// Name: Freddy Lopez
// DuckID: flopez2
// Project: A Pthread-based Packet Driver 


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdbool.h>
//additional #includes needed
#include "BoundedBuffer.h"
#include "destination.h"
#include "diagnostics.h"
#include "fakeapplications.h"
#include "freepacketdescriptorstore__full.h"
#include "freepacketdescriptorstore.h"
#include "networkdevice__full.h"
#include "networkdevice.h"
#include "packetdescriptorcreator.h"
#include "packetdescriptor.h"
#include "pid.h"
#include "queue.h"


#define MAX_PID 10
#define UNUSED __attribute__((unused))
typedef struct network_device NetworkDevice;
unsigned long myfpds_size; 

pthread_t consumer_thread;
pthread_t producer_thread;
//pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER; 
//pthread_cond_t mycond = PTHREAD_COND_INITIALIZER; 
NetworkDevice *myNetworkDevice; 
//bounded buffer implimentation
BoundedBuffer *bbrecieving[MAX_PID+1]; // 2D Bounded buffer for producers packages 
BoundedBuffer *bbsend; // Bounded buffer for consumer packages 
FreePacketDescriptorStore *myfpds; 


 

void *receivingThread(UNUSED void *arg){
	PacketDescriptor *currentPD;
	printf("Thread created successfully\n");
	
	// remove packet from fpds
	//clear pd
	while(1){ 
		// 0  1  2  3  4  5 
		// pd pd pd pd pd pd

		myfpds->blockingGet(myfpds, &currentPD);
		initPD(currentPD);
		// register packet descriptor to the Network Device 
		myNetworkDevice->registerPD(myNetworkDevice, currentPD); // what "pd" should this point to 
		// wait for the packet descriptor to be populated 
		//myNetworkDevice->sendPacket(myNetworkDevice, currentPD);

		//printf("AwaitngIncomingPacket....\n");

		/*
		myfpds_size = myfpds->size(myfpds);
		printf("Updated size: %lu\n", myfpds_size);
		*/

		myNetworkDevice->awaitIncomingPacket(myNetworkDevice); 
		//write packet Descriptor to the bbout buffer once filled by the Network device 
		// write the pd in the bbrecieving buffer according to its PID
		PID appPID = getPID(currentPD);
		(bbrecieving[appPID])->blockingWrite(bbrecieving[appPID], (void*)currentPD);
	}
}




void *sendingThread(UNUSED void *arg){
	PacketDescriptor *pdToReturn; 
	//unsigned long current_fpdsSize; 

	// from here, we need to take the pd in bbsend and send it to the Network Device. Use the send packets and if 
	// the poac
	printf("Thread created successfully\n");
	while(1){

		bbsend->blockingRead(bbsend, (void**)&pdToReturn);


		int attempts = 0; 
		// attemp to send the packet
		while(attempts < 10){
			myNetworkDevice->sendPacket(myNetworkDevice, pdToReturn);
			attempts++;
		}

		//once done with the 
		myfpds->blockingPut(myfpds, pdToReturn);

		//current_fpdsSize = myfpds->size(myfpds);
		//printf("Current Size of myfds: %lu\n", current_fpdsSize);


		// once packet is sent and finished being used by the app, we then need to put the pd into the bbin??
		// and THEN pull it out and place into the fpds again???
	}
}



void init_packet_driver(NetworkDevice *nd, void *mem_start, 
	unsigned long mem_length, FreePacketDescriptorStore **fpds){
	/*
	**Create FreePacketDescriptorStore using mem_start and mem_length
	**create any buffers required by our threads
	**create any threads you require for your implementation 

	**return the FDPS to the code that called it 
	*/

	myNetworkDevice = nd; 
	myfpds = FreePacketDescriptorStore_create(mem_start, mem_length); // call the create function for the file descriptor
	myfpds_size = myfpds->size(myfpds);

	//printf("Size of myfds: %lu\n", myfpds_size);


	//bbrecieving = BoundedBuffer_create(MAX_PID+1);
	for(int i = 0; i < MAX_PID+1; i++){
		// we dont want to have a buffer to large or to small to where we run out of packets 
		bbrecieving[i] = BoundedBuffer_create((int)myfpds_size/(MAX_PID+1));
	}

	bbsend = BoundedBuffer_create(MAX_PID+1);

	*fpds = myfpds;
	// create both threads 

	pthread_create(&producer_thread, NULL, &sendingThread, NULL);
	pthread_create(&consumer_thread, NULL, &receivingThread, NULL);
	//pthread_join(producer_thread, NULL);
	//pthread_join(consumer_thread, NULL); 
	//apparenlty we dont need to join, whoops
}


/*these two functions hand in a Packet Descriptor for dispatching. Neither call should 
delay until the packet is actually sent
*/

 

void blocking_send_packet(PacketDescriptor *pd){
	/*
	Blocking call will usually return promptly, 
	but there may be a delay while it waits for space in the buffers  

	**Queue up packet descriptor for sending
	**Do not return until it has be successfullt queued 
	*/

	printf("blocking_send_packet funciton\n");
	bbsend->blockingWrite(bbsend, (void*)pd);

	//sendbuffer
}


int nonblocking_send_packet(PacketDescriptor *pd){
	/*
	Non-blocking call MUST return promptly, 
	indicating whether or not the indicated packet
	has been accepted by my code

	**If able to queue up packet descriptor immediately, do so and return 1
	**otherwise return 0
	*/
	

	/*PID tmp = getPID(pd);
	arr_pid[tmp];
	num_prog++;*/

	int success; 

	success = bbsend->nonblockingWrite(bbsend, (void*)pd);
	if(success){
		return 1; 
	}
	else{
		return 0; 
	}
}


/*These two functions represent requests for packets by the application threads
both calls indicate their process identifier and should only be given appropriate packets 
Permitted to use a small bounded buffer to hold packets that haven't yet been collected by 
a process but we are allowed to discard extra packets if at lest one is waiting uncollected 
for the same PID*/


void blocking_get_packet(PacketDescriptor **pd, PID pid){
	/*
	Only return when a packet is received 
	for the indicated process and the first 
	arg point to it
	**Wait until there is a packet for 'pid'
	**return packet descriptor to the calling application
	*/
	//function that checks if the pid is here
	//printf("blocking_get_packet() is getting called\n");
	//SandSbuffer((void*)&pd, pid);

	//getbuffer
	//return NULL;
	bbrecieving[pid]->blockingRead(bbrecieving[pid], (void**)pd);

}



int nonblocking_get_packet(PacketDescriptor **pd, PID pid){
	/*This must return promptly. Result 1
	 if a packet was found, 0 if no packet 
	 is waiting

	 **If there is currently a waiting packet for 'pid', return that packet 
	 	to the calling application and return 1 for the value of the function (hence the int)
	 	otherwise, return 0 for the value of the function
	 */


	//printf("nonblocking_get_packet() is getting called\n");
	//if((SandSbuffer((void*)&pd, pid)) == 1){
	//	return 1; 
	//}
	//return 0; 

	int res = bbrecieving[pid]->nonblockingRead(bbrecieving[pid], (void**)pd);
	if(res ==1){
		return 1;
	}
	return 0; 

}