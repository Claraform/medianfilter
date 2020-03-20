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
 int windowsize = 3; //! windowsize: Size of filter window

 // Read the input image
 if (!Input.Read("Data/small.jpg"))
 {
  printf("Cannot read image\n");
  return;
 }

 // Allocated RAM for the output image
 if (!Output.Allocate(Input.Width, Input.Height, Input.Components))
  return;

 // Number of Slaves
 int slaves = numprocs - 1;

 printf("0: We have %d processors\n", numprocs);
 for (int j = 1; j <= slaves; j++)
 {
  // Start Row for Slave
  int ystart = (j-1) * Input.Height / slaves;
  // End Row for Slave
  int yend = Input.Height / slaves + ystart;
  // Number of Image Rows in Partition
  int numrows = yend - ystart + 1;
  // Number of Image Rows Needed for Filter
  int windowrows = numrows + windowsize - 1;
  JSAMPLE **NewRows = new JSAMPLE *[windowrows];
  
  int cnt = 0;
  for (int i = ystart - (windowsize / 2); i < yend + (windowsize / 2); i++)
  {
   NewRows[cnt] = new JSAMPLE[Input.Width * Input.Components];
   //printf("Allocating\n");
   if (i < 0 || i > Input.Height)
   {
    NewRows[cnt] = nullptr;
    cnt++;
   }
   else
   {
    NewRows[cnt] = Input.Rows[i];
    cnt++;
   }
  }
  int info[7];
  info[0] = windowsize;
  info[1] = windowrows;
  info[2] = numrows;
  info[3] = Input.Width;
  info[4] = Input.Components;
  info[5] = ystart;
  info[6] = yend;
  MPI_Send(info, 7, MPI_INT, j, TAG, MPI_COMM_WORLD);
  for (int AH = 0; AH < windowrows; AH++)
  {
   MPI_Send(&NewRows[AH], Input.Width*Input.Components, MPI_INT, j, TAG, MPI_COMM_WORLD);
  }
  printf("Sent\n");
 }

 for (int j = 1; j <= slaves; j++)
 {
  printf("Master waiting \n");
  int rec_info[3];
  MPI_Recv(rec_info, 3, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat);
  printf("got \n");
  int ystart=rec_info[0];
  int yend=rec_info[1];
  int numrows=rec_info[2];
  printf("Received %d \n", j);
  JSAMPLE **received = new JSAMPLE *[numrows];
  for (int z = 0; z < numrows; z++)
  {
   received[z] = new JSAMPLE[Input.Width * Input.Components];
  }
  for (int plswork = 0; plswork < numrows; plswork++)
  {
   MPI_Recv(&received[plswork][0], Input.Width*Input.Components, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat);
  }
  printf("Received again %d \n", j);
  int dummy = 0;
  printf("ys %d ye %d \n", ystart, yend);
  for (int i = ystart; i <= yend; i++)
  {
   Output.Rows[i] = received[dummy];
   dummy++;
   //printf("did a thing? \n");
  }
  printf("done ?? \n");
 }
 // Write the output image
 if (!Output.Write("Data/Output.jpg"))
 {
  printf("Cannot write image\n");
  return;
 }
 printf("he wrote? \n");
 //! <h3>Output</h3> The file Output.jpg will be created on success to save
 //! the processed output.
}

int compare(int* i,int* j) { return (*i>*j); }
int median(int* window, int size)
{
 //printf("Median \n");
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
 printf("Slave\n");

 // Receiver data
 MPI_Recv(info, 7, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
 printf("Got size\n");
 windowsize = info[0];
 windowrows = info[1];
 numrows = info[2];
 width = info[3];
 components = info[4];
 ystart = info[5];
 yend = info[6];
 JSAMPLE** data = new JSAMPLE*[windowrows];
 for (int z = 0; z < windowrows; z++)
 {
  data[z] = new JSAMPLE[width * components];
 }

 for (int BRUH = 0; BRUH < windowrows; BRUH++)
 {
  MPI_Recv(&data[BRUH][0], width*components, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
 }

 printf("Got Data\n");
 /*
 for (int i = 10; i < 12; i++)
 {
  for (int j = 0; j < width * components; j++)
  {
   printf("out %d = %d \n", j, data[i][j]);
  }
 }
 */
 // Output Buffer
 JSAMPLE **output = new JSAMPLE *[numrows];
 for (int z = 0; z < numrows; z++)
 {
  output[z] = new JSAMPLE[width * components];
 }
 // Filter Data with Multiple Components
 for (int y = windowsize/2; y < windowsize/2 + numrows; y++)
  {
   int r = 0;
   //printf("y %d \n", y);
   for (int x = 0; x < width; x++)
   {
    //printf("x %d \n", x);
    for (int c = 0; c < components; c++)
    {
     //printf("c %d \n", c);
     int window[windowsize * windowsize];
     int cnt = 0;
     for (int i = y - windowsize/2; i <= y + windowsize/2; i++)
     {
      for (int j = x - (windowsize/2)*components + c; j <= x + (windowsize/2)*components + c; j+=components)
      {
       //printf("j %d \n", j);
       if (j < 0 || j > width*components)
       {
        window[cnt] = 0;
        //printf("window %d %d is %d \n", window[cnt]);
       }
       else
       {
        //printf("grrrr \n");
        window[cnt] = data[i][j];
        //printf("window %d %d is %d \n", i, j, window[cnt]);
       }
       cnt++;
      }
     }
     int m = median(window, windowsize*windowsize);
     //printf("m = %d \n", m);
     output[r][components*x + c] = m;
    }
   }
   r++;
  }

 printf("Done processing \n");
 // send to rank 0 (master):
 int out[3];
 out[0] = ystart;
 out[1] = yend;
 out[2] = numrows;
 printf("why %d \n", ystart);
 MPI_Send(out, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 //printf("why \n");
 //MPI_Send(&yend, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 //printf("why \n");
 //MPI_Send(&numrows, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 //printf("Slave sent %p\n", &output[44][90]);
 for (int GRR = 0; GRR < numrows; GRR++)
 {
  printf("meow \n");
  MPI_Send(&output[GRR], width*components, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 }

 printf("Slave sent again\n");
 /*
 if (output)
 {
  for (int j = 0; j < numrows; j++)
  {
   if (output[j])
    delete[] output[j];
  }
  delete[] output;
 }
 if (data)
 {
  for (int j = 0; j < windowrows; j++)
  {
   if (data[j])
    delete[] data[j];
  }
  delete[] data;
 }
 */
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
