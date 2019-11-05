/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-10-31 16:33
 * Last modified : 2019-11-06 03:12
 * Filename      : 
 * Description   : 
 **********************************************************/
#include <unistd.h>
#include "../jackson/jackson/FileReadStream.h"
#include "../jackson/jackson/Document.h"
#include "ServerStubGenerator.h"


void writeStubToFile(StubGenerator* generator)
{
    printf("start write stub to file\n");
    std::string outputName = generator->genStubClassName() + ".h";
    FILE *out = fopen(outputName.c_str(), "w");
    if(out == NULL)
    {
        fprintf(stderr, "StubGenerator Error\n");
        exit(1);
    }

    auto stub = generator->genStub();
    fputs(stub.c_str(), out);
    printf("success write stub to file\n");
}


void genStub(FILE* input)
{
    // 解析json
    printf("start parse json\n");
    json::FileReadStream is(input);
    json::Document proto;
    json::ParseError err = proto.parseStream(is);
    if(err != json::PARSE_OK)
    {
        fprintf(stderr, "parse error: %s\n", json::parseErrorStr(err));
        exit(1);
    }
    printf("success parse json\n");
    try
    {
        ServerStubGenerator* serverStubGenerator = new ServerStubGenerator(proto);
        writeStubToFile(serverStubGenerator);
        //ClientStubGenerator* clientStubGenerator = new ClientStubGenerator(proto);
        //writeStubToFile(clientStubGenerator);
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "input error: %s\n", e.what());
        exit(1);
    }
    
}




int main(int argc, char** argv)
{
    const char *inputFileName = NULL;
    if(argc > 1)
    {
        inputFileName = argv[1];
    }
    else
    {
        fprintf(stderr, "unknow file name\n");
        exit(1);
    }

    FILE* input = fopen(inputFileName, "r");
    if(input == nullptr)
    {
        fprintf(stderr, "open file %c error\n", inputFileName);
        exit(1);
    }

    genStub(input);
    return 0;
}