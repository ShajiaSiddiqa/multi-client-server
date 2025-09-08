#include <string>
#include <vector>
#include <cstddef>

// ====================== File Object ======================

struct File {
    char* content;
    size_t size;
    char* job_type;
};

// ====================== JobResult ========================
struct JobResult {
    std::string result;
    std::string job_type;
};

// ====================== ClientResult ========================
struct Request 
{
    uint64_t id;
    int client_fd;
    std::string job_type;
    std::vector<char> payload;
};