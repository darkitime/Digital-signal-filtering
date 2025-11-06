#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include "Block.h"

class ProcessingSystem {
private:
    // Все блоки
    std::unordered_map<std::string, std::unique_ptr<Block>> blocks;

    // Связи: key = блок-назначение, value = векторы блоков-источников
    std::unordered_map<std::string, std::vector<std::string>> connections;

public:
    ProcessingSystem() = default;

    // Добавление блока
    void addBlock(std::unique_ptr<Block> block) {
        if (!block) throw std::logic_error("Block pointer is null");
        std::string name = block->getName();
        if (blocks.find(name) != blocks.end())
            throw std::logic_error("Block already exists: " + name);
        blocks.emplace(name, std::move(block));
    }

    // Добавление связей: outputBlock зависит от sourceBlocks
    void connect(const std::string& outputBlock, const std::vector<std::string>& sourceBlocks) {
        if (blocks.find(outputBlock) == blocks.end())
            throw std::logic_error("Output block not found: " + outputBlock);
        for (const auto& src : sourceBlocks)
            if (blocks.find(src) == blocks.end())
                throw std::logic_error("Source block not found: " + src);

        connections[outputBlock] = sourceBlocks;
    }

    // Получение блока по имени
    Block* getBlock(const std::string& name) {
        auto it = blocks.find(name);
        return (it != blocks.end()) ? it->second.get() : nullptr;
    }

    // Рекурсивный расчёт выхода блока с учётом связей
    double computeBlock(const std::string& name, double input) {
        auto it = connections.find(name);
        std::vector<double> inputs;

        if (it != connections.end()) {
            // Блок зависит от других блоков
            for (const auto& src : it->second) {
                inputs.push_back(computeBlock(src, input));
            }
        }
        else {
            // Нет зависимостей — используем входной сигнал
            inputs.push_back(input);
        }

        Block* b = getBlock(name);
        if (!b) throw std::logic_error("Block not found: " + name);
        return b->process(inputs);
    }

    // Вычисление выходов всех блоков для одного входного значения
    std::unordered_map<std::string, double> computeAll(double input) {
        std::unordered_map<std::string, double> results;
        for (const auto& pair : blocks) {
            const std::string& name = pair.first;
            results[name] = computeBlock(name, input);
        }
        return results;
    }

    // Сброс всех блоков
    void resetAll() {
        for (auto& pair : blocks) {
            pair.second->reset();
        }
    }
};
