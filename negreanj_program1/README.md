Make sure the following files are contained within the same directory:

    main.c <- program with main function, creates executable

    movie.h <- movie struct and function declaration
    movie.c <- movie struct and function implementation

    language.h <- language struct and function declaration
    language.c <- language struct and function implementation

Included in this submission is also the sample .csv file given for the assignment, but any file similarly formatted with different data will also work.

Enter the following command to compile the entire program into a single executable file named "movies"

    gcc movie.c language.c main.c -o movies -Wall

Now run the executable with a single argument. This argument will be the file you want to parse. This can be a file with any extension, but it must be formatted according to what was given on the project assignment page. Just include the extension in the name. For example, if you would like to use the included sample file, replace "data_file" with "movies_sample_1.csv" in order to run the executable on that file.

    ./movies data_file

To clean the directory of the executable in order to recompile, run the following command:

    rm -f movies