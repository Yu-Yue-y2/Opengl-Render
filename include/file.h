#ifndef  FILE_H
#define FILE_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
const char* ReadShader(const char* path)
{    
    // 以读模式打开文件
    std::ifstream infile;
    infile.open(path , std::ios::in);
    if (!infile.is_open())
    {
        std::cout << "can't open shader: " << path << std::endl;
        return NULL;
    }
    char* data = new char[9000];
    //infile >> data;
    infile.getline(data, 9000, '\0');
    // 关闭打开的文件
    infile.close();
    return data;
}
#endif // ! FILE_H
