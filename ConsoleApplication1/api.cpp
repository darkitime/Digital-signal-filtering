#include "api.h"
#include "ProcessingSystem.h" 
#include "FIRFilter.h"
#include "IIRFilter.h"
#include "Summator.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

// Глобальная переменная для хранения ошибки
static std::string g_lastError = "";

const char* getLastError() {
    if (g_lastError.empty()) {
        return nullptr;
    }
    return g_lastError.c_str();
}

void clearError() {
    g_lastError.clear();
}

void* createSystem() {
    clearError();
    try {
        return new ProcessingSystem();
    }
    catch (const std::exception& e) {
        g_lastError = e.what();
        return nullptr;
    }
}

void destroySystem(void* systemPtr) {
    // destroySystem обычно не кидает исключений, но для безопасности можно обернуть
    if (systemPtr) {
        delete static_cast<ProcessingSystem*>(systemPtr);
    }
}

void addFIR(void* systemPtr, const char* name, const double* coeffs, int n) {
    clearError();
    try {
        if (!systemPtr) throw std::runtime_error("System pointer is null");
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        std::vector<double> b(coeffs, coeffs + n);
        sys->addBlock(std::make_unique<FIRFilter>(name, b));
    }
    catch (const std::exception& e) {
        g_lastError = std::string("addFIR error: ") + e.what();
    }
}

void addIIR(void* systemPtr, const char* name,
    const double* b, int nB,
    const double* a, int nA) {
    clearError();
    try {
        if (!systemPtr) throw std::runtime_error("System pointer is null");
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        std::vector<double> vb(b, b + nB);
        std::vector<double> va(a, a + nA);
        sys->addBlock(std::make_unique<IIRFilter>(name, vb, va));
    }
    catch (const std::exception& e) {
        g_lastError = std::string("addIIR error: ") + e.what();
    }
}

void addSummator(void* systemPtr, const char* name, double u, double v) {
    clearError();
    try {
        if (!systemPtr) throw std::runtime_error("System pointer is null");
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        sys->addBlock(std::make_unique<Summator>(name, u, v));
    }
    catch (const std::exception& e) {
        g_lastError = std::string("addSumm error: ") + e.what();
    }
}

void connect(void* systemPtr, const char* outputBlock, const char** sourceBlocks, int nSources) {
    clearError();
    try {
        if (!systemPtr) throw std::runtime_error("System pointer is null");
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        std::vector<std::string> sources;
        for (int i = 0; i < nSources; ++i)
            sources.emplace_back(sourceBlocks[i]);
        sys->connect(outputBlock, sources);
    }
    catch (const std::exception& e) {
        g_lastError = std::string("Connect error: ") + e.what();
    }
}

double computeBlock(void* systemPtr, const char* blockName, double input) {
    clearError();
    try {
        if (!systemPtr) throw std::runtime_error("System pointer is null");
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        return sys->computeBlock(blockName, input);
    }
    catch (const std::exception& e) {
        g_lastError = std::string("Compute error: ") + e.what();
        return 0.0;
    }
}

void resetAll(void* systemPtr) {
    clearError();
    try {
        if (!systemPtr) throw std::runtime_error("System pointer is null");
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        sys->resetAll();
    }
    catch (const std::exception& e) {
        g_lastError = std::string("resetAll error: ") + e.what();
    }
}

void processSignal(void* systemPtr, const char* blockName,
    const double* input, double* output, int length) {
    clearError();
    try {
        if (!systemPtr) throw std::runtime_error("System pointer is null");
        auto* sys = static_cast<ProcessingSystem*>(systemPtr);
        for (int i = 0; i < length; ++i) {
            output[i] = sys->computeBlock(blockName, input[i]);
        }
    }
    catch (const std::exception& e) {
        g_lastError = std::string("Processing error: ") + e.what();
    }
}
