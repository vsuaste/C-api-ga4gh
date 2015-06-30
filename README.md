# testing_rapidJson

For compiling and creating shared library:

-Add rapidJson submodule

 *`git submodule add https://github.com/miloyip/rapidjson.git`*

-Compile .cpp wrapper

  *`g++ -c -Wall -Werror -fPIC -I/your_path_directory/rapidjson/include ga4gh_wrapper.cpp`*
  
-Create shared library

  *`g++ -shared -o libga4gh.so ga4gh_wrapper.o`*
  
-Compile C library and main

  *`gcc -L/your_path_directory -Wall search_variants.c main.c ga4gh_client.c -o ga4gh-tools -lga4gh -lcurl`*
  
-Add current directory path to the environment variable LD_LIBRARY_PATH

  *`export LD_LIBRARY_PATH=/your_path_directory:$LD_LIBRARY_PATH`*
  
  
Run examples:

  *`./ga4gh-tools --server ga4gh --variantSetIds 1kg-phase1 -referenceName 2 -s 33100 -e 34000`*  
  
This command will give you the "1kg-phase1.vcf" file as output.

  *`./ga4gh-tools --help`* 
  
This command will display information about how to use the tool.
