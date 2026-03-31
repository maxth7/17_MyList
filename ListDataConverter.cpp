
#include "ListDataConverter.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <set>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <algorithm>
// ============= PRIVATE =============
   
    // 1. Auxiliary utilities
    void ListDataConverter::clearNodes() noexcept {
        nodes.clear();
        rand_indices.clear();
    }
    // Searches for the end of the current record (the position of the '\n' character, which is the record separator)
    size_t ListDataConverter::find_record_end(const std::string& content, size_t start) const noexcept {
        size_t newline_pos = content.find('\n', start);
        if (newline_pos != std::string::npos) {
            return newline_pos;
        }
        return content.length() - 1;
    }

std::string ListDataConverter::escapeSpecialChars(const std::string& s) {
        std::string result;
        for (char c : s) {
            switch (c) {
            case '\n': result += "\\n"; break;
            case '\t': result += "\\t"; break;
            case '\r': result += "\\r"; break;
            case '\0': result += "\\0"; break;
            case '\\': result += "\\\\"; break;
            case ';': result += "\\;"; break;
            default:
                if (isprint(static_cast<unsigned char>(c))) {
                    result += c;
                }
                else {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\x%02X", static_cast<unsigned char>(c));
                    result += buf;
                }
            }
        }
        return result;
    }

    std::string ListDataConverter::escapeForInput(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '\\' || c == ';') {
                result += '\\';
            }
            result += c;
        }
        return result;
    }
    // 2.Internal loading methods
    
    bool ListDataConverter::loadFromFileInternal(const std::string& filename, LoaderFunc loader) {
        // 1. 1. Open the file (same for all formats)
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: couldn't open the file " << filename << std::endl;
            return false;
        }

        // 2.Clearing the previous data
        clearNodes();

        // 3. Creating temporary structures
        std::vector<std::unique_ptr<ListNode>> temp_nodes;
        std::vector<int> temp_indices;

        // 4. CALLING THE TRANSFERRED LOADER FUNCTION
        if (!loader(file, temp_nodes, temp_indices)) {
            return false;
        }

        // 5. Post-processing (common for all formats)
        if (!buildLinksFromVectors(temp_nodes, temp_indices))  {
            return false;
        }

        // 6. Transfer of ownership
        nodes = std::move(temp_nodes);
        rand_indices = std::move(temp_indices);

        return true;
    }

    // Specific strategy 1: loading from a text file
  bool ListDataConverter::textFileLoader(
        std::istream& file,
        std::vector<std::unique_ptr<ListNode>>& temp_nodes,
        std::vector<int>& temp_indices
    ) {
        std::cout << "A text loader is being used" << std::endl;

        std::string full_content;
        std::string line;
        
        size_t pos = 0;
        size_t end_pos = 0;
        std::string data = "";
        std::string rand_str = "";
        size_t line_number = 0;
      
        while (std::getline(file, line)) {
            
            if (temp_nodes.size() >= MAX_NODES) {
                std::cerr << "The node limit has been reached: " << MAX_NODES << std::endl;
                return false;
            }

            line_number += 1;

                size_t last_semicolon = line.find(';');
            if (!line.empty()) {
              
                if (last_semicolon == std::string::npos) {
                    data = data + line.substr(0, last_semicolon)+'\n';//010=0A
            
                }
                else {
            
                     rand_str = line.substr(last_semicolon + 1);
                     data = data + line.substr(0, last_semicolon);
            
                    try {
                        int rand_idx = std::stoi(rand_str);
                   
                        auto node = std::make_unique<ListNode>();
                        node->data = data;

                        if (data.size() > MAX_DATA_LEN) {
                            std::cerr << "Error: Maximum data length ("
                                << MAX_DATA_LEN << "  characters) in the string is exceeded."
                                << line_number << " Length: " << data.size() << std::endl;
                            return false;
                        }

                        data = "";

                        temp_nodes.push_back(std::move(node));
                        temp_indices.push_back(rand_idx);
                        
                        
                    }
                    catch (...) {
                        std::cerr << "Error: problem parsing rand_index" << std::endl;
                        return false;
                    }
                }
            }
            pos = end_pos + 1;
        }

        return !temp_nodes.empty();
    }

    // Specific strategy 2: loading from a binary file
    bool ListDataConverter::binaryFileLoader(
        std::ifstream& file,
        std::vector<std::unique_ptr<ListNode>>& temp_nodes,
        std::vector<int>& temp_indices
    ) {
        std::cout << "Binary bootloader is used" << std::endl;

        uint32_t node_count;
        file.read(reinterpret_cast<char*>(&node_count), sizeof(node_count));

        if (file.gcount() != sizeof(node_count)) {
            std::cerr << "Error reading the number of nodes" << std::endl;
            return false;
        }

        if (node_count > MAX_NODES) {
            std::cerr << "The number of nodes ("<< MAX_NODES<<") has been exceeded : " << node_count << std::endl;
            return false;
        }

        temp_nodes.reserve(node_count);
        temp_indices.resize(node_count, -1);

        for (uint32_t i = 0; i < node_count; ++i) {
            uint32_t data_len;
            file.read(reinterpret_cast<char*>(&data_len), sizeof(data_len));

            if (file.gcount() != sizeof(data_len)) {
                std::cerr << "Data length reading error" << std::endl;
                return false;
            }

            if (data_len > MAX_DATA_LEN) {
                std::cerr << "Data size limit("<< MAX_DATA_LEN<<") exceeded: " << data_len<<std::endl;
                return false;
            }

            auto node = std::make_unique<ListNode>();
            node->data.resize(data_len);
            file.read(&node->data[0], data_len);

            if (static_cast<uint32_t>(file.gcount()) != data_len) {
                std::cerr << "Data reading error" << std::endl;
                return false;
            }

            temp_nodes.push_back(std::move(node));
        }

        for (uint32_t i = 0; i < node_count; ++i) {
            int32_t rand_index;
            file.read(reinterpret_cast<char*>(&rand_index), sizeof(rand_index));

            if (file.gcount() != sizeof(rand_index)) {
                std::cerr << "Error reading the rand index" << std::endl;
                return false;
            }

            temp_indices[i] = rand_index;
        }

        return true;
    }

    // General method for building connections (private, with parameters)
    bool ListDataConverter::buildLinksFromVectors(
        std::vector<std::unique_ptr<ListNode>>& temp_nodes,
        const std::vector<int>& temp_indices
    ) noexcept {
        if (temp_nodes.empty()) return false;

        for (size_t i = 0; i < temp_nodes.size(); ++i) {
            if (i > 0) temp_nodes[i]->prev = temp_nodes[i - 1].get();


            if (i < temp_nodes.size() - 1) temp_nodes[i]->next = temp_nodes[i + 1].get();
        

        }

        for (size_t i = 0; i < temp_nodes.size(); ++i) {
            int idx = temp_indices[i];
            if (idx >= 0 && idx < static_cast<int>(temp_nodes.size())) {
                temp_nodes[i]->rand = temp_nodes[idx].get();

                
            }
            
        }

        return true;
    }
    // ============= PUBLIC ==============

    ListDataConverter::~ListDataConverter() {
        clearNodes();
    }
    // 5. Basic serialization/deserialization methods
    // Public methods
    bool ListDataConverter::loadFromTextFile(const std::string& filename) {
        return loadFromFileInternal(filename, textFileLoader);
    }

    bool ListDataConverter::loadFromBinaryFile(const std::string& filename) {
        return loadFromFileInternal(filename, binaryFileLoader);
    }

    ListNode* ListDataConverter::deserializeFromFile(const std::string& filename) {
        return loadFromBinaryFile(filename) ? nodes[0].get() : nullptr;
    }

    // Serialization to a binary file
    bool ListDataConverter::serializeToFile(const std::string& filename) const{
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Failed to create file " << filename << std::endl;
            return false;
        }


        if (nodes.empty()) {
            std::cerr << "Error: empty list for serialization" << std::endl;
            file.close(); 
            return false;
        }

        if (nodes.size() > MAX_NODES) {
            std::cerr << "Ошибка: слишком много узлов (" << nodes.size()
                << " > " << MAX_NODES << ")" << std::endl;
            file.close();
            return false;
        }


        // unordered_map for O(1) access
        std::unordered_map<ListNode*, uint32_t> node_to_index;
        node_to_index.reserve(nodes.size());  


        for (size_t i = 0; i < nodes.size(); ++i) {
            node_to_index[nodes[i].get()] = static_cast<uint32_t>(i);
        }

        uint32_t node_count = static_cast<uint32_t>(nodes.size());
        file.write(reinterpret_cast<const char*>(&node_count), sizeof(node_count));

        for (size_t i = 0; i < nodes.size(); ++i) {
            const std::string& data = nodes[i]->data;
            uint32_t data_len = static_cast<uint32_t>(data.size());
            file.write(reinterpret_cast<const char*>(&data_len), sizeof(data_len));
            file.write(data.c_str(), data_len);
        }

        for (size_t i = 0; i < nodes.size(); ++i) {
            int32_t rand_index = -1;
            if (nodes[i]->rand) {
                auto it = node_to_index.find(nodes[i]->rand);
                if (it != node_to_index.end()) {
                    rand_index = static_cast<int32_t>(it->second);
                }
            }
            file.write(reinterpret_cast<const char*>(&rand_index), sizeof(rand_index));
        }

        file.close();
        std::cout << "Serialized nodes:" << nodes.size() << " to file " << filename << std::endl;
        return true;
    }
    
    bool ListDataConverter::rebuildLinks() noexcept {
        if (nodes.empty()) {
            return true; 
        }

        // 1. Building prev/next connections
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (i > 0) {
                nodes[i]->prev = nodes[i - 1].get();
            }
            else {
                nodes[i]->prev = nullptr;
            }

            if (i < nodes.size() - 1) {
                nodes[i]->next = nodes[i + 1].get();
            }
            else {
                nodes[i]->next = nullptr;
            }
        }

        // 2. Building rand connections
        for (size_t i = 0; i < nodes.size(); ++i) {
            int idx = rand_indices[i];
            // control of the maximum number in rand
            if (idx >= 0 && idx < static_cast<int>(nodes.size())) {
                nodes[i]->rand = nodes[idx].get();
                std::cout << "Node " << i << ": rand -> Node " << idx << std::endl;
            }
            else if (idx == -1) {
                nodes[i]->rand = nullptr;
                std::cout << "Node " << i << ": rand = null" << std::endl;
            }
            else {
                std::cerr << "Error: incorrect rand_index=" << idx
                    << " for node " << i << std::endl;
                return false;
            }
        }

        return true;
    }

    // 7. Static utilities
    void ListDataConverter::printStats() const {
        std::cout << "Statistics:" << std::endl;

        if (nodes.empty()) {
            std::cout << "    The list is empty" << std::endl;
            std::cout << "    Total nodes: 0" << std::endl;
            std::cout << "    Total amount of data: 0 bytes" << std::endl;
            std::cout << "    Max.line length : 0 bytes" << std::endl;
            
            return;
        }

        std::cout << "    Total nodes: " << nodes.size() << std::endl;

        size_t total_data_size = 0;
        size_t max_data_size = 0;
        int rand_count = 0;

        for (size_t i = 0; i < nodes.size(); ++i) {
            total_data_size += nodes[i]->data.size();
            max_data_size = std::max(max_data_size, nodes[i]->data.size());
            if (nodes[i]->rand) rand_count++;

        }

        std::cout << "    Total amount of data: " << total_data_size << " bytes" << std::endl;
        std::cout << "    Max. line length: " << max_data_size << " bytes" << std::endl;
        std::cout << "    Nodes with rand: " << rand_count << std::endl;
       
    }


     void ListDataConverter::printToConsole(ListNode* head) {
        if (!head) {
            std::cout << "The list is empty\n";
            return;
        }

        
        //  unordered_map for O(1) access
        std::unordered_map<ListNode*, int> nodeToIndex;
        nodeToIndex.reserve(1000000);  


        ListNode* current = head;
        int index = 0;
        while (current) {
            nodeToIndex[current] = index++;
            current = current->next;
        }

        std::cout << "Contents of the list:\n";
        std::cout << "---------------------\n";

        current = head;
        while (current) {
            std::cout << "Node " << nodeToIndex[current] << ": ";
            std::cout << "data=\"" << escapeSpecialChars(current->data) << "\"";

            if (current->prev) std::cout << ", prev=" << nodeToIndex[current->prev];
            else std::cout << ", prev=null";

            if (current->next) std::cout << ", next=" << nodeToIndex[current->next];
            else std::cout << ", next=null";

            if (current->rand) std::cout << ", rand=" << nodeToIndex[current->rand];
            else std::cout << ", rand=null";

            std::cout << "\n";
            current = current->next;
        }
        std::cout << "---------------------\n";
    }

     bool ListDataConverter::save_TXT_Format(ListNode* head, const std::string& filename) {

        std::ofstream file(filename, std::ios::binary);
      
        if (!file.is_open()) {
            std::cerr << "Error: Failed to create file " << filename << std::endl;
            return false;
        }

      
        // unordered_map for O(1) access
        std::unordered_map<ListNode*, int> nodeToIndex;
        nodeToIndex.reserve(1000000);


        ListNode* current = head;
        int index = 0;
        while (current) {
            nodeToIndex[current] = index++;
            current = current->next;
        }

        current = head;
        bool isFirstLine = true;

        while (current) {
            std::string escapedData = escapeForInput(current->data);
            int randIdx = (current->rand) ? nodeToIndex[current->rand] : -1;

            if (!isFirstLine) {
              
#if defined(_WIN32) || defined(_WIN64)
                file << "\r\n";  // Windows
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
              file << "\n";    // Unix/Linux/macOS
#else
              file << "\n";    // By default
#endif

            }

            file << escapedData << ";" << randIdx;
            isFirstLine = false;

            current = current->next;
        }

        file.close();
        std::cout << "\nData has been successfully saved in the input file format  inlet.in: " << filename << std::endl;
        return true;
    }



