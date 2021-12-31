#define PROGRAM_FILE "GOL.cl"
#define KERNEL_FUNC "GOL"
#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

char** initializeArray(int ARRAYSIZE);
char* initializeArrayOCL(int ARRAYSIZE);

void loadConfigurationSerial(char** myArray, int ARRAYSIZE, int CONFIGURATION);
void printArray(char** myArray, int ARRAYSIZE);
void freeArray(char** myArray, int ARRAYSIZE);

void runConfiguration(char** myArray, int ARRAYSIZE, int currentIteration);
int checkNeighbours(char** myArray, int row, int col, int ARRAYSIZE);

///////////////////////////////////////////////////////////////////////////

//HELPER FUNCTIONS

//Return and initialize a 2D array given ARRAYSIZE
char** initializeArray(int ARRAYSIZE) {
    char** myArray = (char**) malloc(sizeof(char*) * ARRAYSIZE);
    for (int i = 0; i < ARRAYSIZE; i++) {
        myArray[i] = malloc(sizeof(char) * ARRAYSIZE+1);
        for (int j = 0; j < ARRAYSIZE+1; j++) {
            if (j == ARRAYSIZE) {
                myArray[i][j] = '\0';
            //} else if (j == ARRAYSIZE - 1) {
                //myArray[i][j] = 'X';
            //} else if (j == ARRAYSIZE - 2) {
                //myArray[i][j] = 'Y';
            } else {
                myArray[i][j] = '.';
            }
        }
    }
    return myArray;
}

//Return and initialize a 1D array given ARRAYSIZE -- OPENCL READY
char* initializeArrayOCL(int ARRAYSIZE) {

    //In order to represent a 2D array of size n, the 1D array with have size n^2.
    //Add +n to size to insert newline characters and the terminating string character for printing later.
    int totalSize = (ARRAYSIZE * ARRAYSIZE) + ARRAYSIZE;

    char* myArray = malloc(sizeof(char) * totalSize);

    //Counter uses a numerical relationship with ARRAYSIZE to determine when to insert a new line character
    int positionCounter = 1;
    int addNewLineFlag = 0;

    for (int i = 0; i < totalSize; i++) {

        //Add terminating character
        if (i == totalSize - 1) {
            myArray[i] = '\0';
            // printf("Terminating character added at position %d\n", i);
        }
        //Add new line
        else if (addNewLineFlag) {
            addNewLineFlag = 0;
            myArray[i] = '\n';
            // printf("New line character added at position %d\n", i);
            positionCounter++;

        }
        //Prepare to add new line
        else if (positionCounter != 0 && positionCounter % ARRAYSIZE == 0) {
            addNewLineFlag = 1;
            myArray[i] = '.';
            // printf("New line flag set, general character added at position %d\n", i);
        }
        //General case
        else {
            myArray[i] = '.';
            // printf("General character added at position %d\n", i);
            positionCounter++;

        }
    }
    return myArray;
}

