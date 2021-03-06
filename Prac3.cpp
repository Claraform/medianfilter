//==============================================================================
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
void Master()
{
 //! <h3>Local vars</h3>
 // The above outputs a heading to doxygen function entry
 MPI_Status stat;    //! stat: Status of the MPI application
 int flag = 0;        //! flag: Flag for message availability
 int windowsize = 9; //! windowsize: Size of filter window

 // Read the input image
 if (!Input.Read("Data/fly.jpg"))
 {
  printf("Cannot read image\n");
  return;
 }

 // Allocated RAM for the output image
 if (!Output.Allocate(Input.Width, Input.Height, Input.Components))
  return;

 // Number of Slaves
 int slaves = numprocs - 1;
 tic();
 
 for (int j = 1; j <= slaves; j++)
 {
  // ACK Value
  int ack[1];
  // Start Row for Slave
  int ystart = (j-1) * Input.Height / slaves;
  // End Row for Slave
  int yend = ceil((float)Input.Height / slaves + ystart);
  //printf("ys %d ye %d \n", ystart, yend);
  // Number of Image Rows in Partition
  int numrows = yend - ystart + 1;
  // Number of Image Rows Needed for Filter
  int windowrows = numrows + windowsize - 1;
  // Send Buffer
  char buffer[windowrows][Input.Width*Input.Components];
  // Partition Data
  int cnt = 0;
  for (int i = ystart - (windowsize / 2); i < yend + (windowsize / 2); i++)
  {
   if (i < 0 || i >= Input.Height)
   {
    for (int j = 0; j < Input.Width * Input.Components; j ++)
    {
     buffer[cnt][j] = 0;
    }
   }
   else
   {
    for (int j = 0; j < Input.Width*Input.Components; j ++)
    {
     buffer[cnt][j] = Input.Rows[i][j];
    }
   }
    cnt++;
  }

  int info[7];
  info[0] = windowsize;
  info[1] = windowrows;
  info[2] = numrows;
  info[3] = Input.Width;
  info[4] = Input.Components;
  info[5] = ystart;
  info[6] = yend;
  // Send Information and Data Size
  MPI_Send(info, 7, MPI_INT, j, TAG, MPI_COMM_WORLD);
  // Receive ACK
  MPI_Recv(ack, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat);
  // Send Data
  MPI_Send(buffer, windowrows*Input.Width*Input.Components, MPI_CHAR, j, TAG, MPI_COMM_WORLD);
  // Receive ACK
  MPI_Recv(ack, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat);
 }
 // Receive Slave Data
 for (int j = 1; j <= slaves; j++)
 {
  // Receive Size of Slave Data
  int rec_info[3];
  MPI_Recv(rec_info, 3, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat);
  int ystart=rec_info[0];
  int yend=rec_info[1];
  int numrows=rec_info[2];
  // Receive Data
  char output[numrows][Input.Width*Input.Components];
  MPI_Recv(output, numrows*Input.Width*Input.Components, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat);
  
  // Save Received Data to Output
  int cnt = 0;
  for (int i = ystart; i < yend; i++)
  {
   for (int j = 0; j < Input.Width * Input.Components; j ++)
   {
    Output.Rows[i][j] = output[cnt][j];
   }
   cnt++;
  }
 }
 
 // Write the output image
 if (!Output.Write("Data/Output.jpg"))
 {
  printf("Cannot write image\n");
  return;
 }
 printf("Image Written \n");
 printf("MPI Time = %lg ms\n", (double)toc() / 1e-3);
 //! <h3>Output</h3> The file Output.jpg will be created on success to save
 //! the processed output.
}
// Compare Function for Quicksort
int compare(int* i,int* j) { return (*i>*j); }
// Median Function
int median(int* window, int size)
{
 qsort(window,size,sizeof(int),(int (*)(const void*,const void*))compare);
 return window[size/2];
}
/** This is the Slave function, the workers of this MPI application. */
void Slave(int ID){
 MPI_Status stat;
 int info[7];
 int windowsize;
 int windowrows;
 int numrows;
 int width;
 int components;
 int ystart;
 int yend;
 //printf("Slave %d Activated \n", ID);

 // Receive Data Size from Master
 MPI_Recv(info, 7, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
 windowsize = info[0];
 windowrows = info[1];
 numrows = info[2];
 width = info[3];
 components = info[4];
 ystart = info[5];
 yend = info[6];

 char in[windowrows][width*components];
 char out[numrows][width*components];
 // Send ACK
 MPI_Send(info, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 // Receive Data
 MPI_Recv(in, windowrows*width*components, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
 // Send ACK
 MPI_Send(info, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 //printf("Slave %d Received from Master \n", ID);

 // Filter Data
 int r = 0;
 for (int y = windowsize / 2; y < windowsize / 2 + numrows; y++)
 {
  for (int x = 0; x < width * components; x++)
  {
   int window[windowsize * windowsize];
   int cnt = 0;
   for (int i = y - windowsize / 2; i <= y + windowsize / 2; i++)
   {
    for (int j = x-components*(windowsize/2); j <= x+components*(windowsize/2); j = j + components)
    {
     if (j < 0 || j >= width * components)
     {
      window[cnt] = 0;
     }
     else
     {
      window[cnt] = in[i][j];
     }
     cnt++;
    }
   }
   int m = median(window, windowsize * windowsize);
   out[r][x] = m;
  }
  r++;
 }

 // send to rank 0 (master):
 int par[3];
 par[0] = ystart;
 par[1] = yend;
 par[2] = numrows;
 // Send Data Size to Master
 MPI_Send(par, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 // Send Data to Master
 MPI_Send(out, numrows*width*components, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
 //printf("Slave %d Sent to Master\n", ID);
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
