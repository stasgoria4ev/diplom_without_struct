#pragma once

#include <string>

struct RelativeIndex{
    size_t doc_id;
    float rank;
    bool operator ==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && rank == other.rank);
    }
};

class SearchServer {
public:
/**
* @param idx � ����������� ������ ��������� ������ �� �����
InvertedIndex,
* ����� SearchServer ��� ������ ������� ���� ����������� �
�������
*/
    SearchServer(InvertedIndex& idx) : _index(idx){ };
/**
* ����� ��������� ��������� ��������
* @param queries_input ��������� ������� ������ �� �����
requests.json
* @return ���������� ��������������� ������ ����������� ������� ���
�������� ��������
*/

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input) {
        std::vector<std::vector<RelativeIndex>> rIndexes;
        for (const auto & i1 : queries_input) { //���������� �����, ��������
            std::vector<RelativeIndex> rIndex;
            std::string word;
            std::vector<std::string> stringRequest;
            for (int j = 0; j <= i1.size(); j++) {
                if (i1[j] != ' ' && i1[j] != '\0')
                    word += i1[j];
                else {
                    if (!word.empty())
                        stringRequest.push_back(word);//��������� ��������� ������ �� ��������� �����
                    word.clear();
                }
            }

            for (int i = 0; i < stringRequest.size(); i++) { // ��������� �� ��� ������ ����������.
                for (int j = i + 1; j <= stringRequest.size() - 1; j++)
                    if (stringRequest[i] == stringRequest[j]) {
                        stringRequest.erase(stringRequest.begin() + j);
                        j--;
                    }
            }

            auto* calculationTable = new std::map<size_t , std::vector<size_t>>;//������� �������
            for (const auto & z : stringRequest) {
                std::vector<Entry> temp5 = this->_index.GetWordCount(z);
                if (!temp5.empty())
                    for (auto & j : temp5)
                        (*calculationTable)[j.doc_id].push_back(j.count);
                }

            for (auto & it : *calculationTable) {//����� ���� count`�� �� �������
                size_t count = 1;
                if (it.second.size() > 1) {
                    count--;
                    for (size_t i : it.second)
                        count += i;
                }
                RelativeIndex relativeIndex{};
                relativeIndex.doc_id = it.first;
                relativeIndex.rank = float(count);
                rIndex.push_back(relativeIndex);
            }

            delete calculationTable;

            for (int i = 0; i < rIndex.size(); i++) //����������
                for (unsigned j = rIndex.size() - 1; j >= 1; j--)
                    if (rIndex[j].rank > rIndex[j - 1].rank) {
                        RelativeIndex relativeIndexTemp = rIndex[j];
                        rIndex[j] = rIndex[j - 1];
                        rIndex[j - 1] = relativeIndexTemp;
                    }

            // ��������� ������ ������� �������� ������������
            unsigned max_responses = rIndex.size() - this->_index.max_responses;
            if (rIndex.size() > this->_index.max_responses)
                for (int i = 0; i < max_responses; i++)
                    rIndex.pop_back();

            float max = 0;
            for (auto & ii : rIndex) { // ������� ������������ ���������� ������������� ����� ���� ���������� ��� ������ ��������� ������
                if (ii.rank > max)
                    max = ii.rank;
            }

            for (auto & ii : rIndex) {// ������������� �������������
                ii.rank /= max;
            }

            rIndexes.push_back(rIndex);
        }

        return rIndexes;
    };
private:
    InvertedIndex _index;
};