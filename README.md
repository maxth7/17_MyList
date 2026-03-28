#17_MyList
Solution of a test task (TT) from a very good and generous employer.<br>
called "Serialization of a doubly linked list with arbitrary links"<br>
TT completed within the specified time.<br>
Employer noted the following positive aspects and approaches in my solution:
1. **Structure integrity**: The `ListNode` structure has not been modified (only the pointers `prev`, `next`, `rand`, and `std::string data` are provided).
2. **File I/O**: The program reads `inlet.in` and creates `outlet.out`.
3. **Deserialization**: Full-fledged deserialization is implemented (not required by the task, which is a big advantage).
4. **Memory management**: Smart pointers `std::unique_ptr` are used — no leaks.
5. **Data types**: Platform-independent types (`uint32_t`, `int32_t`) in serialization.
6. **Binary format**: Optimal structure (number of nodes, string lengths, data, rand-indexes).
7. **Serialization logic**: Saves rand reference indices instead of raw pointers.
8. **Code organization**: Divided into `.h`/`cpp` files:
    - `ListNode.h`
    - `ListDataConverter.h`
    - `ListDataConverter.cpp`
    - `MyList.cpp`
9. **Loader design**: Functional approach with `LoaderFunc` (`std::function`).
10. **Error handling**: Centralized with `bool` return.
11. **Safety**: `MAX_NODES` and `MAX_DATA_LEN` limits against overflow.
12. **Link restoration**: Rand links restored correctly during deserialization.
13. **Link building**: `rebuildLinks()` and `buildLinksFromVectors()` build `prev`/`next`/`rand` links correctly.
14. **Diagnostics**: Statistics printed via `printStats()` (node count, data sizes, rand links).
15. **Verification**: Restored list saved to `recovered_from_binary.txt`.
