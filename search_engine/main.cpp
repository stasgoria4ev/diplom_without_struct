#include <iostream>
#include <fstream>
#include <vector>
#include "nlohmann/json.hpp"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include "ConverterJSON.h"

int main() {
    ConverterJSON converterJSON;
    InvertedIndex invertedIndex;
    try {
        invertedIndex.UpdateDocumentBase(converterJSON.GetTextDocuments());
    }
    catch (const NoFileConfigJsonException& x) {
        std::cerr << "Cought exception: " << x.what() << '\n';
        exit(1);
    }
    catch (const MissingConfigFieldException& x) {
        std::cerr << "Cought exception: " << x.what() << '\n';
        exit(1);
    }

    invertedIndex.max_responses = ConverterJSON::GetResponsesLimit();//проброс max_responses в SearchServer через _index(idx)

    SearchServer searchServer(invertedIndex);

    std::vector<std::vector<RelativeIndex>> searchServerSearch = searchServer.search(converterJSON.GetRequests());
    std::vector<std::vector<std::pair<int, float>>> answersTemp;
    for (auto & i : searchServerSearch) {
        std::vector<std::pair<int, float>> answersTempJ;
        for (auto & j : i) {
            answersTempJ.emplace_back(std::pair(j.doc_id, j.rank));
        }
        answersTemp.push_back(answersTempJ);
    }
    converterJSON.putAnswers(answersTemp);
}