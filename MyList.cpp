// 06_MyList.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "ListDataConverter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <locale>

void printUsage(const char* program) {
    std::cout << "Using:: " << program << " [the input file] [the output file]" << std::endl;
    std::cout << std::endl;
    std::cout << "  input file - file with list description (default: inlet.in)" << std::endl;
    std::cout << "  output file - file for serialization (default: outlet.out)" << std::endl;
    std::cout << std::endl;
    std::cout << "Input file format:" << std::endl;
    std::cout << "  <data>;<rand_index>" << std::endl;
    std::cout << "  где rand_index = -1 если rand == nullptr" << std::endl;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    std::cout << "Визуальная проверка на крякозяблы" << std::endl << std::endl;
    std::string input_file = "inlet.in";
    std::string output_file = "outlet.out";
    std::string recovered_file = "recovered_from_binary.txt";

    if (argc > 3) {
        printUsage(argv[0]);
        return 1;
    }

    if (argc >= 2) {
        input_file = argv[1];
    }

    if (argc >= 3) {
        output_file = argv[2];
    }

    std::cout << "Serialization and deserialization of a doubly linked list with arbitrary links" << std::endl;
    std::cout << "==============================================================================" << std::endl;
    std::cout << "The input file :     " << input_file << std::endl;
    std::cout << "The output file:     " << output_file << std::endl;
    std::cout << "Restored from "<< output_file<<" the output file: " << recovered_file << std::endl;
    std::cout << std::endl;

    ListDataConverter listHandler;

    // PART 1: SERIALIZATION
    std::cout << "Step 1. Parsing: reading" + input_file + " and creating list nodes in memory." << std::endl;
    std::cout << "=============================================================================" << std::endl;

    std::cout << "      [Step 1/3] Loading data from a text file " << input_file << std::endl;
    std::cout.flush();

    if (!listHandler.loadFromTextFile(input_file)) {
        std::cerr << "\nError loading text file" << std::endl;
        return 1;
    }
   
    std::cout << "Successful data loading from a text file" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    
 
    std::cout << "      [Step 2/3]. Building connections and displaying the list in the console"<<std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    std::cout.flush();
   
    if (!listHandler.rebuildLinks()) {  // Новый метод
        std::cerr << "\nError while building connections" << std::endl;
        return 1;
    }
    std::cout << "------------------------------------------------------" << std::endl;
    
    

    listHandler.printStats();

    std::cout << "      [Step 4/4]. Serialization: saving a list from memory to a binary file " << output_file << std::endl;
    std::cout.flush();
    if (!listHandler.serializeToFile(output_file)) {  
        std::cerr << "\nSerialization error" << std::endl;
        return 1;
    }
    std::cout << "Serialization is successfully complete." << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    // ЧАСТЬ 2: ДЕСЕРИАЛИЗАЦИЯ
    std::cout << "\nStep 2. Deserialization and export." << std::endl;
    std::cout << "==========================================================================" << std::endl;


    std::cout << "      Step [1/2]. Loading data from a binary file " << output_file << std::endl;
    std::cout.flush();

    ListNode* restored_head = listHandler.deserializeFromFile(output_file);

    if (!restored_head) {
        std::cerr << "\nError during deserialization" << std::endl;
        return 1;
    }
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "The deserialization was successful." << std::endl;

    //std::cout << "Шаг [2/3]. Визуализация и экспорт." << std::endl;

    std::cout << "      Step [2/3]. Output to console:" << std::endl;
    listHandler.printToConsole(restored_head);

    std::cout << "      Step [3/3]. Saving to the input file in text format " << input_file<< ": "<< recovered_file;

    ListDataConverter::save_TXT_Format(restored_head, recovered_file);
      
    // ЧАСТЬ 3: СРАВНЕНИЕ ФАЙЛОВ
    std::cout << "\nSTEP 3: Comparing files." << std::endl;
    std::cout << "=========================================================================" << std::endl;

    std::ifstream original(input_file, std::ios::binary | std::ios::ate);
    std::ifstream reconstructed(recovered_file, std::ios::binary | std::ios::ate);

    if (original.is_open() && reconstructed.is_open()) {
        std::streamsize original_size = original.tellg();
        std::streamsize reconstructed_size = reconstructed.tellg();

        std::cout << "----------------------------------------" << std::endl;

        std::cout << "|The size of the source file:  " << original_size << " bytes|" << std::endl;
        std::cout << "|The size of the restored:     " << reconstructed_size << " bytes|" << std::endl;
        std::cout << "----------------------------------------" << std::endl;


        if (original_size == reconstructed_size) {
            std::cout << "File sizes  \"" << input_file
                << "\" and  \"" << recovered_file << "\"  are the same" << std::endl; 
            std::cout << "2 files have been created:" << std::endl;
            std::cout << "      1. " << output_file << " (binary serialization)" << std::endl;
            std::cout << "      2. " << recovered_file << " (restored text format)" << std::endl;
            std::cout << "The program has been successfully completed." << std::endl;
        }
        else {
            std::cout << "File Sizes \"" << input_file
                << "\" and  \"" << recovered_file << "\" differ?" << std::endl;
            std::cout << "We have a problem.?" << std::endl;
            std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

        }
    }
    else {
        std::cout << "Couldn't open files for comparison" << std::endl;
        
        return 0;
    }
    std::cout << "==========================================================================" << std::endl;
  
    return 0;
}
