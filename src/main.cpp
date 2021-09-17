#include "depend.hpp"


#include "Data_structs.hpp"
#include "Board.hpp"
#include "Utility.hpp"
#include "Search.hpp"
#include "Bitboard.hpp"
#include "Evaluation.hpp"
#include "Transposition_table.hpp"
#include "Opening_book.hpp"
#include "Engine.hpp"
#include "UCI.hpp"
#include "Thread.hpp"
#include "tests.hpp"


std::string directory_from_file(std::string str) {
    if (str.back() == '/') {
        str.pop_back();
    }
    int index = str.size() - 1;
    for (auto it = str.rbegin(); it != str.rend(); it++) {
        if ((*it) == '/') {
            break;
        }
        index--;
    }
    return str.substr(0, index + 1);
}

// Define resource_path();
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
std::string resource_path() {
    char pBuf[1024];
    size_t len = sizeof(pBuf);
    int bytes = GetModuleFileName(NULL, pBuf, len);
    if (bytes) {
        std::string str = std::string(pBuf);
        str = str.substr(0, str.size() - 14);
        str += "Resources\\";
        return str;
    }
    else {
        std::cout << "Buffer too small\n";
        abort();
    }
}
#elif __APPLE__

#include <TargetConditionals.h>

#if TARGET_OS_MAC

#include <mach-o/dyld.h>

std::string resource_path() {
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        std::string str = std::string(path);
        str = directory_from_file(str);
        str += "Resources/";
        return str;
    } else {
        std::cout << "Buffer too small\n";
        abort();
    }
}

#else
#   error "Incompatible Apple platform"
#endif
#elif __linux__
std::string resource_path() {
    int bytes = MIN(readlink("/proc/self/exe", pBuf, len), len - 1);
    if(bytes >= 0) {
        pBuf[bytes] = '\0';
        std::string str = std::string(pBuf);
        str = directory_from_file(str);
        str += "Resources/";
        return str;
    }
    else {
        std::cout << "Buffer too small\n";
        abort();
    }
}
#else
#   error "Unknown compiler"
#endif

int main() {

    init_uci();

    init_bitboard_utils();
    init_eval_utils();
    init_ray_gen();
    init_zobrist_bitstrings();
    init_opening_book();

    tests();


    // Synchronization utils
    Thread::SafeQueue<std::vector<std::string>> cmd_queue;
    std::atomic<bool> should_end_search(false);

    Engine engine(cmd_queue, should_end_search);
    UCI uci(cmd_queue, should_end_search);

    std::thread t = engine.spawn();
    uci.loop();
    t.join();

    return 0;
}
