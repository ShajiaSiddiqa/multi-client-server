#include "../include/jobs.hpp"
#include <sstream>
#include <regex>
#include <vector>
#include <cstdlib>

std::vector<std::string> JOB_TYPES = {
    "count-words",
    "extract-emails",
    "csv-to-tsv",
    "compress",
    "decompress",
    "base64-encode",
    "base64-decode"
};

// ============= Helper: Convert char* to std::string =============
static std::string toString(const File& file) {
    return std::string(file.content, file.size);
}

// ============= count-words =================
JobResult JobProcessor::countWords(const File& file) {
    std::string text = toString(file);
    std::istringstream iss(text);
    int count = 0;
    std::string word;
    while (iss >> word) count++;
    
    return {std::to_string(count), "count-words"};
}

// ============= extract-emails =================
JobResult JobProcessor::extractEmails(const File& file) {
    std::string text = toString(file);
    std::regex emailRegex(R"(([\w\.-]+)@([\w\.-]+)\.([\w\.]{2,}))");

    std::sregex_iterator it(text.begin(), text.end(), emailRegex);
    std::sregex_iterator end;

    std::ostringstream oss;
    while (it != end) {
        oss << (*it)[0].str() << "\n";
        ++it;
    }

    return {oss.str(), "extract-emails"};
}

// ============= csv-to-tsv =================
JobResult JobProcessor::csvToTsv(const File& file) {
    std::string text = toString(file);
    std::ostringstream oss;
    bool insideQuotes = false;

    for (char c : text) {
        if (c == '"') {
            insideQuotes = !insideQuotes;
        } else if (c == ',' && !insideQuotes) {
            oss << '\t';  // Replace comma with tab
        } else {
            oss << c;
        }
    }

    return {oss.str(), "csv-to-tsv"};
}


// ================================================================
// ============= Bonus: Compression (RLE) =================
JobResult JobProcessor::compress(const File& file) {
    std::string text = toString(file);
    std::ostringstream oss;

    for (size_t i = 0; i < text.size();) {
        char c = text[i];
        size_t count = 0;
        while (i < text.size() && text[i] == c) {
            ++count;
            ++i;
        }
        oss << c << count;
    }

    return {oss.str(), "compress"};
}

// ============= Bonus: Decompression (RLE) =================
JobResult JobProcessor::decompress(const File& file) {
    std::string text = toString(file);
    std::ostringstream oss;

    for (size_t i = 0; i < text.size();) {
        char c = text[i++];
        std::string num;
        while (i < text.size() && std::isdigit(text[i])) {
            num.push_back(text[i++]);
        }
        int count = std::stoi(num);
        oss << std::string(count, c);
    }

    return {oss.str(), "decompress"};
}


// ================================================================
// ============= Bonus: Base64 Encode/Decode =================
static const std::string BASE64_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string base64EncodeStr(const std::string &input) {
    std::string output;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            output.push_back(BASE64_CHARS[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) output.push_back(BASE64_CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
    while (output.size() % 4) output.push_back('=');
    return output;
}

static std::string base64DecodeStr(const std::string &input) {
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[BASE64_CHARS[i]] = i;

    std::string output;
    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return output;
}

JobResult JobProcessor::base64Encode(const File& file) {
    return {base64EncodeStr(toString(file)), "base64-encode"};
}

JobResult JobProcessor::base64Decode(const File& file) {
    return {base64DecodeStr(toString(file)), "base64-decode"};
}


// ============= Dispatcher =================
JobResult JobProcessor::process(const File& file) {
    std::string job(file.job_type);

    if (job == "count-words") {
        return countWords(file);
    }
    else if (job == "extract-emails") {
        return extractEmails(file);
    }
    else if (job == "csv-to-tsv") {
        return csvToTsv(file);
    }
    else if (job == "compress") {
        return compress(file);
    }
    else if (job == "decompress") {
        return decompress(file);
    }
    else if (job == "base64-encode") {
        return base64Encode(file);
    }
    else if (job == "base64-decode") {
        return base64Decode(file);
    }

    return {"Unsupported job type", "error"};
}
