#include<stdio.h>
#include "ezxml.h"
#include "xml.h"
#include "binary.h"
#include "kanji.h"
#include "kanjis.h";
#include "globals.h";
#include "normalize.h";
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <cstring>

int main(int argc, char **argv)
{
        // input directory of xml files
        // output data file
        // load all data from xml
        // load all data from data file
        // compare correctness
        // -i source_dir
    
        int iflag = 0;
        char *ivalue = NULL;
        int c;
     
        opterr = 0;
    
        while ((c = getopt (argc, argv, "i:")) != -1)
         switch (c) {
           case 'i':
               iflag = 1;
               ivalue = optarg;
             break;
           case '?':
             if (optopt == 'i')
               fprintf (stderr, "Option -%c requires an argument.\n", optopt);
             else if (isprint (optopt))
               fprintf (stderr, "Unknown option `-%c'.\n", optopt);
             else
               fprintf (stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
             return 1;
           default:
             abort ();
         }
        
        if(iflag==1) {
            kanji kjis[2200];
            int i=0;
                // read all xml files
               DIR* dirFile = opendir(ivalue);
               if ( dirFile ) 
               {
                  struct dirent* hFile;
                  // int errno = 0;
                  while (( hFile = readdir( dirFile )) != NULL ) 
                  {
                     if ( !strcmp( hFile->d_name, "."  )) continue;
                     if ( !strcmp( hFile->d_name, ".." )) continue;

                     // in linux hidden files all start with '.'
                     if (hFile->d_name[0] == '.' ) continue;

                     // dirFile.name is the name of the file. Do whatever string comparison 
                     // you want here. Something like:
                     printf("now processing: %s\n", hFile->d_name);
                     if ( strstr( hFile->d_name, ".xml" )) {
                         kanji temp;
                         // char test[] = "test";
                         char dir_file[80];
                         strcat(dir_file,ivalue);
                         strcat(dir_file,hFile->d_name);
                         printf("dirfile %s\n",dir_file);
                         read_xml_file(dir_file,&temp);
                         print_kanji(temp);
                         kanji ex = extract_features(temp, INTERVAL);
                         kjis[i] = ex;
                         i++; 
                     }
                  } 
                  closedir( dirFile );
               }
               // write them to binary file
               char out[] = "data.dat";
               int cnt = i;
               kanjis kjis_out;
               kjis_out.count = cnt;
               kjis_out.arr = (kanji*) malloc(cnt * sizeof(kanji));
               for(int i=0;i<cnt;i++) {
                   kjis_out.arr[i] = kjis[i];
               }
               write_bin_file(kjis_out,out);
               kanjis kjis_bin = read_bin_file((char*) "data.dat");
               for(int i=0;i<kjis_bin.count;i++) {
                   for(int j=0;j<kjis_bin.arr[i].c_strokes;j++) {
                       for(int k=0;k<kjis_bin.arr[i].c_points[j];k++) {
                           int x0 = kjis_bin.arr[i].xy[j][k].x;
                           int y0 = kjis_bin.arr[i].xy[j][k].y;
                           int x1 = kjis[i].xy[j][k].x;
                           int y1 = kjis[i].xy[j][k].y;
                           if((x0 != x1) || (y0 != y1)) { 
                               printf("read-write error\n");
                               exit(1); 
                           }
                       }
                   }
               }
               
        } else {
            fprintf(stderr, "Usage: convert -i XML_DIR\n");
        }return 0;
}
