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
#include "rasterize.h"

// read all xml files from a directory
// extract features from kanji, and save to
// first argument
int read_directory(kanji kjis[], char* directory) {

    printf("Given input directory: %s\n\n",directory);
    int i = 0;
    DIR* dirFile = opendir(directory);
    if (dirFile) {
        struct dirent* hFile;
        while ((hFile = readdir(dirFile)) != NULL) {
            if (!strcmp(hFile->d_name, ".")) continue;
            if (!strcmp(hFile->d_name, "..")) continue;

            // in linux hidden files all start with '.'
            if (hFile->d_name[0] == '.') continue;

            // dirFile.name is the name of the file. 
            // compare to check xml ending
            if (strstr(hFile->d_name, ".xml")) {
                kanji temp;
                // char test[] = "test";
                char dir_file[255];
                strcpy(dir_file, directory);
                strcat(dir_file, "/");
                strcat(dir_file, hFile->d_name);
                printf("now processing: %s\n", dir_file);
                read_xml_file(dir_file, &temp);
                // first raster to get intermediate
                // points and increase resolution
                // then moment normalize, and then
                // raster again to get intermediate points
                // lost due to normalization
                kanji temp_r = raster(temp);
                moment(temp_r);
                kanji temp_rmr = raster(temp_r);
                kanji ex = extract_features(temp_rmr, INTERVAL);
                kjis[i] = ex;
                i++;
            }
        }
        closedir(dirFile);
    }
    return i;
}

// a crude check to verify two sets of kanjis
// note: no check if same number of strokes 
// and points

void verify_kanjis(kanjis kjis_a, kanji kjis_b[]) {

    for (int i = 0; i < kjis_a.count; i++) {
        if(kjis_a.arr[i].kji != kjis_b[i].kji) {
            printf("read-write error: not the same kanji\n");
            exit(1);
        }
        for (int j = 0; j < kjis_a.arr[i].c_strokes; j++) {
            for (int k = 0; k < kjis_a.arr[i].c_points[j]; k++) {
                int x0 = kjis_a.arr[i].xy[j][k].x;
                int y0 = kjis_a.arr[i].xy[j][k].y;
                int x1 = kjis_b[i].xy[j][k].x;
                int y1 = kjis_b[i].xy[j][k].y;
                if ((x0 != x1) || (y0 != y1)) {
                    printf("read-write error: not the same point\n");
                    exit(1);
                }
            }
        }
    }
}


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
            // char ivalue[] = "/Users/user/Documents/Code/convert/xmls/";
            kanji kjis[2300];
            int cnt = read_directory(kjis,ivalue);
            
            // write them to binary file
            // write_bin_file expects datatype
            // kanjis, so first transform the array
            char out[] = "data.dat";
            kanjis kjis_out;
            kjis_out.count = cnt;
            kjis_out.arr = (kanji*) malloc(cnt * sizeof(kanji));
            for(int i=0;i<cnt;i++) {
                kjis_out.arr[i] = kjis[i];
            }
            
            write_bin_file(kjis_out,out);
            
            // read them back in for verification
            // 
            kanjis kjis_bin = read_bin_file((char*) "data.dat");
            verify_kanjis(kjis_bin, kjis);
            
            
               
        } else {
            fprintf(stderr, "Usage: convert -i XML_DIR\n");
        }return 0;
}
