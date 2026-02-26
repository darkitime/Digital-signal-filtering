#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include "Block.h"

/**
 * @brief Система управления графом обработки сигналов.
 * @details Позволяет регистрировать различные блоки (фильтры, сумматоры),
 * устанавливать связи между ними (кто откуда берет данные) и
 * выполнять комплексные вычисления над сигналом с автоматическим разрешением зависимостей.
 */
class ProcessingSystem {
private:
    /** @brief Хранилище всех блоков системы (ключ - имя блока) */
    std::unordered_map<std::string, std::unique_ptr<Block>> blocks;

    /** @brief Таблица связей: ключ = блок-назначение, значение = список блоков-источников */
    std::unordered_map<std::string, std::vector<std::string>> connections;

public:
    /**
     * @brief Конструктор по умолчанию.
     */
    ProcessingSystem() = default;

    /**
     * @brief Добавляет новый вычислительный блок в систему.
     * @param block Уникальный указатель (unique_ptr) на добавляемый блок.
     * @throw std::logic_error Если указатель пуст или блок с таким именем уже существует.
     */
    void addBlock(std::unique_ptr<Block> block) {
        if (!block) throw std::logic_error("Block pointer is null");
        std::string name = block->getName();
        if (blocks.find(name) != blocks.end())
            throw std::logic_error("Block already exists: " + name);
        blocks.emplace(name, std::move(block));
    }

    /**
     * @brief Устанавливает связи (зависимости) для блока.
     * @details Указывает, что `outputBlock` должен получать данные на вход от `sourceBlocks`.
     * @param outputBlock Имя целевого блока, который принимает данные.
     * @param sourceBlocks Вектор имен блоков-источников.
     * @throw std::logic_error Если указанные блоки не найдены в системе.
     */
    void connect(const std::string& outputBlock, const std::vector<std::string>& sourceBlocks) {
        if (blocks.find(outputBlock) == blocks.end())
            throw std::logic_error("Output block not found: " + outputBlock);
        for (const auto& src : sourceBlocks)
            if (blocks.find(src) == blocks.end())
                throw std::logic_error("Source block not found: " + src);

        connections[outputBlock] = sourceBlocks;
    }

    /**
     * @brief Получает "сырой" указатель на блок по его имени.
     * @param name Имя искомого блока.
     * @return Указатель на объект Block или nullptr, если блок не найден.
     */
    Block* getBlock(const std::string& name) {
        auto it = blocks.find(name);
        return (it != blocks.end()) ? it->second.get() : nullptr;
    }

    /**
     * @brief Рекурсивный расчет выхода конкретного блока с учетом всех его зависимостей.
     * @param name Имя блока, выход которого нужно вычислить.
     * @param input Значение внешнего входного сигнала системы.
     * @return Вычисленное выходное значение указанного блока.
     * @throw std::logic_error Если блок не найден в системе.
     */
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
            // Нет зависимостей — используем внешний входной сигнал
            inputs.push_back(input);
        }

        Block* b = getBlock(name);
        if (!b) throw std::logic_error("Block not found: " + name);
        return b->process(inputs);
    }

    /**
     * @brief Вычисляет выходы абсолютно всех блоков системы для одного входа.
     * @param input Значение внешнего входного сигнала.
     * @return Хеш-таблица (словарь), где ключ — имя блока, а значение — его выход.
     */
    std::unordered_map<std::string, double> computeAll(double input) {
        std::unordered_map<std::string, double> results;
        for (const auto& pair : blocks) {
            const std::string& name = pair.first;
            results[name] = computeBlock(name, input);
        }
        return results;
    }

    /**
     * @brief Сбрасывает внутреннее состояние (буферы, память) всех блоков в системе.
     */
    void resetAll() {
        for (auto& pair : blocks) {
            pair.second->reset();
        }
    }
};
