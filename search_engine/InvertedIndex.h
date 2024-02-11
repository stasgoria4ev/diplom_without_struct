#pragma once

#include <string>
#include <thread>
#include <utility>

struct Entry {
    size_t doc_id, count;
// Данный оператор необходим для проведения тестовых сценариев
    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
                count == other.count);
    }
};

//std::mutex flow_access;
void Flow(std::vector <std::string> flow, std::map<std::string, std::vector<Entry>> (*freq_dictionary)) {

    for (auto & i : flow) {
        int id;
        if ( i[i.size() - 2] > '0' && i[i.size() - 2] <= '9') {
            char ch1 = i[i.size() - 2];
            std::string ch2 = &i[i.size() - 1];
            std::string temp = ch1 + ch2;
            id = std::stoi(temp);
            i.pop_back(); i.pop_back();
        } else {
            id = std::stoi(&i.back());
            i.pop_back();
        }

        std::map<std::string, std::vector<Entry>> &freq_dictionaryTemp = *freq_dictionary;

        std::string word;
        i += ' ';

        for (char j : i) {

            Entry entry{};
            entry.doc_id = id;
            entry.count = 1;

            if (j != ' ')
                word += j;
            else {
                for (auto &it: freq_dictionaryTemp)
                    if (it.first == word) {
                        for (int z = 0; z < it.second.size(); z++)
                            if (entry.doc_id == it.second[z].doc_id && entry.count <= it.second[z].count) {
                                entry.count += it.second[z].count;
                                freq_dictionaryTemp[it.first].pop_back();
                            }
                    }
                freq_dictionaryTemp[word].push_back(entry);
                word.clear();
            }
        }
    }
}

class InvertedIndex {
public:
    InvertedIndex() = default;

    /**
     * Данное значение оператора необходимо для проведения тестового сценария,
     * в основной программе значение пробрасывается с main.
     */
    int max_responses = 5;

    std::vector<std::string> streams[3];//ко-во ядер/потоков/нитей ПОКА ТОЛЬКО ОДНА ЦИФРА
    int streamsSize = sizeof(streams) / sizeof(streams[0]);
    int streamsSizeTemp = streamsSize;
    /**
    * Обновить или заполнить базу документов, по которой будем совершать
поиск
    * @param texts_input содержимое документов
    */

    void UpdateDocumentBase(std::vector<std::string> input_docs) {
        docs = std::move(input_docs);

        for (int i = 0; i < docs.size(); i++) {
            std::string result = std::to_string(i);

            streams[streamsSize - streamsSizeTemp].push_back(docs[i] + result);
            streamsSizeTemp--;
            if (streamsSizeTemp == 0)
                streamsSizeTemp = streamsSize;
        }

        std::vector <std::thread> flows;
        for (int i = 0; i < streamsSize; i++)
            flows.emplace_back(Flow, streams[i], &freq_dictionary);
        for (auto & flow : flows)
            flow.join();
    };

    /**
    * Метод определяет количество вхождений слова word в загруженной базе
документов
    * @param word слово, частоту вхождений которого необходимо определить
    * @return возвращает подготовленный список с частотой слов
    */
    std::vector<Entry> GetWordCount(const std::string& word) {
        std::vector<Entry> temp;
        for (auto & it : freq_dictionary)
            if (it.first == word)
                temp = it.second;
        return temp;
    };

private:
    std::vector<std::string> docs;//список содержимого документов или коллекция для хранения текстов
        // документов, ..деляет doc_id дл..
    std::map<std::string, std::vector<Entry>> freq_dictionary;//частотный словарь
};