# testing_rapidJson

For compiling and creating shared library:

-Compile .cpp wrapper
  g++ -c -Wall -Werror -fPIC ga4gh_wrapper.cpp
-Create shared library
  g++ -shared -o libga4gh.so ga4gh_wrapper.o
-Compile C library and main
  gcc -L/your_path_directory -Wall main.c ga4gh_library.c -o library_c -lga4gh -lcurl
  
-Add current directory path to the environment variable LD_LIBRARY_PATH
  export LD_LIBRARY_PATH=/your_path_directory:$LD_LIBRARY_PATH
  
Run examples:
  ./library_c --server ga4gh  
This command will give you the "test_ga4gh.vcf" as output.

  ./library_c --server ebi 
This command will give you the "test_ebi.vcf" as output.
