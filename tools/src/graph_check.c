/****************************************************************************
 *
 *
 * Copyright (c) 2012 Dolby International AB.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED
 * BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @file
 * @brief  Comparison utility for operating on the output of SATS tools
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void print_usage(void);


int main(int argc, char *argv[])
{
    // first go though all the options and configure tool
    int i;
    char filename1[1024] = "\0";
    char filename2[1024] = "\0";
    double max = 0.0, min = 0.0, total = 0.0, deviation = 0.0;
    double xmin = 0.0, xmax = 0.0;
    double total_deviation = 0.0;
    double x1_field = 0, y1_field = 0, x2_field = 0, y2_field = 0;
    int max_enabled = 0, min_enabled = 0, total_enabled = 0;
    int xmin_enabled = 0, xmax_enabled = 0;
    int quiet_mode = 0, verbose_mode = 0;
    char result[10] = "Pass";
    FILE *fid1, *fid2;
    char line1[256];
    char line2[256];
    char line21[256];
    char line22[256];
    char *split;
    unsigned int line_no = 1;
    unsigned int expect_num_comp = 0;
    int num_comp = 0;
    int min_num_comp = 0;

    for (i = 1 ; i < argc ; i++)
    {
        // check to see if its a filename first
        if (argv[i][0] != '-')
        {
            // Have we got the first filename yet
            if (strlen(filename1) == 0)
            {
                strcpy(filename1,argv[i]);
            }
            else
            {
                // Have we got the second filename yet
                if (strlen(filename2) > 0)
                {
                    print_usage();
                    fprintf(stderr,"Error: Too many filenames specified\n");
                    exit(-2);
                }
                else
                {
                    strcpy(filename2,argv[i]);
                }
            }
        }

        if (!strcmp(argv[i],"-min"))
        {
            if (i == (argc - 1))
            {
                print_usage();
                fprintf(stderr, "Error: Can't find min value\n");
                exit(-1);
            }
            min = atof(argv[i+1]);
            min_enabled = 1;
            // We increment i here to step over the next parameter
            // which has been parsed as the value
            i++;
        }

        if (!strcmp(argv[i],"-max"))
        {
            if (i == (argc - 1))
            {
                print_usage();
                fprintf(stderr, "Error: Can't find max value\n");
                exit(-1);
            }
            max = atof(argv[i+1]);
            max_enabled = 1;
            // We increment i here to step over the next parameter
            // which has been parsed as the value
            i++;
        }

        if (!strcmp(argv[i],"-xmin"))
        {
            if (i == (argc - 1))
            {
                print_usage();
                fprintf(stderr, "Error: Can't find xmin value\n");
                exit(-1);
            }
            xmin = atof(argv[i+1]);
            xmin_enabled = 1;
            // We increment i here to step over the next parameter
            // which has been parsed as the value
            i++;
        }

        if (!strcmp(argv[i],"-xmax"))
        {
            if (i == (argc - 1))
            {
                print_usage();
                fprintf(stderr, "Error: Can't find xmax value\n");
                exit(-1);
            }
            xmax = atof(argv[i+1]);
            xmax_enabled = 1;
            // We increment i here to step over the next parameter
            // which has been parsed as the value
            i++;
        }

        if (!strcmp(argv[i],"-dev"))
        {
            if (i == (argc - 1))
            {
                print_usage();
                fprintf(stderr, "Error: Can't find max average value\n");
                exit(-1);
            }
            total = atof(argv[i+1]);
            total_enabled = 1;
            // We increment i here to step over the next parameter
            // which has been parsed as the value
            i++;
        }

        if (!strcmp(argv[i],"-n"))
        {
            if (i == (argc - 1))
            {
                print_usage();
                fprintf(stderr, "Error: Can't find #comparisons value\n");
                exit(-4);
            }
            expect_num_comp = atof(argv[i+1]);
            // We increment i here to step over the next parameter
            // which has been parsed as the value
            i++;
        }

        if (!strcmp(argv[i],"-nmin"))
        {
            if (i == (argc - 1))
            {
                print_usage();
                fprintf(stderr, "Error: Can't find minimum #comparisons value\n");
                exit(-5);
            }
            min_num_comp = atof(argv[i+1]);
            // We increment i here to step over the next parameter
            // which has been parsed as the value
            i++;
        }

        if (!strcmp(argv[i],"-v"))
        {
            verbose_mode = 1;
        }

    }

    // Check for condition where no filenames were specified
    if ((strlen(filename1) + strlen(filename2)) == 0)
    {
        print_usage();
        fprintf(stderr, "Error: No filename specified\n");
        exit(-3);
    }

    // Check for negative comparisons number
    if (expect_num_comp < 0)
    {
        print_usage();
        fprintf(stderr, "Error: Invalid expected comparison specified\n");
        exit(-7);
    }

    // Check for negative comparisons number
    if (min_num_comp < 0)
    {
        print_usage();
        fprintf(stderr, "Error: Invalid minimum comparison specified\n");
        exit(-8);
    }

    // Check for negative comparisons number
    if ((expect_num_comp > 0) && (min_num_comp > 0))
    {
        print_usage();
        fprintf(stderr, "Error: Both minimum and exact number of comparisons specified\n");
        exit(-9);
    }

    // If in verbose mode then print out all the parsed values from
    // the command line
    if (verbose_mode)
    {
        printf("First file: %s\n",filename1);
        if (strlen(filename2) > 0)
        {
            printf("Second file:%s\n",filename2);
        }
        if (min_enabled)
        {
            printf("Using min value of %3.2f\n",min);
        }
        if (max_enabled)
        {
            printf("Using max value of %3.2f\n",max);
        }
        if (xmin_enabled)
        {
            printf("Using xmin value of %3.2f\n",xmin);
        }
        if (xmax_enabled)
        {
            printf("Using xmax value of %3.2f\n",xmax);
        }
        if (total_enabled)
        {
            printf("Using max average value of %3.2f\n",total);
        }
        if (expect_num_comp > 0)
        {
            printf("Using expected comparisons value of %u\n",expect_num_comp);
        }
        if (min_num_comp > 0)
        {
            printf("Using minimum comparisons value of %u\n",min_num_comp);
        }
    }

    // Select either single or double file mode
    if (strlen(filename2) == 0)
    {
        // Single file mode
        if (!(min_enabled || max_enabled))
        {
            fprintf(stderr,"Max or Min levels must be specified in single file mode\n");
            print_usage();
            exit(-3);
        }

        if (total_enabled)
        {
            fprintf(stderr,"Warning: Ignoring total value as single file mode selected\n");
        }

        fid1 = fopen(filename1,"r");
        if (fid1 == NULL)
        {
            fprintf(stderr,"Error: File not found\n");
            print_usage();
            exit(-4);
        }
        line_no = 1;
        // Loop until end of file
        while(!feof(fid1))
        {
            fscanf(fid1,"%s%s\n",line1,line2);
            //If, for some reason, either of the strings are empty then
            // we have a problem
            if ((strlen(line1) == 0) || (strlen(line2) == 0))
            {
                fprintf(stderr,"Error: Formatting problem in %s at line %u\n",
                        filename1, line_no);
                fclose(fid1);
                exit(-6);
            }
            x1_field = atof(line1);
            y1_field = atof(line2);

            if ((!xmin_enabled || (x1_field >= xmin)) &&
                (!xmax_enabled || (x1_field <= xmax)))
            {
                if (min_enabled && (y1_field < min))
                {
                    if (!quiet_mode)
                    {
                        printf("Minimum of %3.2f violated by value of %3.2f at line %u in %s\n",
                               min, y1_field, line_no, filename1);
                    }
                    strcpy(result,"Fail");
                }
                
                if (max_enabled && (y1_field > max))
                {
                    if (!quiet_mode)
                    {
                        printf("Maximum of %3.2f violated by value of %3.2f at line %u in %s\n", max, y1_field, line_no, filename1);
                    }
                    strcpy(result,"Fail");
                }
                num_comp++;
            }
            line_no++;
        }
    }
    else
    {
        // Dual file mode
        if (!(min_enabled || max_enabled || total_enabled))
        {
            fprintf(stderr,"Max, Min or Max average levels must be specified in dual file mode\n");
            print_usage();
            exit(-3);
        }

        fid1 = fopen(filename1,"r");
        fid2 = fopen(filename2,"r");
        if (fid1 == NULL)
        {
            fprintf(stderr,"Error: File %s not found\n",filename1);
            print_usage();
            exit(-4);
        }
        if (fid2 == NULL)
        {
            fprintf(stderr,"Error: File %s not found\n",filename2);
            print_usage();
            exit(-4);
        }
        line_no = 1;
        total_deviation = 0.0;
        while(!feof(fid1))
        {
            fscanf(fid1,"%s%s\n",line1,line2);
            fscanf(fid2,"%s%s\n",line21,line22);
            // If, for some reason, any of the parsed strings are empty
            // we have a problem
            if ((strlen(line1) == 0) || (strlen(line2) == 0))
            {
                fprintf(stderr,"Error: Formatting problem in %s at line %u\n",
                        filename1, line_no);
                fclose(fid1);
                fclose(fid2);
                exit(-6);
            }
            if ((strlen(line21) == 0) || (strlen(line22) == 0))
            {
                fprintf(stderr,"Error: Formatting problem in %s at line %u\n",
                        filename2, line_no);
                fclose(fid1);
                fclose(fid2);
                exit(-6);
            }
            x1_field = atof(line1);
            y1_field = atof(line2);
            x2_field = atof(line21);
            y2_field = atof(line22);
            deviation = fabs(y1_field - y2_field);

            if ((!xmin_enabled || (x1_field >= xmin)) &&
                (!xmax_enabled || (x1_field <= xmax)))
            {
                total_deviation += deviation;

                if (!quiet_mode &&
                    (x1_field != x2_field))
                {
                    printf("X fields don't match at line %u (%3.2f,%3.2f)\n",
                           line_no, x1_field, x2_field);
                }

                if (min_enabled && (deviation < min))
                {
                    if (!quiet_mode)
                    {
                        printf("Min. deviation limit of %3.2f violated by %3.2f and %3.2f at line %u\n", min, y1_field, y2_field, line_no);
                    }
                    strcpy(result,"Fail");
                }

                if (max_enabled && (deviation > max))
                {
                    if (!quiet_mode)
                    {
                        printf("Max. deviation limit of %3.2f violated by %3.2f and %3.2f at line %u\n", max, y1_field, y2_field, line_no);
                    }
                    strcpy(result,"Fail");
                }
                num_comp++;
            }
            line_no++;
        }

        // Scale down according to the number of lines
        total_deviation = total_deviation / (double)line_no;
        if (total_enabled && !quiet_mode)
        {
            printf("Average deviation: %f\n",total_deviation);
        }
        if (total_enabled && (total_deviation > total))
        {
            if (!quiet_mode)
            {
                printf("Average deviation of %3.2f violates maximum limit of %3.2f\n",
                       total_deviation,total);
            }
            strcpy(result,"Fail");
        }
    }

    if ((expect_num_comp > 0) && (expect_num_comp != num_comp))
    {
        if (!quiet_mode)
        {
            printf("%u Comparisons made doesn't match expected value of %u\n",
                   num_comp, expect_num_comp);
        }
        strcpy(result,"Fail");
    }   

    if ((num_comp < min_num_comp))
    {
        if (!quiet_mode)
        {
            printf("%u comparisons violates minimum value of %u\n",
                   num_comp, min_num_comp);
        }
        strcpy(result,"Fail");
    }   

    printf("%s\n",result);
    if (strcmp(result, "Pass"))
    {
        return(1);
    }
    else
    {
        return(0);
    }
}
            
void print_usage(void)
{
    fprintf(stderr,"graph_check [OPTION]... FILE1 [FILE2]\n\n");
    fprintf(stderr,"Copyright Dolby Laboratories 2006-2008 v1.1\n");
    fprintf(stderr,"-xmin <xmin_val>           minimum value of 1st column to check\n");
    fprintf(stderr,"-xmax <xmax_val>           maxmimum value of 1st column to check\n");
    fprintf(stderr,"-n,                        exact number of comparision required\n");
    fprintf(stderr,"-nmin,                     minimum number of comparision required\n");

    fprintf(stderr,"Single File Mode:\n");
    fprintf(stderr,"-min <min_val>,            minimum limit of 2nd column\n");
    fprintf(stderr,"-max <max_val>,            maximum limit of 2nd column\n");
    fprintf(stderr,"Dual File Mode:\n");
    fprintf(stderr,"-min <min_val>,            minimum deviation of 2nd columns\n");
    fprintf(stderr,"-max <max_val>,            maximum deviation of 2nd columns\n");
    fprintf(stderr,"-dev,                      maximum average deviation\n");
    fprintf(stderr,"-v,                        verbose mode\n\n");
    fprintf(stderr,"This tools is intended to operate on the standard output of the SATS tools\n");
    fprintf(stderr,"The standard output of the SATS tools when used in text mode should be\n");
    fprintf(stderr,"redirected to a file. These files can be specified in the command line\n");
    fprintf(stderr,"as FILE1 and FILE2. Single file mode is for comparing against fixed limits.\n");
    fprintf(stderr,"Dual file mode is intended for comparing results against a refence set of\n");
    fprintf(stderr,"results that have been previously generated.\n");
    fprintf(stderr,"Single or multiple limits may be used as required.\n\n");

}

