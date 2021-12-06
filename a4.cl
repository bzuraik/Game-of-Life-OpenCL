//C Program -> Work Group (Global Rank) -> Work Items (Local Rank)

//#pragma OPENCL EXTENSION cl_intel_printf : enable

//Driving function for each work group; accomplishes a number of tasks including runConfiguration and checkNeighbours functions
//myArguments[0] = ARRAYSIZE
//myArguments[1] = totalSize
__kernel void a4(__global char* myArrayGlobal, __global int* myArguments, __global int* myColumnsResponsible, __local char* myArray) {

    //Global rank is relative to the total number of work-items running the same kernel
    uint myRank = get_global_id(0);

    //Move global array to private memory
    for (int i = 0; i < myArguments[1]; i++) {
        myArray[i] = myArrayGlobal[i];
    }

    //Determine starting and ending indices for each work group based on their rank
    int rankCounter = 0;
    int startingIndex = 0;
    while (rankCounter != myRank) {
        startingIndex += myColumnsResponsible[rankCounter++];
    }
    int endingIndex = startingIndex + myColumnsResponsible[myRank] - 1;


    //Same logic as runConfiguration but inserted here; propagate the array using their rank value
    for (int i = 1; i < myArguments[0]; i++) {
        startingIndex += myArguments[0] + 1;
        endingIndex = startingIndex + myColumnsResponsible[myRank] - 1;

        int oneRowOffSet = myArguments[0] + 1;
        for (int j = startingIndex; j <= endingIndex; j++) {

            //Same logic as checkNeighbours but inserted here; returns a cell's status for the next iteration after calculating its neighbours and self-status
            int neighbourCount = 0;
            int myStatus = 0;
            if (myArray[j - oneRowOffSet] != '.') {
                myStatus = 1;
            }

            //Slightly altered checks for iteration 1
            if (i == 1) {
                //Special edge case 1: Leftmost edge
                if (j - oneRowOffSet == 0) {
                    if ((j+1 <= myArguments[1]-1) && myArray[j+1-oneRowOffSet] != '.' && myArray[j+1-oneRowOffSet] != '\n' && myArray[j+1-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                    if ((j+2 <= myArguments[1]-1) && myArray[j+2-oneRowOffSet] != '.' && myArray[j+2-oneRowOffSet] != '\n'&& myArray[j+2-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                }

                //Special edge case 2: Left edge
                else if (j - oneRowOffSet - 1 == 0) {
                    if (myArray[0] != '.' && myArray[j-1-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j+1 <= myArguments[1]-1) && myArray[j+1-oneRowOffSet] != '.' && myArray[j+1-oneRowOffSet] != '\n' && myArray[j+1-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                    if ((j+2 <= myArguments[1]-1) && myArray[j+2-oneRowOffSet] != '.' && myArray[j+2-oneRowOffSet] != '\n'&& myArray[j+2-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                }

                //Special edge case 3: Right edge
                else if (j+2 <= (myArguments[0] * myArguments[0]) && myArray[j+2-oneRowOffSet] == '\n' || myArray[j+2-oneRowOffSet] == '\0') {

                    if ((j-2 >= 0) && myArray[j-2-oneRowOffSet] != '.' && myArray[j-2-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j-1 >= 0) && myArray[j-1-oneRowOffSet] != '.' && myArray[j-1-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j+1 <= myArguments[1]-1) && myArray[j+1-oneRowOffSet] != '.' && myArray[j+1-oneRowOffSet] != '\n' && myArray[j+1-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                }

                //Special edge case 4: Rightmost edge
                else if (j+1 <= (myArguments[0] * myArguments[0]) && myArray[j+1-oneRowOffSet] == '\n' || myArray[j+1-oneRowOffSet] == '\0') {
                    if ((j-1 >= 0) && myArray[j-1-oneRowOffSet] != '.' && myArray[j-1-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j-2 >= 0) && myArray[j-2-oneRowOffSet] != '.' && myArray[j-2-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                }

                //General case
                else {
                    if ((j-1 >= 0) && myArray[j-1-oneRowOffSet] != '.' && myArray[j-1-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j-2 >= 0) && myArray[j-2-oneRowOffSet] != '.' && myArray[j-2-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j+1 <= myArguments[1]-1) && myArray[j+1-oneRowOffSet] != '.' && myArray[j+1-oneRowOffSet] != '\n' && myArray[j+1-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                    if ((j+2 <= myArguments[1]-1) && myArray[j+2-oneRowOffSet] != '.' && myArray[j+2-oneRowOffSet] != '\n'&& myArray[j+2-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                }
            } else {
                //Special edge case 1: Leftmost edge
                if (j-1 >= 0 && myArray[j-1-oneRowOffSet] == '\n') {
                    if ((j+1 <= myArguments[1]-1) && myArray[j+1-oneRowOffSet] != '.' && myArray[j+1-oneRowOffSet] != '\n' && myArray[j+1-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                    if ((j+2 <= myArguments[1]-1) && myArray[j+2-oneRowOffSet] != '.' && myArray[j+2-oneRowOffSet] != '\n'&& myArray[j+2-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                }

                //Special edge case 2: Left edge
                else if (j-2 >= 0 && myArray[j-2-oneRowOffSet] == '\n') {
                    if ((j-1 >= 0) && myArray[j-1-oneRowOffSet] != '.' && myArray[j-1-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j+1 <= myArguments[1]-1) && myArray[j+1-oneRowOffSet] != '.' && myArray[j+1-oneRowOffSet] != '\n' && myArray[j+1-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                    if ((j+2 <= myArguments[1]-1) && myArray[j+2-oneRowOffSet] != '.' && myArray[j+2-oneRowOffSet] != '\n'&& myArray[j+2-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                }

                //Special edge case 3: Right edge
                else if (j+2 <= (myArguments[0] * myArguments[0]) && myArray[j+2-oneRowOffSet] == '\n' || myArray[j+2-oneRowOffSet] == '\0') {

                    if ((j-2 >= 0) && myArray[j-2-oneRowOffSet] != '.' && myArray[j-2-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j-1 >= 0) && myArray[j-1-oneRowOffSet] != '.' && myArray[j-1-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j+1 <= myArguments[1]-1) && myArray[j+1-oneRowOffSet] != '.' && myArray[j+1-oneRowOffSet] != '\n' && myArray[j+1-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                }

                //Special edge case 4: Rightmost edge
                else if (j+1 <= (myArguments[0] * myArguments[0]) && myArray[j+1-oneRowOffSet] == '\n' || myArray[j+1-oneRowOffSet] == '\0') {
                    if ((j-1 >= 0) && myArray[j-1-oneRowOffSet] != '.' && myArray[j-1-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j-2 >= 0) && myArray[j-2-oneRowOffSet] != '.' && myArray[j-2-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                }

                //General case
                else {
                    if ((j-1 >= 0) && myArray[j-1-oneRowOffSet] != '.' && myArray[j-1-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j-2 >= 0) && myArray[j-2-oneRowOffSet] != '.' && myArray[j-2-oneRowOffSet] != '\n') {
                        neighbourCount++;
                    }
                    if ((j+1 <= myArguments[1]-1) && myArray[j+1-oneRowOffSet] != '.' && myArray[j+1-oneRowOffSet] != '\n' && myArray[j+1-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                    if ((j+2 <= myArguments[1]-1) && myArray[j+2-oneRowOffSet] != '.' && myArray[j+2-oneRowOffSet] != '\n'&& myArray[j+2-oneRowOffSet] != '\0') {
                        neighbourCount++;
                    }
                }
            }

            if (myStatus == 1 && (neighbourCount == 2 || neighbourCount == 4)) {
                if (myArguments[2] >= 10) {
                    myArray[j] = 88;
                } else {
                    myArray[j] = myRank + 48;
                }
            } else if (myStatus == 0 && (neighbourCount == 2 || neighbourCount == 3)) {
                if (myArguments[2] >= 10) {
                    myArray[j] = 88;
                } else {
                    myArray[j] = myRank + 48;
                }
            } else {
                myArray[j] = '.';
            }

            myArrayGlobal[j] = myArray[j];
        }

        //A synchronization point -- forces all work-items to reach this point before any of them can continue
        barrier(CLK_GLOBAL_MEM_FENCE);
    }
}