
CSE 532 Spring 2020 Lab 0

Team:
    Clayton Knittel, c.j.knittel@wustl.edu



Play wrapper facade:
    Abstracts away the parsing of character files into the data structure
representing the play, the printing of the play, and the writing of a play
to the configuration file + character files structure. The parsing of
character files is done in parallel since the Play object is designed to be
thread-safe.

    The parsing of the configuration file is done by first reading in a
configuration file, and for each player file referenced, a new thread is
spawned, and it reads through the given character file, adding lines to
the play structure.

    To then write the play formatted from the internal representation, the
lines are traversed in order (they are stored in an ordered map, indexed by
their line number), and upon a change of character (i.e. the line following
the line just printed belongs to a different character), two newlines are
printed and the character's name is printed, followed by a period.


    I noticed that there was a significant spedup from doing this calculation
in parallel. I was able to test reading in a very large configuration file since
I did the extra credit, and I downloaded the entirety of Hamlet and created a
config file and character files from it. A lot of the work comes from reading from
a file and creating line structs to store character information, but the insertion
of these line structs into the map of lines is fairly cheap, which makes
parallelization work well.





How to unzip and test:

via linux shell (ssh):
    unzip lab0.zip

then open the lab0.sln file with Visual Studio 2019, and either build (^b in
Visual Studio) and run from a windows cmd prompt (at the bottom, after building,
it will tell you where the .exe is output), or run with "local windows debugger".
To specify command line arguments for debugger, go to
Debug > lab0 properties... > Debugging, and under Command Arguments, replace
hamlet_ii_2_config.txt with whatever you'd like to test it on.

The program should read in the config file plus all files it references, and
construct a Play from it, and at the end, it will print out the formatted play.




Extra credit portion:

how to unzip: this was included in lab0.zip, so you can just open the lab0extra.sln
from within the lab0extra directory.

The program will take in arguments
    lab0extra.exe <play file> <output file> <play name...>

and will construct a Play object with the given play name from the play file, and
then write a configuration file and character files in output file (the character
files will be placed in the same directory as the output file, even if the output
file is not in the same directory as the working directory of the executing process)


How this works:

First the formatted play file is read straight into a Play object (I created a
second constructor that takes in the name of the play and a generic ostream,
and it constructs the play line-by-line, similar to how the play is constructed
from character files, but not in parallel). I also created a method within play
that constructs a configuration file and makes a character file for each character
in the play (Play::write_config), which is called after the Play has been
constructed. This iterates through each line of the play, keeping track of the player
files being constructed with an unordered map from character name to std::ofstream.
For any character which has not been visited yet, a new file is opened, named with
their name (spaced replaced by underscores) followed by ".txt". Then, after each
character file has been constructed, the config file is filled in with the play name
and the names of each of the character files.



How to test on full hamlet:

    from Visual Studio 2019, after opening lab0extra.sln, go to
Debug > lab0extra properties... > Debugging, and set command arguments to
hamlet_full.txt out/out.txt Hamlet, then run by clicking Local Windows
Debugger. This will construct the configuration file from the formatted Play
and output the config file with all of the character files in the directory
"out", which is at the same level as all of the .cpp source files.

    To then read this play in from the other program, pass in these arguments
to the Local Windows Debugger:
../lab0extra/out/out.txt

This will then print the entire play of Hamlet (which may take a second).




