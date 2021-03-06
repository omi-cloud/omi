/*
**==============================================================================
**
** Copyright (c) Microsoft Corporation. All rights reserved. See file LICENSE
** for license information.
**
**==============================================================================
*/

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

const char* arg0;

static bool Inhale(const char* path, vector<char>& data)
{
    FILE* is = fopen(path, "r");

    if (!is)
        return false;

    size_t n;
    char buf[4096];

    while ((n = fread(buf, 1, sizeof(buf), is)) != 0)
    {
        data.insert(data.end(), buf, buf + n);
    }

    fclose(is);

    return true;
}

static bool WriteFileFromBuffer(
    const char* path,
    const vector<char>& buf)
{
    // Open file:

    FILE* os = fopen(path, "wb");

    if (!os)
    {
        return false;
    }

    // Write file:

    size_t n = fwrite(&buf[0], 1, buf.size(), os);

    if (n != buf.size())
    {
        return false;
    }

    return true;
}

static bool StripCtrlM(const char* path)
{
    // Open the source file:

    vector<char> buf;

    if (!Inhale(path, buf))
    {
        fprintf(stderr, "%s: failed to read file: %s\n", arg0, path);
        exit(1);
    }

    // Strip the leading comment containing the given pattern:

    vector<char> out;

    for (size_t i = 0; i < buf.size(); i++)
    {
        if (i + 1 != buf.size() && buf[i] == '\r' && buf[i+1] == '\n')
        {
            out.push_back('\n');
            i++;
        }
        else
        {
            out.push_back(buf[i]);
        }
    }

    // Rewrite file:

    if (!WriteFileFromBuffer(path, out))
    {
        fprintf(stderr, "%s: failed to write file: %s\n", arg0, path);
        exit(1);
    }

    return true;
}

int main(int argc, char** argv)
{
    arg0 = argv[0];

    // Check arguments:

    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " FILENAME..." << endl;
        exit(1);
    }

    // Strip licensings:

    for (int i = 1; i < argc; i++)
    {
        printf("%s\n", argv[i]);
        StripCtrlM(argv[i]);
    }

    return 0;
}
