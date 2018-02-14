To compile, type:
make

To run, type:
./a.out <filename>

NOTE: My program accepts floating point coordinates, but immediately convertes them to integers. Therefore, what is written back to the file will always be integers.

Every time you want to transform a polygon, specify a viewport, change the line drawing algorithm, or write the updated scene to the text file, simply press any key and the command line will prompt you for specific actions. If you incorrectly type a character, I'm not sure what will happen because I did not concern myself with handling incorrect input. 

Quitting the program without writing back the most recent scene will not do it automatically. As such, you must save the scene manually each time you want to write back to the textfile.

1 known issue: For some complex polygons, if you drag it out of the scene too much it might segfault. However this only happens with complex polygons that have subtle special cases.

Enjoy!
Ovi

