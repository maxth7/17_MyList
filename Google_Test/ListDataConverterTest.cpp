#include "gtest/gtest.h"
#include "ListDataConverter.h"
#include <sstream>
#include <vector>
#include <memory>


// Тестовый класс
class ListDataConverterTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    void TearDown() override {
    }
};
//--------------- 1 тест -------------------------------------
TEST_F(ListDataConverterTest, LoadValidTextFile) {
    // Arrange
    std:: cout<<"\nTest 1. Successful loading of correct data" << std::endl;
    std::stringstream test_stream;
    test_stream << "data1;0\n";
    test_stream << "data2;1\n";

    std::vector<std::unique_ptr<ListNode>> temp_nodes;
    std::vector<int> temp_indices;

    // Act
    bool result = ListDataConverter::textFileLoader(test_stream, temp_nodes, temp_indices);

    // Assert
    EXPECT_TRUE(result);
    EXPECT_EQ(temp_nodes.size(), 2);
    EXPECT_EQ(temp_indices.size(), 2);
    EXPECT_EQ(temp_indices[0], 0);
    EXPECT_EQ(temp_indices[1], 1);
    EXPECT_EQ(temp_nodes[0]->data, "data1");
    EXPECT_EQ(temp_nodes[1]->data, "data2");
}
//--------------- 2 тест -------------------------------------
TEST_F(ListDataConverterTest, ExceedMaxNodesLimit) {
    //  create a flow  with 1,000,001 rows
    std::cout << "\nTest 2. Exceeding the node limit" << std::endl;
    std::stringstream test_stream;
    for (size_t i = 0; i < ListDataConverter::MAX_NODES + 1; ++i) {
        test_stream << "test_data;" << i << "\n";
    }

    std::vector<std::unique_ptr<ListNode>> temp_nodes;
    std::vector<int> temp_indices;
    
    bool result = ListDataConverter::textFileLoader(test_stream, temp_nodes, temp_indices);

    // the function should return false due to exceeding the limit
    EXPECT_FALSE(result);
    EXPECT_LT(temp_nodes.size(), ListDataConverter::MAX_NODES + 1);  
                                                                     
}
//--------------- 3 тест -------------------------------------
TEST_F(ListDataConverterTest, ExceedMaxDataLength) {
    //  the string is longer than MAX_DATA_LEN
    std::cout << "\nTest 3. Exceeding the maximum data length " << std::endl;

    std::string long_data(ListDataConverter::MAX_DATA_LEN + 10, 'A');
    std::stringstream test_stream;
    test_stream << long_data << ";0\n";

    std::vector<std::unique_ptr<ListNode>> temp_nodes;
    std::vector<int> temp_indices;

    bool result = ListDataConverter::textFileLoader(test_stream, temp_nodes, temp_indices);

    EXPECT_FALSE(result);
    EXPECT_TRUE(temp_nodes.empty());
}
//--------------- 4 тест -------------------------------------
TEST_F(ListDataConverterTest, ParseIndexError01) {
    // Arrange:invalid index
    std::cout << "\nTest 4. Index parsing error. Invalid index=<valid_data; invalid_index\n> " << std::endl;
    //a; pple; 2
    std::stringstream test_stream;
    test_stream << "valid_data;invalid_index\n";  // не число

    std::vector<std::unique_ptr<ListNode>> temp_nodes;
    std::vector<int> temp_indices;

    bool result = ListDataConverter::textFileLoader(test_stream, temp_nodes, temp_indices);

    EXPECT_FALSE(result);
    EXPECT_TRUE(temp_nodes.empty());
    EXPECT_TRUE(temp_indices.empty());
}
//--------------- 5 тест -------------------------------------
TEST_F(ListDataConverterTest, ParseIndexError02) {
    // invalid index
    std::cout << "\nTest 5. Index parsing error. Invalid input string=< a; pple; 2>" << std::endl;
    //a; pple; 2
    std::stringstream test_stream;
    test_stream << "a; pple;;2\n";  // There is no number

    std::vector<std::unique_ptr<ListNode>> temp_nodes;
    std::vector<int> temp_indices;

    bool result = ListDataConverter::textFileLoader(test_stream, temp_nodes, temp_indices);

   
    EXPECT_FALSE(result);
    EXPECT_TRUE(temp_nodes.empty());
    EXPECT_TRUE(temp_indices.empty());
}

//--------------- 6 тест -------------------------------------
TEST_F(ListDataConverterTest, EmptyFile) {
    // empty flow 
    std::cout << "\nTest 6. Empty file " << std::endl;

    std::stringstream empty_stream;

    std::vector<std::unique_ptr<ListNode>> temp_nodes;
    std::vector<int> temp_indices;

    bool result = ListDataConverter::textFileLoader(empty_stream, temp_nodes, temp_indices);

    //  the function returns false if there are no nodes.
    EXPECT_FALSE(result);
    EXPECT_TRUE(temp_nodes.empty());
    EXPECT_TRUE(temp_indices.empty());
}

//--------------- 7 тест -------------------------------------
TEST_F(ListDataConverterTest, SingleLineFile) {

    std::cout << "\nTest 7. A file with a single line" << std::endl;

    std::stringstream test_stream;
    test_stream << "single_data;5\n";

    std::vector<std::unique_ptr<ListNode>> temp_nodes;
    std::vector<int> temp_indices;


    bool result = ListDataConverter::textFileLoader(test_stream, temp_nodes, temp_indices);

    EXPECT_TRUE(result);
    EXPECT_EQ(temp_nodes.size(), 1);
    EXPECT_EQ(temp_indices.size(), 1);
    EXPECT_EQ(temp_indices[0], 5);
    EXPECT_EQ(temp_nodes[0]->data, "single_data");
}



/*
[==========] Running 6 tests from 1 test case.
[----------] Global test environment set-up.
[----------] 6 tests from ListDataConverterTest
[ RUN      ] ListDataConverterTest.LoadValidTextFile

Тест 1. Успешная загрузка корректных данных
Используется текстовый загрузчик
[       OK ] ListDataConverterTest.LoadValidTextFile (2 ms)
[ RUN      ] ListDataConverterTest.ExceedMaxNodesLimit

Тест 2. Превышение лимита узлов
Используется текстовый загрузчик
Лимит узлов достигнут: 1000000
[       OK ] ListDataConverterTest.ExceedMaxNodesLimit (4123 ms)
[ RUN      ] ListDataConverterTest.ExceedMaxDataLength

Тест 3. Превышение максимальной длины данных
Используется текстовый загрузчик
Ошибка: превышена максимальная длина данных (1000 символов) в строке 1. Длина: 1010
[       OK ] ListDataConverterTest.ExceedMaxDataLength (3 ms)
[ RUN      ] ListDataConverterTest.ParseIndexError

Тест 4. Ошибка парсинга индекса
Используется текстовый загрузчик
Ошибка парсинга rand_index
[       OK ] ListDataConverterTest.ParseIndexError (2 ms)
[ RUN      ] ListDataConverterTest.EmptyFile

Тест 5. Пустой файл
Используется текстовый загрузчик
[       OK ] ListDataConverterTest.EmptyFile (1 ms)
[ RUN      ] ListDataConverterTest.SingleLineFile

Тест 6. Файл с одной строкой
Используется текстовый загрузчик
[       OK ] ListDataConverterTest.SingleLineFile (1 ms)
[----------] 6 tests from ListDataConverterTest (4143 ms total)

[----------] Global test environment tear-down
[==========] 6 tests from 1 test case ran. (4149 ms total)
[  PASSED  ] 6 tests.
*/