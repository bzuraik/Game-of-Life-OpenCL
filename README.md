# Game-of-Life-OpenCL

Description
An OpenCL program in C that implements the 1-D Game of Life cellular automaton described by J. Millen.
This algorithm creates an image by calculating one row at a time based upon the previous row.
The image is represented by a two dimensional array.
 ------------------------------------------------------------------------------------------------------------
How to compile?

$ make
 ------------------------------------------------------------------------------------------------------------
How to run?

Use oclgrind to run the OpenCL emulator. Run your compiled C program which will run the OpenCL kernel using:

$ oclgrind ./a4 <-n #> <-s #> <-i #>

For example:

$ oclgrind ./a4 -n 80 -s 80 -i 2

  will run the program using 80 kernels, using an array that
  is 80x80 units, and using the initial row configuration two, which means the first row will be a
  Spider.
  
$ oclgrind ./a4 -n 5 -s 20 -i 0 
  will run the program using five kernels, using an array that
  is 20x20 units, and using the initial row configuration zero, which means the first row will be
  random.
  
 ------------------------------------------------------------------------------------------------------------
The command line arguments are:

  -n # - the number represents the number of kernels. 
    The default number of kernels is 1.
    
  -s # - the number represents both the height and width of the array
    The default size of the array is 20.
    
  -i # - the number indicates which initial configuration will be used in the first row of the array.
     The initial configuration is 0 (random).
      The initial configuration patterns and associated command line numbers i for the array are:
      - 0: random pattern. Each element in the row has a 50% chance of being occupied.
      - 1: FlipFlop. Pattern is X XX
      - 2: Spider. Pattern is XXXXXX
      - 3: Glider. Pattern is X XXX
      - 4: Face. Pattern is XXXXXXX
      
      
 ![Face-GOL](https://user-images.githubusercontent.com/95400232/153684036-19d6a532-7880-48e7-95f7-df364a4ca05e.png)

