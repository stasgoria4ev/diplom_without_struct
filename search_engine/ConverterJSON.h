#pragma once

#include <string>
#include <sstream>

class MissingConfigFieldException: public std::exception {
public:
    const char* what() const noexcept override {
        return "config file is empty";
    }
};

class NoFileConfigJsonException: public std::exception {
public:
    const char* what() const noexcept override {
        return "config file is missing";
    }
};

/**
* Класс для работы с JSON-файлами
*/
class ConverterJSON {

public:
    ConverterJSON() = default;

/**
* Метод получения содержимого файлов
* @return Возвращает список с содержимым файлов перечисленных
* в config.json
*/
    std::vector<std::string> GetTextDocuments() {
        //-----------------------config----------------------------
        std::ifstream file("config.json");
        if (!file.is_open())
            throw NoFileConfigJsonException();

        nlohmann::json dict;
        file >> dict;
        file.close();

        if (dict["config"].is_null() || dict["config"].empty())
            throw MissingConfigFieldException();
        //-----------------------name------------------------------
        std::cout << "Starting " << dict["config"]["name"] << ".\n";
        //-----------------------version---------------------------
        std::ifstream file2("..\\CMakeLists.txt");
        if (!file2.is_open())
            std::cerr << "File CMakeLists.txt not open...\n";

        std::vector <std::string> words;
        while (!file2.eof()) {
            std::string phrase;
            file2 >> phrase;
            words.push_back(phrase);
        }
        file2.close();

        std::string projectVersion = " ";
        bool found = false;
        for (int i = 0; i < words.size() && !found; i++)
            if (words[i] == "project(search_engine") {
                found = true;
                projectVersion = words[i + 2];
                projectVersion.pop_back();
            }

        std::string fieldVersion = dict["config"]["version"];
        if (fieldVersion != projectVersion)
            std::cerr << "\"config.json\" has incorrect file version\n";
        //-----------------------files-----------------------------
        bool good = true;
        std::map<std::string, std::vector<std::string>> longWords;
        std::map<std::string, std::vector<std::string>> incorrectWords;
        std::vector<std::string> listWithFileContents;//список с содержимым файлов(по строкам) перечисленных в config.json
        for (const auto & i : dict["files"]) {
            int wordCount = 0;
            std::ifstream fileNameInTheFileField(i);
            bool exitTemp = false;
            if (!fileNameInTheFileField.is_open()) {
                std::cerr << "The file " << i << " does not exist\n";
                exitTemp = true;
            }
            if (exitTemp) continue;

            std::string word;
            std::string wordFile;
            while(fileNameInTheFileField >> word) {
                wordFile += word + ' ';

                if (word.length() > 100) { //максимальная длина каждого слова 100 символов
                    good = false;
                    longWords[i].push_back(word);
                }

                wordCount++;

                bool found2 = false;
                for (int j = 0; j < word.length() && !found2; j++) //cлова состоят из строчных латинских букв
                    if (word[j] < 'a' || word[j] > 'z') {
                        found2 = true;
                        good = false;
                        incorrectWords[i].push_back(word);
                    }
            }
            wordFile.pop_back();
            listWithFileContents.push_back(wordFile);

            fileNameInTheFileField.close();

            int wordCount1000 = 1000;
            if (wordCount > wordCount1000) { //не более 1000 слов
                good = false;
                std::cout << "\n\tReduce the number of words in the " << i << " file by "
                          << wordCount - wordCount1000 << " words.\n";
            }
        }

        if (!longWords.empty()) {
            //for (std::map<std::string, std::vector<std::string>>::iterator it = longWords.begin(); it != longWords.end(); ++it) {
            for (auto &longWord: longWords) {
                std::cout << "\n\tThe file " << longWord.first << " contains a word longer than 100 characters: \n";
                if (!longWord.second.empty())
                    for (int i = 0; i < longWord.second.size(); i++)
                        std::cout << '\t' << i + 1 << ". " << longWord.second[i] << ".\n";
            }
        }
        if (!incorrectWords.empty()) {
            for (auto &incorrectWord: incorrectWords) {
                std::cout << "\n\tThe file " << incorrectWord.first << " contains a word with invalid characters: \n";
                if (!incorrectWord.second.empty())
                    for (int i = 0; i < incorrectWord.second.size(); i++)
                        std::cout << '\t' << i + 1 << ". " << incorrectWord.second[i] << ".\n";
            }
        }
        //---------------------------------------------------------
        if (!good) {
            std::cout << "\n\tCorrect and run the program again.\n";
            exit(1);
        }
        return listWithFileContents;
    };
/**
* Метод считывает поле max_responses для определения предельного
* количества ответов на один запрос
* @return
*/
    static int GetResponsesLimit() {

        std::ifstream file("config.json");
        if (!file.is_open()) {
            std::cerr << "File \"config.json\" not open...\n";
            exit(1);
        }

        nlohmann::json dict;
        file >> dict;
        file.close();

        int max_responses;
        if (dict["config"]["max_responses"].is_null() || dict["config"]["max_responses"].empty())
            max_responses = 5;
        else
            max_responses = dict["config"]["max_responses"];

        return max_responses;
    };
/**
* Метод получения запросов из файла requests.json
* @return возвращает список запросов из файла requests.json
*/
    std::vector<std::string> GetRequests() {
        std::ifstream file("requests.json");
        if (!file.is_open()) {
            std::cerr << "File \"requests.json\" not open...\n";
            exit(1);
        }

        nlohmann::json dict;
        file >> dict;
        file.close();

        int numberOfRequests1000 = 1000;

        bool good = true;
        if (dict["requests"].size() > numberOfRequests1000) { //не более 1000 запросов
            good = false;
            std::cout << "\n\tReduce the number of requests in the \"requests.json\" file by " <<
                      dict["requests"].size() - numberOfRequests1000 << " request.\n";
        }

        std::vector<std::string> requestList; // список запросов из файла requests.json
        if (good) {
            for (int i = 0; i < dict["requests"].size() && good; i++) {
                int wordCountInTheQuery = 0; //счётчик слов в запросе
                std::string str = dict["requests"][i];

                str += ' ';

                std::string strTemp;
                for (int j = 0; j < str.length() && good; j++) {
                    strTemp += str[j];

                    if (str[j] == ' ') {
                        strTemp.pop_back();

                        bool found = false;

                        if (strTemp.empty())
                            found = true;

                        for (int s = 0; s < strTemp.length() && !found; s++)
                            if (strTemp[s] < 'a' || strTemp[s] > 'z') {
                                found = true;
                                good = false;
                                std::cout << "\n\tInvalid character found in words.\n";
                            }

                        if (!found)
                            wordCountInTheQuery++; //счётчик слов в запросе
                        strTemp.erase();
                    }
                }

                requestList.push_back(str);

                if (wordCountInTheQuery > 10) {
                    std::cout << "\n\tReduce the number of words in the " << i + 1 << " query by " <<
                              wordCountInTheQuery - 10 << " word.\n";
                    good = false;
                } else if (wordCountInTheQuery == 0) {
                    std::cout << "\n\tIncrease the number of words in the " << i + 1 << " query by 1 word.\n";
                    good = false;
                }
            }
        } else {
            std::cout << "\n\tCorrect and run the program again.\n";
            exit(1);
        }

        if (!good) {
            std::cout << "\n\tCorrect and run the program again.\n";
            exit(1);
        }

        return requestList;
    };
/**
* Положить в файл answers.json результаты поисковых запросов
*/
    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {

        std::ofstream file("answers.json");
        if (!file.is_open()) {
            std::cout << "The answers.json file is missing, restarting the program will fix the error\n";
            std::ofstream file2("answers.json");
            file2.close();
        } else {
            std::ofstream file1("answers.json");
            file1.close();
        }

        nlohmann::json dict;
        for (int i = 0; i < answers.size(); i++) {
            //std::vector<std::pair<nlohmann::json, nlohmann::json>> relevance;
            std::vector <nlohmann::json> relevance;

            int i1 = i + 1;
            std::string numbZeros;
            if ( i1 < 10)
                numbZeros = "00";
            else if (i1 <= 100)
                numbZeros = "0";
            else
                numbZeros = "";
            std::string request = "request" + numbZeros + std::to_string(i1);

            std::string result;

            result = !answers[i].empty() ? "true" : "false";
            dict["answers"][request]["result"] = result;
            if (answers[i].size() > 1) {
                for (auto & it : answers[i])
                    relevance.push_back({{"docid", it.first},
                                         {"rank",  round(it.second * 1000) / 1000}});
                dict["answers"][request]["relevance"] = relevance;
            } else if (answers[i].size() == 1) {
                dict["answers"][request]["docid"] = answers[i][0].first;
                dict["answers"][request]["rank"] = answers[i][0].second;
            }
        }
        file << dict.dump(2);
        file.close();
    }
};