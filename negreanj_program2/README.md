Make sure the following files are contained within the same directory:

    main.c <- program with main function, creates executable

    movie.h <- movie struct and function declaration
    movie.c <- movie struct and function implementation

    language.h <- language struct and function declaration
    language.c <- language struct and function implementation

Enter the following command to compile the entire program into a single executable file named "movies_by_year"

    gcc movie.c language.c main.c -o movies_by_year -Wall

Now run the executable. Make sure that you have valid parseable files to test with.

    ./movies_by_year

To clean the directory of the executable in order to recompile, run the following command:

    rm -f movies_by_year