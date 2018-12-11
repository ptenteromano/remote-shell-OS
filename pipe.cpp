#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

std::string exec(const char* cmd) {
    char buffer[1024];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    if (feof(pipe)) return "Not Found!\n";
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 1024, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int main() {

	std::cout << exec("ls -al\n");
}
