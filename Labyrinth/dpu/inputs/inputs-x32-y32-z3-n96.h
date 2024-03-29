#ifndef _INPUTS_H_
#define _INPUTS_H_

#define NUM_PATHS 96
#define RANGE_X 32
#define RANGE_Y 32
#define RANGE_Z 3

__mram int PATHS[NUM_PATHS][6] =
{
    {27,24,1,8,16,1},
    {25,9,1,18,29,1},
    {9,24,1,8,29,2},
    {25,28,0,23,28,2},
    {15,3,1,19,29,2},
    {15,27,0,25,17,0},
    {23,12,2,21,0,1},
    {27,7,0,27,6,1},
    {7,30,2,14,2,0},
    {16,29,0,17,22,1},
    {26,17,2,19,18,1},
    {19,12,1,9,6,0},
    {19,21,1,2,24,2},
    {29,26,2,29,17,1},
    {22,8,2,27,28,1},
    {30,18,1,21,31,2},
    {25,2,1,15,20,2},
    {7,23,0,7,25,0},
    {26,3,0,22,1,1},
    {29,17,2,0,20,1},
    {18,12,1,31,1,0},
    {30,5,0,6,25,2},
    {0,13,0,8,7,1},
    {11,5,1,1,3,2},
    {6,11,2,26,29,0},
    {21,30,0,21,27,1},
    {8,19,1,5,15,1},
    {18,16,0,11,26,0},
    {17,0,2,10,1,0},
    {7,30,1,9,11,2},
    {20,19,2,12,13,1},
    {0,6,1,7,20,1},
    {28,18,1,12,22,1},
    {21,1,1,8,5,1},
    {15,17,2,28,15,0},
    {14,25,2,25,6,2},
    {20,2,2,31,12,2},
    {10,6,2,0,26,1},
    {3,3,1,27,8,2},
    {3,27,1,2,8,1},
    {25,27,0,16,20,1},
    {27,8,0,1,21,1},
    {30,30,2,1,23,2},
    {20,22,2,20,11,1},
    {6,18,0,4,10,2},
    {22,10,1,1,5,2},
    {9,12,1,9,15,0},
    {1,5,1,2,12,0},
    {13,3,2,15,26,2},
    {10,15,2,13,9,2},
    {28,29,1,12,31,1},
    {2,2,2,1,0,1},
    {16,14,1,18,21,1},
    {11,31,0,24,13,1},
    {2,27,2,28,14,2},
    {3,12,0,1,30,0},
    {4,6,1,17,4,2},
    {31,4,1,21,28,1},
    {29,10,1,15,21,0},
    {19,7,1,30,28,2},
    {1,4,0,25,26,1},
    {22,25,0,2,27,0},
    {7,1,0,27,10,0},
    {4,20,2,16,28,1},
    {18,21,2,24,31,2},
    {28,6,1,19,26,1},
    {25,12,1,27,25,1},
    {0,5,1,8,2,2},
    {30,9,1,25,1,1},
    {4,9,2,1,1,1},
    {15,27,2,21,4,2},
    {13,19,1,1,15,0},
    {1,19,1,3,17,1},
    {12,24,1,28,19,1},
    {20,10,0,21,19,2},
    {4,29,2,29,27,2},
    {25,16,2,6,25,1},
    {24,14,2,2,1,2},
    {15,28,2,21,18,0},
    {2,26,2,24,22,1},
    {9,3,1,18,29,2},
    {13,3,2,23,0,1},
    {21,0,2,30,23,0},
    {2,11,0,11,0,2},
    {26,2,2,6,6,2},
    {30,3,0,11,0,1},
    {27,5,0,11,30,0},
    {30,11,1,9,29,2},
    {20,5,2,3,18,0},
    {28,30,2,10,7,2},
    {9,13,0,4,0,0},
    {2,13,1,23,4,1},
    {20,2,1,11,30,0},
    {13,13,2,10,6,0},
    {15,30,2,8,17,2},
    {25,14,1,24,13,0}
};

// __mram int PATHS[1][6] = {
//     {0, 0, 1, 5, 5, 1},
// };

// __mram int PATHS[1][6] = {
//     {0, 0, 1, 5, 5, 1},
// };

// cat file.txt | tr -s ' ' ", #" | cut -d ', ' -f 2-7 | awk '{print "{"$0"},"}'

#endif /* _INPUTS_H_ */
