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
//additional #includes needed
#include "destination.h"
#include "diagnostics.h"
#include "fakeapplications.h"
#include "freepacketdescriptorstore_full.h"
#include "freepacketdescriptorstore.h"
#include "networkdevice_full.h"
#include "networkdevice.h"
#include "packetdescriptorcreator.h"
#include "packetdescriptor.h"
#include "pid.h"
#include "queue.h"




typedef struct free_packet_descriptor_store FreePacketDescriptorStore;

FreePacketDescriptorStore *FreePacketDescriptorStore_create(void *mem_start, unsigned long mem_length);

void FreePacketDescriptorStore_destroy(FreePacketDescriptorStore *fpds); 

struct free_packet_descriptor_store{
	/*
	Blocking verison: Only returns if it is successful 
	NonBlocking version: returns 1 if successful; 0 otherwise
	Get Method set their final argument if successful
	*/
	void *self; 
	void (*blockingGet)(FreePacketDescriptorStore *fpds, PacketDescriptor **pd);
	int (*nonblockingGet)(FreePacketDescriptorStore *fpds, PacketDescriptor **pd);
	void (*blockingPut)(FreePacketDescriptorStore *fpds, PacketDescriptor **pd);
	int (*nonblockingPut)(FreePacketDescriptorStore *fpds, PacketDescriptor **pd);
	unsigned long (*size)(FreePacketDescriptorStore *fpds); 

}


/*
When OS boots up, invoke the following initialization routine
Called before any other methods, to allow you to initialize data 
structures and start any internal threads

Args:
nd: the network device that I must drive
mem_start, mem_length: some memory for PacketDescriptors
fpds: You hand back a FreePacketDescriptorStore into which PacketDescriptor build from the memory found 
and described in args 2 and 3 have been put
*/


/* 
**Create FreePacketDescriptorStore using mem_start and mem_length
**create any buffers required by our threads
**create any threads you require for your implementation 

**return the FDPS to the code that called it 
*/

void init_packet_driver(NetworkDevice *nd, void * mem_start, 
	unsigned long mem_length, FreePacketDescriptorStore **fpds){

}







					/*these two functions hand in a Packet Descriptor for dispatching. Neither call should 
					delay until the packet is actually sent
					*/

 
/*
Blocking call will usually return promptly, 
but there may be a delay while it waits for space in the buffers  

**Queue up packet descriptor for sending
**Do not return until it has be successfullt queued 
*/
void blocking_send_packet(PacketDescriptor *pd){
	return NULL; 

}

/*
Non-blocking call MUST return promptly, 
indicating whether or not the indicated packet
has been accepted by my code

**If able to queue up packet descriptor immediately, do so and return 1
**otherwise return 0
*/
int nonblocking_send_packet(PacketDescriptor *pd){
	return 1; 
}







					/*These two functions represent requests for packets by the application threads
					both calls indicate their process identifier and should only be given appropriate packets 
					Permitted to use a small bounded buffer to hold packets that haven't yet been collected by 
					a process but we are allowed to discard extra packets if at lest one is waiting uncollected 
					for the same PID*/

/*
Only return when a packet is received 
for the indicated process and the first 
arg point to it

**Wait until there is a packet for 'pid'

**return packet descriptor to the calling application
*/
void blocking_get_packet(PacketDescriptor **pd, PID){
	return NULL; 
}


/*This must return promptly. Result 1
 if a packet was found, 0 if no packet 
 is waiting

 **If there is currently a waiting packet for 'pid', return that packet 
 	to the calling application and return 1 for the value of the function (hence the int)
 	otherwise, return 0 for the value of the function
 */
int nonblocking_get_packet(PacketDescriptor **pd, PID){

}




int main(int argc, char *argv[]){
	return 1;
}