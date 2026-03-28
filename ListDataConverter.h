#pragma once
#include "ListNode.h"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <cstddef>

class ListDataConverter {
private:

    // 1. Member data
    std::vector<std::unique_ptr<ListNode>> nodes;
    std::vector<int> rand_indices;

    static const size_t MAX_NODES = 1000000;
    static const size_t MAX_DATA_LEN = 1000;

    // 2. Auxiliary utilities
    void clearNodes() noexcept;
    size_t find_record_end(const std::string& content, size_t start) const noexcept;

    static std::string escapeSpecialChars(const std::string& s);
    static std::string escapeForInput(const std::string& s);

    // 3. ÂInternal loading methods

    using LoaderFunc = std::function<bool(
        std::ifstream&,
        std::vector<std::unique_ptr<ListNode>>&,
        std::vector<int>&
        )>;
    // 4.Private download methods
    // A single internal upload method
    bool loadFromFileInternal(const std::string& filename, LoaderFunc loader);

    static bool textFileLoader(
        std::istream& file,
        std::vector<std::unique_ptr<ListNode>>& temp_nodes,
        std::vector<int>& temp_indices
    );

    static bool binaryFileLoader(
        std::ifstream& file,
        std::vector<std::unique_ptr<ListNode>>& temp_nodes,
        std::vector<int>& temp_indices
    );

    bool buildLinksFromVectors(
        std::vector<std::unique_ptr<ListNode>>& temp_nodes,
        const std::vector<int>& temp_indices
    ) noexcept;
   

public:
   
    // 5. Destructor
     ~ListDataConverter();
    // 6. Public upload/Save methods
    
    bool loadFromTextFile(const std::string& filename);
    bool loadFromBinaryFile(const std::string& filename);
    ListNode* deserializeFromFile(const std::string& filename);

    bool serializeToFile(const std::string& filename) const;

    bool rebuildLinks() noexcept;
    // 7.Public methods of working with the list
      void printStats() const;

    // 8. Public static utilities 
    static  void printToConsole(ListNode* head);
    static bool save_TXT_Format(ListNode* head, const std::string& filename);

};

