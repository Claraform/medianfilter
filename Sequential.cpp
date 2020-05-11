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
//==============================================================================

#include "Sequential.h"
//------------------------------------------------------------------------------

// Compare Function for Quicksort
int compare(int* i,int* j) { return (*i>*j); }
// Median Function
int median(int* window, int size)
{
 qsort(window,size,sizeof(int),(int (*)(const void*,const void*))compare);
 return window[size/2];
}
int main(int argc, char **argv)
{
 // Size of window
 int windowsize = 3;
 // Read the input image
 if (!Input.Read("Data/altsmall.jpg"))
 {
  printf("Cannot read image\n");
  return -1;
 }

 // Allocated RAM for the output image
 if (!Output.Allocate(Input.Width, Input.Height, Input.Components))
  return -1;

 // This is example code of how to copy image files ----------------------------
 printf("Start of sequential code...\n");
 tic();
 for (int y = 0; y < Input.Height; y++)
 {
  for (int x = 0; x < Input.Width * Input.Components; x++)
  {
   // Create a window
   int window[windowsize*windowsize];
   int cnt = 0;
   for (int i = y - windowsize/2; i < y + windowsize/2; i++)
   {
    for (int j = x - (windowsize/2) * Input.Components; j <= x + (windowsize/2) * Input.Components; j += Input.Components)
    {
     if (j < 0 || j >= Input.Width * Input.Components || i < 0 || i >= Input.Height)
     {
      window[cnt] = 0;
     }
     else
     {
      window[cnt] = Input.Rows[i][j];
     }
     cnt++;
    }
   }

   int m = median(window, windowsize * windowsize);
   Output.Rows[y][x] = m;
  }
 }
 printf("Sequential Time = %lg ms\n", (double)toc() / 1e-3);
 
 // Write the output image
 if (!Output.Write("Data/smallOut.jpg"))
 {
  printf("Cannot write image\n");
  return -1;
 }

 return 0;
}
//------------------------------------------------------------------------------
