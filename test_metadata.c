#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libics.h"

int main(int argc, const char* argv[]) {
   ICS*         ip;
   Ics_DataType dt;
   int          ndims;
   size_t       dims[ICS_MAXDIM];
   size_t       bufsize;
   void*        buf1;
   void*        buf2;
   Ics_Error    retval;


   if (argc != 2) {
      fprintf(stderr, "One file name required\n");
      exit(-1);
   }

   /* Open image for update */
   retval = IcsOpen(&ip, argv[1], "rw");
   if (retval != IcsErr_Ok) {
      fprintf(stderr, "Could not open input file: %s\n",
              IcsGetErrorText(retval));
      exit(-1);
   }
   IcsGetLayout(ip, &dt, &ndims, dims);
   bufsize = IcsGetDataSize(ip);
   buf1 = malloc(bufsize);
   if (buf1 == NULL) {
      fprintf(stderr, "Could not allocate memory.\n");
      exit(-1);
   }
   retval = IcsGetData(ip, buf1, bufsize);
   if (retval != IcsErr_Ok) {
      fprintf(stderr, "Could not read input image data: %s\n",
              IcsGetErrorText(retval));
      exit(-1);
   }

   /* Add and change metadata */
   retval = IcsSetPosition(ip, 0, 1834, 0.02, "milimeter");
   if (retval == IcsErr_Ok)
      retval = IcsSetPosition(ip, 1, -653, 0.014, "milimeter");
   if (retval != IcsErr_Ok) {
      fprintf(stderr, "Could not set pixel position: %s\n",
              IcsGetErrorText(retval));
      exit(-1);
   }
   retval = IcsAddHistory(ip, "test", "Adding history line.");
   if (retval != IcsErr_Ok) {
      fprintf(stderr, "Could not add history line: %s\n",
              IcsGetErrorText(retval));
      exit(-1);
   }

   /* Commit changes */
   retval = IcsClose(ip);
   if (retval != IcsErr_Ok) {
      fprintf(stderr, "Could not close input file: %s\n",
              IcsGetErrorText(retval));
      exit(-1);
   }

   /* Read image */
   retval = IcsOpen(&ip, argv[1], "r");
   if (retval != IcsErr_Ok) {
      fprintf(stderr, "Could not open output file for reading: %s\n",
              IcsGetErrorText(retval));
      exit(-1);
   }
   if (bufsize != IcsGetDataSize(ip)) {
      fprintf(stderr, "Data in output file not same size as input.\n");
      exit(-1);
   }
   buf2 = malloc(bufsize);
   if (buf2 == NULL) {
      fprintf(stderr, "Could not allocate memory.\n");
      exit(-1);
   }
   retval = IcsGetData(ip, buf2, bufsize);
   if (retval != IcsErr_Ok) {
      fprintf(stderr, "Could not read output image data: %s\n",
              IcsGetErrorText(retval));
      exit(-1);
   }
   retval = IcsClose(ip);
   if (retval != IcsErr_Ok) {
      fprintf(stderr, "Could not close output file: %s\n",
              IcsGetErrorText(retval));
      exit(-1);
   }
   if (memcmp(buf1, buf2, bufsize) != 0) {
      fprintf(stderr, "Data in output file does not match data in input.\n");
      exit(-1);
   }

   free(buf1);
   free(buf2);
   exit(0);
}