//Load a configuration into the array
void loadConfigurationSerial(char** myArray, int ARRAYSIZE, int CONFIGURATION) {


    printf("LOADING CONFIGURATION %d...\n", CONFIGURATION);
    int startingIndex = 0;
    int endingIndex = 0;

    //CONFIGURATIONS:
    //0: Random pattern; each element in the row has a 50% chance of being occupied
    //1. FlipFlop: X XX
    //2. Spider: XXXXXX
    //3. Glider: X XXX
    //4. Face: XXXXXXX
    if (CONFIGURATION == 0) {
        for (int i = 0; i < ARRAYSIZE; i++) {
            double randomValue = (double) rand()/RAND_MAX;
            if (randomValue > 0.5) {
                myArray[0][i] = '1';
            }
        }
    } else if (CONFIGURATION == 1) {
        if (ARRAYSIZE <= 4) {
            startingIndex = 0;
            endingIndex = ARRAYSIZE;
        } else {
            startingIndex = (ARRAYSIZE - 4) - ((ARRAYSIZE - 4)/2);
            endingIndex = startingIndex + 4;
        }

        for (int i = startingIndex; i < endingIndex; i++) {
            if (i != startingIndex + 1) {
                myArray[0][i] = '1';
            }
        }

    } else if (CONFIGURATION == 2) {
        if (ARRAYSIZE <= 6) {
            startingIndex = 0;
            endingIndex = ARRAYSIZE;
        } else {
            startingIndex = (ARRAYSIZE - 6) - ((ARRAYSIZE - 6)/2);
            endingIndex = startingIndex + 6;
        }

        for (int i = startingIndex; i < endingIndex; i++) {
            myArray[0][i] = '1';
        }


    } else if (CONFIGURATION == 3) {
        if (ARRAYSIZE <= 5) {
            startingIndex = 0;
            endingIndex = ARRAYSIZE;
        } else {
            startingIndex = (ARRAYSIZE - 5) - ((ARRAYSIZE - 5)/2);
            endingIndex = startingIndex + 5;
        }

        for (int i = startingIndex; i < endingIndex; i++) {
            if (i != startingIndex + 1) {
                myArray[0][i] = '1';
            }
        }
    } else if (CONFIGURATION == 4) {
        if (ARRAYSIZE <= 7) {
            startingIndex = 0;
            endingIndex = ARRAYSIZE;
        } else {
            startingIndex = (ARRAYSIZE - 7) - ((ARRAYSIZE - 7)/2);
            endingIndex = startingIndex + 7;
        }

        for (int i = startingIndex; i < endingIndex; i++) {
            myArray[0][i] = '1';
        }
    }

}

//Load a configuration into the array -- OPENCL READY
void loadConfigurationOCL(char* myArray, int ARRAYSIZE, int CONFIGURATION) {

    printf("LOADING CONFIGURATION %d --- \n\n", CONFIGURATION);
    int startingIndex = 0;
    int endingIndex = 0;

    //CONFIGURATIONS:
    //0: Random pattern; each element in the row has a 50% chance of being occupied
    //1. FlipFlop: X XX
    //2. Spider: XXXXXX
    //3. Glider: X XXX
    //4. Face: XXXXXXX
    if (CONFIGURATION == 0) {
        for (int i = 0; i < ARRAYSIZE; i++) {
            double randomValue = (double) rand()/RAND_MAX;
            if (randomValue > 0.5) {
                myArray[i] = 'X';
            }
        }
    } else if (CONFIGURATION == 1) {
        if (ARRAYSIZE <= 4) {
            startingIndex = 0;
            endingIndex = ARRAYSIZE;
        } else {
            startingIndex = (ARRAYSIZE - 4) - ((ARRAYSIZE - 4)/2);
            endingIndex = startingIndex + 4;
        }

        for (int i = startingIndex; i < endingIndex; i++) {
            if (i != startingIndex + 1) {
                myArray[i] = 'X';
            }
        }

    } else if (CONFIGURATION == 2) {
        if (ARRAYSIZE <= 6) {
            startingIndex = 0;
            endingIndex = ARRAYSIZE;
        } else {
            startingIndex = (ARRAYSIZE - 6) - ((ARRAYSIZE - 6)/2);
            endingIndex = startingIndex + 6;
        }

        for (int i = startingIndex; i < endingIndex; i++) {
            myArray[i] = 'X';
        }


    } else if (CONFIGURATION == 3) {
        if (ARRAYSIZE <= 5) {
            startingIndex = 0;
            endingIndex = ARRAYSIZE;
        } else {
            startingIndex = (ARRAYSIZE - 5) - ((ARRAYSIZE - 5)/2);
            endingIndex = startingIndex + 5;
        }

        for (int i = startingIndex; i < endingIndex; i++) {
            if (i != startingIndex + 1) {
                myArray[i] = 'X';
            }
        }
    } else if (CONFIGURATION == 4) {
        if (ARRAYSIZE <= 7) {
            startingIndex = 0;
            endingIndex = ARRAYSIZE;
        } else {
            startingIndex = (ARRAYSIZE - 7) - ((ARRAYSIZE - 7)/2);
            endingIndex = startingIndex + 7;
        }

        for (int i = startingIndex; i < endingIndex; i++) {
            myArray[i] = 'X';
        }
    }

}

