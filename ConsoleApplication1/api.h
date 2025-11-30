#pragma once

extern "C" {
    __declspec(dllexport) void* createSystem();
    __declspec(dllexport) void destroySystem(void* systemPtr);

    __declspec(dllexport) void addFIR(void* systemPtr, const char* name, const double* coeffs, int n);
    __declspec(dllexport) void addIIR(void* systemPtr, const char* name, const double* b, int nB, const double* a, int nA);
    __declspec(dllexport) void addSummator(void* systemPtr, const char* name, double u, double v);

    __declspec(dllexport) void connect(void* systemPtr, const char* outputBlock, const char** sourceBlocks, int nSources);

    __declspec(dllexport) double computeBlock(void* systemPtr, const char* blockName, double input);
    __declspec(dllexport) void resetAll(void* systemPtr);
    __declspec(dllexport) void processSignal(void* systemPtr, const char* blockName, const double* input, double* output, int length);
}

