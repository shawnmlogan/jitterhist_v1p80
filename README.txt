5.	Program Installation

The current version of jitterhist is v1.73 and its source and documentation are available in the compressed tar file “jitterhist_v1p73_091324.tar.gz”. The most recent and stable version may be downloaded from reference [5]. To install the program and create the executable “jitterhist”, enter Example command line [3] in the directory in which you wish to locate the program.

			$ tar -xvzf jitterhist_v1p73_091324.tar.gz		Example command line [3]

This will create a directory jitterhist_v1p73 and extract its directory structure:

			Documentation/			include/					src/
			README.txt				plotting_routines/
			example/					jitterhist@

Navigate to the “src” subdirectory, and issue the following two UNIX commands:

			$ make													Example command line [4]	
			$ make clean											Example command line [5]

Issuing these two commands will create the executable “jitterhist” and delete object files no longer needed. In addition, Example command line [4] will attempt to create a symbolic link to jitterhist in your $HOME/bin directory if this directory exists. Assuming your $HOME/bin directory is contained in your executable search path (UNIX PATH variable), this will allow you to execute jitterhist from any directory using a command line syntax similar to Example command line [1] in Section 4.1.

References:

[5]	https://1drv.ms/u/s!AnM-GsAEZPoSsistH2ga2HiOVW6h?e=XOh71c

9/13/2024

