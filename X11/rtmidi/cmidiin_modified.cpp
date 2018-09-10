//*****************************************//
//  cmidiin.cpp
//  by Gary Scavone, 2003-2004.
//
//  Simple program to test MIDI input and
//  use of a user callback function.
//
//*****************************************//

#include <iostream>
#include <cstdlib>
#include "RtMidi.h"
#include "terasic_os_includes.h"

#define REG_BASE 0xFF200000
#define REG_SPAN 0x00200000

int on_off;
int pitch;
int velocity;
int fd;

void* virtual_base;
void* input_arbitration_addr;


void setonoff( int turn_on) {
  on_off = turn_on;
}

void setpitch( int node ) {
  pitch = node;
}

void setvolume( int volume ) {
  velocity = volume;
}

int getonoff() {
  return ( on_off);
}

int getpitch() {
  return ( pitch );
}

int getvolume() {
  return ( velocity );
}

void transferData(int on_off, int pitch, int volume) {
  //store values
  uint32_t input0=on_off;
  uint32_t input1=pitch;
  uint32_t input2=volume;
  //write each data to the corresponding memory address
  *(uint32_t *)input_arbitration_addr=input0<<24 | input1<<16 | input2<<8;
}

void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications.
  std::cout << "\nuseage: cmidiin <port>\n";
  std::cout << "    where port = the device to use (default = 0).\n\n";
  exit( 0 );
}

void mycallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ )
{
  unsigned int nBytes = message->size();
  for ( unsigned int i=0; i<nBytes; i++ ) {
    //std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
    if (i == 1) {
      int pitch_number = (int)message->at(1);
      setpitch(pitch_number);
    }
    else if (i == 2) {
      int pitch_velocity = (int)message->at(2);
      setvolume(pitch_velocity);
      if (pitch_velocity > 0)
        setonoff(1);
      else
        setonoff(0);
    }
  }
  if ( nBytes > 0 )
    //std::cout << "stamp = " << deltatime << std::endl;
    //transfer data to fpga
    transferData(getonoff(), getpitch(), getvolume());
    std::cout << "on/off = " << getonoff() << ", pitch = " << getpitch() << ", ";
  if (getonoff() == 1) {
    std::cout << "volume = " << getvolume() << std::endl;
  }
  else
    std::cout << std::endl;
}

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort( RtMidiIn *rtmidi );

int main( int argc, char ** /*argv[]*/ )
{ 

  fd=open("/dev/mem", ( O_RDWR | O_SYNC ) );
  virtual_base = mmap( NULL, REG_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, REG_BASE );
  input_arbitration_addr=virtual_base+0x0;	

  RtMidiIn *midiin = 0;

  // Minimal command-line check.
  if ( argc > 2 ) usage();

  try {

    // RtMidiIn constructor
    midiin = new RtMidiIn();

    // Call function to select port.
    if ( chooseMidiPort( midiin ) == false ) goto cleanup;

    // Set our callback function.  This should be done immediately after
    // opening the port to avoid having incoming messages written to the
    // queue instead of sent to the callback function.
    midiin->setCallback( &mycallback );

    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );

    std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
    char input;
    std::cin.get(input);

  } catch ( RtMidiError &error ) {
    error.printMessage();
  }

 cleanup:

  delete midiin;

  return 0;
}

bool chooseMidiPort( RtMidiIn *rtmidi )
{
  /*std::cout << "\nWould you like to open a virtual input port? [y/N] ";

  std::string keyHit;
  std::getline( std::cin, keyHit );
  if ( keyHit == "y" ) {
    rtmidi->openVirtualPort();
    return true;
  }*/

  std::string portName;
  unsigned int i = 0, nPorts = rtmidi->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No input ports available!" << std::endl;
    return false;
  }

  if ( nPorts == 1 ) {
    std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
  }
  else {
    for ( i=0; i<nPorts; i++ ) {
      portName = rtmidi->getPortName(i);
      std::cout << "  Input port #" << i << ": " << portName << '\n';
    }

    /*do {
      std::cout << "\nChoose a port number: ";
      std::cin >> i;
    } while ( i >= nPorts );
    std::getline( std::cin, keyHit );*/  // used to clear out stdin
  }

  rtmidi->openPort( 5 );

  return true;
}
