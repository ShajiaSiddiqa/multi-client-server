// functions / operations server can perform 

#ifndef JOB_PROCESSOR_HPP
#define JOB_PROCESSOR_HPP

#include <cstddef>
#include <string>
#include "./utils.hpp"


extern std::vector<std::string> JOB_TYPES; 

class JobProcessor {
public:
    static JobResult process(const File& file);
    
private:
    // Individual jobs
    static JobResult countWords(const File& file);
    static JobResult extractEmails(const File& file);
    static JobResult csvToTsv(const File& file);


    // Bonus jobs
    static JobResult compress(const File& file);
    static JobResult decompress(const File& file);
    static JobResult base64Encode(const File& file);
    static JobResult base64Decode(const File& file);

};


#endif // JOB_PROCESSOR_HPP