//Propagate an array after loading a configuration
void runConfiguration(char** myArray, int ARRAYSIZE, int currentIteration) {

    int checkResult = 0;

    if (currentIteration == ARRAYSIZE) {
        printf("CONFIGURATION COMPLETE\n");
        printArray(myArray, ARRAYSIZE);
    } else {
        for (int j = 0; j < ARRAYSIZE; j++) {
            checkResult = checkNeighbours(myArray, currentIteration, j, ARRAYSIZE);
            if (checkResult == 1 && (currentIteration+1) < ARRAYSIZE) {
                myArray[currentIteration+1][j] = '1';
                // printf("Position %d, %d returned 1\n", currentIteration, j);
            } else if (checkResult == 0 && (currentIteration+1) < ARRAYSIZE) {
                myArray[currentIteration+1][j] = '.';
                // printf("Position %d, %d returned 0\n", currentIteration, j);
            }
        }

        runConfiguration(myArray, ARRAYSIZE, currentIteration+1);
    }
}

//Returns a cell's status for the next iteration after calculating its neighbours and self-status
int checkNeighbours(char** myArray, int row, int col, int ARRAYSIZE) {

    int neighbourCount = 0;
    int myStatus = 0;
    if (myArray[row][col] != '.') {
        myStatus = 1;
    }

    // printf("row: %d, col:%d, status: %d\n", row, col, myStatus);

    //Special edge case 1: Leftmost edge
    if(col == 0) {
        if ((col+1 <= ARRAYSIZE-1) && myArray[row][col+1] != '.') {
            neighbourCount++;
        }
        if ((col+2 <= ARRAYSIZE-1) && myArray[row][col+2] != '.') {
            neighbourCount++;
        }
    }

    //Special edge case 2: Left edge
    else if (col == 1) {
        if ((col-1 >= 0) && myArray[row][col-1] != '.') {
            neighbourCount++;
        }
        if ((col+1 <= ARRAYSIZE-1) && myArray[row][col+1] != '.') {
            neighbourCount++;
        }
        if ((col+2 <= ARRAYSIZE-1) && myArray[row][col+2] != '.') {
            neighbourCount++;
        }
    }

    //Special edge case 3: Right edge
    else if (col == ARRAYSIZE-2) {

        if ((col-2 >= 0) && myArray[row][col-2] != '.') {
            neighbourCount++;
        }
        if ((col-1 >= 0) && myArray[row][col-1] != '.') {
            neighbourCount++;
        }
        if ((col+1 <= ARRAYSIZE-1) && myArray[row][col+1] != '.') {
            neighbourCount++;
        }

        // printf("ROW: %d, COL:%d, COUNT: %d\n", row+1, col+1, neighbourCount);

    }

    //Special edge case 4: Rightmost edge
    else if(col == ARRAYSIZE-1) {
        if ((col-1 >= 0) && myArray[row][col-1] != '.') {
            neighbourCount++;
        }
        if ((col-2 >= 0) && myArray[row][col-2] != '.') {
            neighbourCount++;
        }
    }

    //General case
    else {
        if ((col-1 >= 0) && myArray[row][col-1] != '.') {
            neighbourCount++;
        }
        if ((col-2 >= 0) && myArray[row][col-2] != '.') {
            neighbourCount++;
        }
        if ((col+1 <= ARRAYSIZE-1) && myArray[row][col+1] != '.') {
            neighbourCount++;
        }
        if ((col+2 <= ARRAYSIZE-1) && myArray[row][col+2] != '.') {
            neighbourCount++;
        }
    }

    // printf("row: %d, col:%d, status: %d, count: %d\n", row+1, col+1, myStatus, neighbourCount);

    if (myStatus == 1 && (neighbourCount == 2 || neighbourCount == 4)) {
        return 1;
    } else if (myStatus == 0 && (neighbourCount == 2 || neighbourCount == 3)) {
        return 1;
    } else {
        return 0;
    }
}

