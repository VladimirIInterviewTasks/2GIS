#include <iostream>
#include <fstream>

#include "argparser.hpp"
#include "node.hpp"

int main(int agrc, char **argv)
{

    ArgParser input(agrc, argv);
    if (input.cmdOptionExists("-h"))
    {
        std::cout << "app -i input -o output" << std::endl;
        return 0;
    }

    Node n;

    std::string readFilename = input.getCmdOption("-i");
    std::string writeFilename = input.getCmdOption("-o");

    if (!readFilename.empty())
    {
        std::size_t end = 0;
        std::ifstream inputFile(readFilename, std::ios::binary);
        if (!inputFile.is_open())
        {
            return 0;
        }
        inputFile.seekg(0, std::ios::end);
        end = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);
        std::vector<std::byte> data(end);
        inputFile.read(reinterpret_cast<char *>(data.data()), end);
        n.deserialize(data);
        std::cout << n;
        inputFile.close();
    }
    else
    {
        n = {std::string("Hello"), {{std::string("world")}, {std::string("simple node")}, {1, {{2}, {3}}}}};
    }

    if (!writeFilename.empty())
    {
        std::ofstream outputFile(writeFilename, std::ios::binary);
        std::vector<std::byte> data;
        n.serialize(data);
        outputFile.write(reinterpret_cast<char *>(data.data()), data.size());
        outputFile.close();
    }

    return 0;
}