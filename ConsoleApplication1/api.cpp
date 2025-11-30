#include "api.h"
#include "ProcessingSystem.h" 

#include "FIRFilter.h"
#include "IIRFilter.h"
#include "Summator.h"
#include <iostream>

void* createSystem() {
    return new ProcessingSystem();
}

void destroySystem(void* systemPtr) {
    delete static_cast<ProcessingSystem*>(systemPtr);
}

void addFIR(void* systemPtr, const char* name, const double* coeffs, int n) {
    try {
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        std::vector<double> b(coeffs, coeffs + n);
        sys->addBlock(std::make_unique<FIRFilter>(name, b));
    }
    catch (const std::exception& e) {
        std::cerr << "[C++ Error] addFIR failed: " << e.what() << std::endl;
    }
}

void addIIR(void* systemPtr, const char* name,
    const double* b, int nB,
    const double* a, int nA) {
    try {
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        std::vector<double> vb(b, b + nB);
        std::vector<double> va(a, a + nA);
        sys->addBlock(std::make_unique<IIRFilter>(name, vb, va));
    }
    catch (const std::exception& e) {
        std::cerr << "[C++ Error] addIIR failed: " << e.what() << std::endl;
    }
}

void addSummator(void* systemPtr, const char* name, double u, double v) {
    try {
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        sys->addBlock(std::make_unique<Summator>(name, u, v));
    }
    catch (const std::exception& e) {
        std::cerr << "[C++ Error] addSum failed: " << e.what() << std::endl;
    }
}

void connect(void* systemPtr,
    const char* outputBlock,
    const char** sourceBlocks,
    int nSources) {
    try {
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        std::vector<std::string> sources;
        for (int i = 0; i < nSources; ++i)
            sources.emplace_back(sourceBlocks[i]);
        sys->connect(outputBlock, sources);
    }
    catch (const std::exception& e) {
        std::cerr << "[C++ Error] connect failed: " << e.what() << std::endl;
    }
}

double computeBlock(void* systemPtr, const char* blockName, double input) {
    try {
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        return sys->computeBlock(blockName, input);
    }
    catch (const std::exception& e) {
        std::cerr << "[C++ Error] computeBlock failed: " << e.what() << std::endl;
        //возвращаем значение, чтобы не сломать стек вызова
        return 0.0;
    }
}


void resetAll(void* systemPtr) {
    try {
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        sys->resetAll();
    }
    catch (const std::exception& e) {
        std::cerr << "[C++ Error] resetAll failed: " << e.what() << std::endl;
    }
}

void processSignal(void* systemPtr, const char* blockName,
    const double* input, double* output, int length) {
    try {
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);

        // Проходим по всему массиву и вычисляем каждый отсчет
        for (int i = 0; i < length; ++i) {
            output[i] = sys->computeBlock(blockName, input[i]);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[C++ Error] processSignal failed: " << e.what() << std::endl;
    }
}