//Print a 2D char array
void printArray(char** myArray, int ARRAYSIZE) {
    for (int i = 0; i < ARRAYSIZE; i++) {
        for (int j = 0; j < ARRAYSIZE; j++) {
            printf("%c", myArray[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

//Print a 1D char array -- OPENCL READY
void printArrayOCL(char* myArray, int ARRAYSIZE) {
    int totalSize = (ARRAYSIZE * ARRAYSIZE) + ARRAYSIZE;
    for (int i = 0; i < totalSize; i++) {
        printf("%c", myArray[i]);
    }
    printf("\n");
}

//Free a 2D char array
void freeArray(char** myArray, int ARRAYSIZE) {
    for (int i = 0; i < ARRAYSIZE; i++) {
        free(myArray[i]);
    }
    free(myArray);
}

//Free a 1D char array -- OPENCL READY
void freeArrayOCL(char* myArray) {
    free(myArray);
}

///////////////////////////////////////////////////////////////////////////

//OPENCL FUNCTIONS

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {

    cl_platform_id platform;
    cl_device_id dev;
    int err;

    /* Identify a platform */
    err = clGetPlatformIDs(1, &platform, NULL);
    if(err < 0) {
        perror("OPENCL ERROR: Couldn't identify a platform");
        exit(1);
    }

    /* Access a device */
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
    if(err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }
    if(err < 0) {
        perror("OPENCL ERROR: Couldn't access any devices");
        exit(1);
    }

    return dev;

}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("OPENCL ERROR: Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

#endif

int main(int argc, char** argv) {

    //INITIAL SET UP
    int KERNELS = 1;
    int ARRAYSIZE = 20;
    int CONFIGURATION = 0;
    srand(time(NULL));

    //Error checking and command line parameter parsing
    if (argc == 1) {
        ;
    } else if (argc == 2 || argc == 4 || argc == 6 | argc >= 8) {
        printf("USAGE: ./oclgrind [-n #] [-s #] [-i #]\n");
        return 1;
    } else if (argc == 3) {
        if (strcmp(argv[1], "-n") == 0) {
            KERNELS = atoi(argv[2]);
        } else if (strcmp(argv[1], "-s") == 0) {
            ARRAYSIZE = atoi(argv[2]);
        } else if (strcmp(argv[1], "-i") == 0) {
            CONFIGURATION = atoi(argv[2]);
        } else {
            printf("USAGE: ./oclgrind [-n #] [-s #] [-i #]\n");
            return 2;
        }
    } else if (argc == 5) {
        if (strcmp(argv[1], "-n") == 0) {
            KERNELS = atoi(argv[2]);
        } else if (strcmp(argv[3], "-n") == 0) {
            KERNELS = atoi(argv[4]);
        }

        if (strcmp(argv[1], "-s") == 0) {
            ARRAYSIZE = atoi(argv[2]);
        } else if (strcmp(argv[3], "-s") == 0) {
            ARRAYSIZE = atoi(argv[4]);
        }

        if (strcmp(argv[1], "-i") == 0) {
            CONFIGURATION = atoi(argv[2]);
        } else if (strcmp(argv[3], "-i") == 0) {
            CONFIGURATION = atoi(argv[4]);
        }

    } else if (argc == 7) {

        if (strcmp(argv[1], "-n") == 0) {
            KERNELS = atoi(argv[2]);
        } else if (strcmp(argv[3], "-n") == 0) {
            KERNELS = atoi(argv[4]);
        } else if (strcmp(argv[5], "-n") == 0) {
            KERNELS = atoi(argv[6]);
        }

        if (strcmp(argv[1], "-s") == 0) {
            ARRAYSIZE = atoi(argv[2]);
        } else if (strcmp(argv[3], "-s") == 0) {
            ARRAYSIZE = atoi(argv[4]);
        } else if (strcmp(argv[5], "-s") == 0) {
            ARRAYSIZE = atoi(argv[6]);
        }

        if (strcmp(argv[1], "-i") == 0) {
            CONFIGURATION = atoi(argv[2]);
        } else if (strcmp(argv[3], "-i") == 0) {
            CONFIGURATION = atoi(argv[4]);
        } else if (strcmp(argv[5], "-i") == 0) {
            CONFIGURATION = atoi(argv[6]);
        }

    }

    if (CONFIGURATION > 4) {
        printf("Please select from the available configurations:\n0: Random pattern\n1: FlipFlop\n2: Spider\n3: Glider\n4: Face\n");
        return 2;
    }

    /////////////////////////////////////////////////////////////////////////////////////

    ///OPENCL SET UP
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_int i, j, err;
    size_t local_size, global_size;

    /* Create device and context */
    device = create_device();
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if(err < 0) {
        perror("OPENCL ERROR: Couldn't create a context\n");
        exit(1);
    }

    /* Build program */
    program = build_program(context, device, PROGRAM_FILE);

    /* Set number of kernels */
    global_size = KERNELS;
    local_size = KERNELS;


    // Initialize and set up array
    char* oneArray = initializeArrayOCL(ARRAYSIZE);
    loadConfigurationOCL(oneArray, ARRAYSIZE, CONFIGURATION);
    // printArrayOCL(oneArray, ARRAYSIZE);

    // Set up argument array to hold size-related parameters
    int* arguments = malloc(sizeof(int) * 3);
    int ARRAYSIZE_Squared = ARRAYSIZE * ARRAYSIZE;
    int totalSize = ARRAYSIZE_Squared + ARRAYSIZE;
    arguments[0] = ARRAYSIZE;
    arguments[1] = totalSize;
    arguments[2] = KERNELS;
    // printf("ARRAYSIZE: %d, totalSize: %d\n", ARRAYSIZE, totalSize);

    //Set up columns array to hold number of columns responsible for each kernel
    int* columnsResponsible = malloc(sizeof(int) * KERNELS);
    for (int i = 0; i < KERNELS; i++) {
        columnsResponsible[i] = ARRAYSIZE / KERNELS;
    }

    //If number of columns not evenly divisible by number of kernels, last kernel will be responsible for fewer columns
    if (ARRAYSIZE % KERNELS != 0) {
        //Cycle through array and increment column responsible values until remainder is 0
        int remainder = ARRAYSIZE % KERNELS;
        int positionTracker = 0;
        while (remainder != 0) {
            columnsResponsible[positionTracker]++;
            positionTracker++;
            remainder--;
            //Loop back to start
            if (positionTracker == KERNELS) {
                positionTracker = 0;
            }
        }
    }


    /* Prepare data buffers */
    cl_mem myArray = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, (sizeof(char) * totalSize), oneArray, &err);
    cl_mem myArguments = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * 3, arguments, &err);
    cl_mem myColumnsResponsible = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * KERNELS, columnsResponsible, &err);
    if (err < 0) {
        perror("OPENCL ERROR: Couldn't create a buffer");
        exit(1);
    }

    /* Create a command queue */
    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err < 0) {
        perror("OPENCEL ERROR: Couldn't create a command queue");
        exit(1);
    };

    /* Create a kernel */
    kernel = clCreateKernel(program, KERNEL_FUNC, &err);
    if(err < 0) {
        perror("OPENCL ERROR: Couldn't create a kernel");
        exit(1);
    };

    /* Create kernel arguments */
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &myArray);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &myArguments);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &myColumnsResponsible);
    err = clSetKernelArg(kernel, 3, totalSize * sizeof(char), NULL);
    if (err < 0) {
        perror("OPENCL ERROR: Couldn't create a kernel argument");
        exit(1);
    }

    /* Enqueue kernel */
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size,
        &local_size, 0, NULL, NULL);
    if(err < 0) {
        perror("OPENCL ERROR: Couldn't enqueue the kernel");
        exit(1);
    }

    /* Read the kernel's output */
    err = clEnqueueReadBuffer(queue, myArray, CL_TRUE, 0, (sizeof(char) * totalSize), oneArray, 0, NULL, NULL);
    if(err < 0) {
        perror("OPENCL ERROR: Couldn't read the buffer");
        exit(1);
    }

    printArrayOCL(oneArray, ARRAYSIZE);

    /////////////////////////////////////////////////////////////////////////////////////

    /* Deallocate resources */
    free(arguments);
    free(columnsResponsible);
    freeArrayOCL(oneArray);

    clReleaseDevice(device);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    clReleaseMemObject(myArray);
    clReleaseMemObject(myArguments);
    clReleaseMemObject(myColumnsResponsible);

   return 0;
}
