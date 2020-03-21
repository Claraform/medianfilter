t//==============================================================================
// Copyright (C) John-Philip Taylor
// tyljoh010@myuct.ac.za
//
// This file is part of the EEE4084F Course
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//
// This is an adaptition of The "Hello World" example avaiable from
// https://en.wikipedia.org/wiki/Message_Passing_Interface#Example_program
//==============================================================================


/** \mainpage Prac3 Main Page
 *
 * \section intro_sec Introduction
 *
 * The purpose of Prac3 is to learn some basics of MPI coding.
 *
 * Look under the Files tab above to see documentation for particular files
 * in this project that have Doxygen comments.
 */



//---------- STUDENT NUMBERS --------------------------------------------------
//
// Please note:  put your student numbers here !!  <<<< NB!  NB!
//
//-----------------------------------------------------------------------------

/* Note that Doxygen comments are used in this file. */
/** \file Prac3
 *  Prac3 - MPI Main Module
 *  The purpose of this prac is to get a basic introduction to using
 *  the MPI libraries for prallel or cluster-based programming.
 */

// Includes needed for the program
#include "Prac3.h"

/** This is the master node function, describing the operations
    that the master will be doing */
void Master () {
 //! <h3>Local vars</h3>
 // The above outputs a heading to doxygen function entry
 int  j;             //! j: Loop counter
 int buff[BUFSIZE]; //! buff: Buffer for transferring message data
 MPI_Status stat;    //! stat: Status of the MPI application
 int flag = 0;       //! flag: Flag for message availability

 // Read the input image
 if(!Input.Read("Data/greatwall.jpg")){
  printf("Cannot read image\n");
  return;
 }

 // Allocated RAM for the output image
 if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return;
 //int num_pixels = Input.Height*Input.Width;
 //int pix_slaves = num_pixels/numprocs;

 printf("0: We have %d processors\n", numprocs);
 for(j = 0; j < (numprocs-1); j++) 
 {
  // Start Row for Slave
  int ystart = j*Input.Height/(numprocs-1);
  // End Row for Slave
  int yend = Input.Height/(numprocs-1) + ystart;
  // Data to Send to Slave
  int coords[2];
  coords[0] = ystart;
  coords[1] = yend;
 }
  // Split data to send to slaves
 for(y = 0; y < (Input.Height); y++){
  for(x = 0; x < Input.Width*Input.Components; x++){

  }
 }
  MPI_Send(buff, BUFSIZE, MPI_BYTE, j, TAG, MPI_COMM_WORLD);
 
 
 // Block Receive
 int rec = numprocs - 1; // Number of slaves to receive from
 while(rec!=0)
 {
  while(!flag)
  {
   MPI_Iprobe(MPI_ANY_PROBE, TAG, MPI_COMM_WORLD, &flag, &stat);
  }
  MPI_Recv(buff, BUFSIZE, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat);
 }

 // Write the output image
 if(!Output.Write("Data/Output.jpg")){
  printf("Cannot write image\n");
  return;
 }
 //! <h3>Output</h3> The file Output.jpg will be created on success to save
 //! the processed output.
}
//------------------------------------------------------------------------------
void median(int* buff)
{
 int n = 0;
 qsort((void*)buff, n, sizeof(int*), compare);

}
int compare(const int* a, const int* b)
{
 if (a > b)
  return 1;
 else if (a < b)
  return -1;
 else
  return 0;
}
/** This is the Slave function, the workers of this MPI application. */
void Slave(int ID){

 int buff [BUFSIZE];
 int count;

 MPI_Status stat;
 
 // receive from rank 0 (master):
 // This is a blocking receive, which is typical for slaves.
 MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
 MPI_Get_count(&stat, MPI_INT, &count);
 
 // Allocate buffer to hold incoming numbers
 int* number_buf = (int*)malloc(sizeof(int) * count);
 
 // Receiver data
 MPI_Recv(buff, BUFSIZE, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);

 median(buff);
 // send to rank 0 (master):
 MPI_Send(buff, BUFSIZE, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 // End of "Hello World" example................................................
}
//------------------------------------------------------------------------------

/** This is the entry point to the program. */
int main(int argc, char** argv){
 int myid;

 // MPI programs start with MPI_Init
 MPI_Init(&argc, &argv);

 // find out how big the world is
 MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

 // and this processes' rank is
 MPI_Comm_rank(MPI_COMM_WORLD, &myid);

 // At this point, all programs are running equivalently, the rank
 // distinguishes the roles of the programs, with
 // rank 0 often used as the "master".
 if(myid == 0) Master();
 else          Slave (myid);

 // MPI programs end with MPI_Finalize
 MPI_Finalize();
 return 0;
}
//------------------------------------------------------------------------------
