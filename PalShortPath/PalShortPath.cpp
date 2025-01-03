﻿//#define TEST

#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <map>
#include <list>
#include "Queue.hpp"
#include "PalNameConst.hpp"

using namespace std;

static const int NAME_LEN = 32;
static const wchar_t* DELIMITER = L"+= ";
static const wchar_t* INPUT_PATH = L"./교배식.txt";
static const wchar_t* DUPLICATE_NAME = L"DUPLICATED";
static const int MAX_PAL_NUM = 200;

struct CmpStr
{
    bool operator()(const wchar_t* const a, const wchar_t* const b) const
    {
        return std::wcscmp(a, b) < 0;
    }
};

struct Comb {
    const wchar_t* parents[2];
    const wchar_t* child;    
};

struct Node {
    struct Breed {
        const wchar_t* spouse;
        const wchar_t* child;
    };

    const wchar_t* name;    
    Breed breeds[MAX_PAL_NUM];
    int breedNum;
};

struct Link {
    int nodeIdx;
    int prevLinkIdx;
    int prevChildIdx;
    int linkNum;
};

struct CmpLink {
    bool operator()(const pair<int, int>& link1, const pair<int, int>& link2) const
    {
        return link1.second > link2.second;
    }
};

void CopyWithTrim(wchar_t* dst, int dstSize, const wchar_t* src) {    
    int startSpaceNum = 0;
    int endSpaceNum = 0;

    int srcLen = wcslen(src);
    for (int i = 0; i < srcLen; i++) {
        if (src[i] != L' ') {
            break;
        }

        startSpaceNum++;
    }

    for (int i = srcLen - 1; i >= 0; i--) {
        if (src[i] != L' ') {
            break;
        }

        endSpaceNum++;
    }
    
    wcsncpy_s(dst, dstSize, src + startSpaceNum, srcLen - endSpaceNum);
}

void Trim(wchar_t* src) {
    int startSpaceNum = 0;
    int endSpaceNum = 0;

    int srcLen = wcslen(src);
    for (int i = 0; i < srcLen; i++) {
        if (src[i] != L' ') {
            break;
        }

        startSpaceNum++;
    }

    for (int i = srcLen - 1; i >= 0; i--) {
        if (src[i] != L' ') {
            break;
        }

        endSpaceNum++;
    }

    src[srcLen - endSpaceNum] = L'\0';
    src = src + startSpaceNum;
}

wchar_t* TokenizeWCS(wchar_t* str, const wchar_t* delimiter, wchar_t** context) {
    int delimiterLen = wcslen(delimiter);
    
    int delimStartIdx = -1;
    int delimEndIdx = -1;

    for (int i = 0; str[i] != L'\0'; i++) {
        for (int delimIdx = 0; delimIdx < delimiterLen; delimIdx++) {
            if (str[i] == delimiter[delimIdx]) {
                delimStartIdx = i;
                break;
            }            
        }

        if (delimStartIdx != -1) {
            break;
        }
    }

    if (delimStartIdx == -1) {
        *context = nullptr;
        return str;
    }

    if (delimStartIdx != -1) {
        for (int i = delimStartIdx; str[i] != L'\0'; i++) {
            bool isDelim = false;
            for (int delimIdx = 0; delimIdx < delimiterLen; delimIdx++) {
                if (str[i] == delimiter[delimIdx]) {
                    isDelim = true;
                    break;
                }
            }

            if (isDelim == false) {
                delimEndIdx = i - 1;
                break;
            }
        }
    }
    
    str[delimStartIdx] = L'\0';
    *context = str + delimEndIdx + 1;

    return str;    
}

bool IsPalName(const wchar_t* searchName, map<const wchar_t*, int, CmpStr>& palOrderByName) {
    return palOrderByName.find(searchName) != palOrderByName.end();    
}

bool ReadFile(vector<Comb>& combs, map<const wchar_t*, int, CmpStr>& palNameIdxByName, const wchar_t* path, const wchar_t** pErrMsg) {
    wifstream file;
    file.open(path);
    if (file.is_open() == false) {
        *pErrMsg = L"입력 파일을 찾지 못했습니다.";        
        return false;
    }
    
    const int bufferSize = 1024;
    wchar_t* buffer = new wchar_t[bufferSize];
    // get combs
    int combIdx = 0;
    int lineIdx = 0;
    bool isAllNameValid = true;
    while (file.eof() == false) {        
        file.getline(buffer, bufferSize);
        lineIdx++;
        
        // tokenize input line
        wchar_t* parents[2]{ nullptr, };
        wchar_t* child = nullptr;        
        wchar_t* context = nullptr;
        parents[0] = TokenizeWCS(buffer, DELIMITER, &context);
        if (parents[0] == nullptr || context == nullptr) {
            wcout << lineIdx << "줄에 틀린 형식으로 작성되어있습니다. 이 줄은 무시됩니다." << endl
                << "내용 : " << buffer << endl;
            continue;
        }
        parents[1] = TokenizeWCS(context, DELIMITER, &child);
        if (parents[1] == nullptr || child == nullptr) {
            wcout << lineIdx << "줄에 틀린 형식으로 작성되어있습니다. 이 줄은 무시됩니다." << endl
                << "내용 : " << buffer << endl;
            continue;
        }

        // check valid name
        Trim(parents[0]);
        Trim(parents[1]);
        Trim(child);

        const wchar_t* names[3]{ parents[0], parents[1], child };
        bool isValidNames[3];
        bool isNameValidInComb = true;
        for (int i = 0; i < 3; i++) {
            isValidNames[i] = palNameIdxByName.find(names[i]) != palNameIdxByName.end();
            if (isValidNames[i] == false) {
                isNameValidInComb = false;
            }
        }

        if (isNameValidInComb == false) {
            wcout << lineIdx << L"줄에 작성된 팰 이름이 올바르지 않습니다." << endl;
            wcout << L"\t내용 : ";

            for (int i = 0; i < 3; i++) {
                if (isValidNames[i]) {
                    wcout << names[i];
                }
                else {
                    wcout << L"\"" << names[i] << L"\"";
                }
                wcout << L" ";
            }
            wcout << endl;

            isAllNameValid = false;
            continue;
        }
        

        Comb comb;        
        comb.parents[0] = PAL_NAMES[palNameIdxByName[parents[0]]];
        comb.parents[1] = PAL_NAMES[palNameIdxByName[parents[1]]];
        comb.child = PAL_NAMES[palNameIdxByName[child]];
        combs.push_back(comb);
          
    }    

    if (isAllNameValid == false) {
        *pErrMsg = L"[이름 오류]";
        return false;
    }

    // check if comb is valid
    vector<int> duplicateCombIndices;
    vector<pair<int, int>> notValidCombPair;
    for (int i = 0; i < combs.size(); i++) {
        Comb& comb1 = combs[i];
        for (int j = i + 1; j < combs.size(); j++) {
            Comb& comb2 = combs[j];

            bool isSameComb =
                (wcscmp(comb1.parents[0], comb2.parents[0]) == 0 && wcscmp(comb1.parents[1], comb2.parents[1]) == 0)
                || (wcscmp(comb1.parents[0], comb2.parents[1]) == 0 && wcscmp(comb1.parents[1], comb2.parents[0]) == 0);

            if (isSameComb == false)
                continue;

            if (wcscmp(comb1.child, comb2.child) != 0) {
                notValidCombPair.push_back({ i, j });
            }
            else {
                duplicateCombIndices.push_back(j);
            }
        }
    }

    if (notValidCombPair.size() > 0) {        
        wcout << L"겹치는 교배식이 존재합니다. 교배식을 수정해주세요." << endl;
        for (int i = 0; i < notValidCombPair.size(); i++) {
            Comb& comb1 = combs[notValidCombPair[i].first];
            Comb& comb2 = combs[notValidCombPair[i].second];

            wcout << (i + 1) << endl;
            wcout << comb1.parents[0] << "+" << comb1.parents[1] << "=" << comb1.child << endl;
            wcout << comb2.parents[0] << "+" << comb2.parents[1] << "=" << comb2.child << endl;
        }

        *pErrMsg = L"[교배식 오류]";

        return false;
    }



    for (int i = 0; i < duplicateCombIndices.size(); i++) {
        combs[duplicateCombIndices[i]].parents[0] = DUPLICATE_NAME;
        combs[duplicateCombIndices[i]].parents[1] = DUPLICATE_NAME;
        combs[duplicateCombIndices[i]].child = DUPLICATE_NAME;
    }

    file.close();

    return true;
}

int FindNodeIdx(map<const wchar_t*, int, CmpStr>& palNameIdxByName, const wchar_t* name) {
    return palNameIdxByName.find(name) == palNameIdxByName.end() ? -1 : palNameIdxByName[name];
}

bool ConstructNode(Node* nodes, map<const wchar_t*, int, CmpStr>& palNameIdxByName, vector<Comb>& combs, const wchar_t** pErrMsg) {
    for (int i = 0; i < PAL_LEN; i++) {
        nodes[i].name = PAL_NAMES[i];
        nodes[i].breedNum = 0;
    }

    for (int combIdx = 0; combIdx < combs.size(); combIdx++) {
        const Comb& comb = combs[combIdx];
        
        bool isSameParent = wcscmp(comb.parents[0], comb.parents[1]) == 0;
        int parentNum = isSameParent ? 1 : 2;

        for (int parentIdx = 0; parentIdx < parentNum; parentIdx++) {
            int findNodeIdx = FindNodeIdx(palNameIdxByName, comb.parents[parentIdx]);
            if (findNodeIdx == -1) {
                continue;
            }

            Node& findedNode = nodes[findNodeIdx];            
            findedNode.breeds[findedNode.breedNum].spouse = comb.parents[parentIdx == 0 ? 1 : 0];
            findedNode.breeds[findedNode.breedNum].child = comb.child;
            findedNode.breedNum++;
        }        
    }
    
    return true;
}

bool SearchShortestPaths(vector<int>& pFindedLinkIndices,
    const wchar_t* searchParent,
    const wchar_t* searchChild,
    map<const wchar_t*, int, CmpStr> palIdxByName,
    Node* nodes,    
    int* reachLinkNums,
    vector<Link>& linkBuffer,
    const wchar_t** pErrMsg) 
{        
    // link index, link num
    priority_queue<pair<int, int>, vector<pair<int, int>>, CmpLink> pq;    
    Link newLink;
    newLink.nodeIdx = FindNodeIdx(palIdxByName, searchParent);
    newLink.prevLinkIdx = -1;
    newLink.linkNum = 0;
    newLink.prevChildIdx = -1;

    linkBuffer.push_back(newLink);
    pq.push({linkBuffer.size() - 1, 0});    

    // find
    bool isTargetFind = false;
    int findedLinkNum = -1;

    while (pq.empty() == false) {
        pair<int, int> linkIdxAndLinkNum = pq.top();        
        pq.pop();

        int linkIdx = linkIdxAndLinkNum.first;
        Link link = linkBuffer[linkIdx];

        // exceed link num
        if (isTargetFind && link.linkNum > findedLinkNum) {
            continue;
        }              

        // found path       
        // link of which link Num is 0 is just for start queue, and not a breeding info. so skip
        if (link.linkNum != 0) {  
            if (reachLinkNums[link.nodeIdx] != -1 && reachLinkNums[link.nodeIdx] < link.linkNum) {
                continue;
            }
            
            reachLinkNums[link.nodeIdx] = link.linkNum;
            if (wcscmp(nodes[link.nodeIdx].name, searchChild) == 0) {                                            
                isTargetFind = true;
                findedLinkNum = link.linkNum;
                pFindedLinkIndices.push_back(linkIdx);
                continue;
            }
        }      

        Node& node = nodes[link.nodeIdx];
        int childNum = node.breedNum;
        for (int childIdx = 0; childIdx < childNum; childIdx++) {
            // if node doens't have children
            int nextNodeIdx = FindNodeIdx(palIdxByName, node.breeds[childIdx].child);
            if (nextNodeIdx == -1) {
                continue;
            }

            // new path
            Link newLink;
            newLink.nodeIdx = nextNodeIdx;
            newLink.prevLinkIdx = linkIdx;
            newLink.linkNum = link.linkNum + 1;
            newLink.prevChildIdx = childIdx;  
            linkBuffer.push_back(newLink);            
            pq.push({ linkBuffer.size() - 1, link.linkNum + 1 });
        }        
    }


    return true;
}


void PrintPaths(int findedLinkIdx,
    Node* nodes,
    vector<Link>& linkBuffer,
    const wchar_t* searchParent,
    const wchar_t* searchChild)
{
    vector<Link> links;

    // collect links by path order
    {
        int linkIdx = findedLinkIdx;
        while (true) {
            if (linkIdx == -1) {
                break;
            }

            links.push_back(linkBuffer[linkIdx]);
            linkIdx = linkBuffer[linkIdx].prevLinkIdx;
        }
    }

    int linkIdx = 1;
    const wchar_t* parent = searchParent;
    // print links by tracing inversely
    // except first link of which prev link is nullptr
    for (int i = links.size() - 2; i >= 0; i--) {
        Link& link = links[i];
        Link& prevLink = linkBuffer[link.prevLinkIdx];
        Node& node = nodes[link.nodeIdx];
        Node& prevNode = nodes[prevLink.nodeIdx];

        const wchar_t* child = node.name;
        const wchar_t* parent1 = prevNode.name;
        const wchar_t* parent2 = prevNode.breeds[link.prevChildIdx].spouse;

        wcout << linkIdx << "."
            << "\"" << parent1 << "\"" << L" + " << parent2 << L" = " << child << endl;
        parent = child;
        linkIdx++;
    }
}



void PrintShortestPath(const wchar_t* searchParent,
    const wchar_t* searchChild,
    map<const wchar_t*, int, CmpStr>& palIdxByName,
    vector<Comb>& combBuffer,
    Node* nodes,
    vector<Link>& linkBuffer,
    int* reachLinkNums)
{    
    const wchar_t* errMsg;

    // clear
    combBuffer.clear();    
    linkBuffer.clear();
    memset(reachLinkNums, -1, sizeof(int) * PAL_LEN);
    
    // read file
    bool isSuccess = ReadFile(combBuffer, palIdxByName, INPUT_PATH, &errMsg);
    if (isSuccess == false) {
        wcout << errMsg << endl;
        return;
    }

    // check if search is valid
    bool isValidParent = palIdxByName.find(searchParent) != palIdxByName.end();
    bool isValidChild = palIdxByName.find(searchChild) != palIdxByName.end();
    
    if (isValidParent == false || isValidChild == false) {
        wcout << L"교배식에 존재하지 않는 종류입니다." << endl;
        wcout << L"없는 종류: ";
        if (isValidParent == false) {
            wcout << searchParent << " ";
        }
        if (isValidChild == false) {
            wcout << searchChild << " ";
        }
        wcout << endl;

        return;
    }
      
    // construct node
    int nodeNum;
    isSuccess = ConstructNode(nodes, palIdxByName, combBuffer, &errMsg);
    if (isSuccess == false) {
        wcout << errMsg << endl;
        return;
    }

    // fill first link        
    vector<int> pFindedLinkIndices;
    isSuccess = SearchShortestPaths(pFindedLinkIndices, searchParent, searchChild, palIdxByName, nodes, reachLinkNums, linkBuffer, &errMsg);
    if (isSuccess == false) {
        wcout << errMsg << endl;
        return;
    }   

    // output
    if (pFindedLinkIndices.size() == 0) {
        wcout << L"교배식이 없습니다." << endl;
    }
    else {
        for (int i = 0; i < pFindedLinkIndices.size(); i++) {
            wcout << L"교배식 " << (i + 1) << endl;
            PrintPaths(pFindedLinkIndices[i], nodes, linkBuffer, searchParent, searchChild);

            if(i != pFindedLinkIndices.size() - 1)
                wcout << endl;
        }
    }
}

//void Test(map<const wchar_t*, int, CmpStr>& palNameIdxByName, vector<Comb>& combs, Node* nodes) {
//    combs.clear();
//    nodes.clear();
//
//    const wchar_t* errMsg;
//    bool isSuccess = ReadFile(combs, palNameIdxByName, INPUT_PATH, &errMsg);
//    assert(isSuccess);
//
//    isSuccess = ConstructNode(nodes, combs, &errMsg);
//    assert(isSuccess);
//
//    const wchar_t* known1 = L"도로롱";
//    const wchar_t* known2 = L"유령초";
//    Node* pKnown1 = nullptr;
//    for (int i = 0; i < nodes.size(); i++) {
//        if (wcscmp(nodes[i].name, known1) == 0) {
//            pKnown1 = &nodes[i];
//            break;
//        }
//    }
//    assert(pKnown1 != nullptr);
//
//    const wchar_t* child = nullptr;
//    for (int i = 0; i < pKnown1->breedNum; i++) {
//        if (wcscmp(pKnown1->breeds[i].spouse, known2) == 0) {
//            child = pKnown1->breeds[i].child;
//            break;
//        }
//    }
//    assert(child != nullptr);
//
//    const wchar_t* spouse = nullptr;
//    for (int i = 0; i < pKnown1->breedNum; i++) {
//        if (wcscmp(pKnown1->breeds[i].child, known2) == 0) {
//            spouse = pKnown1->breeds[i].spouse;
//            break;
//        }
//    }
//    assert(spouse != nullptr);
//
//    const wchar_t* knowns[4] = {
//        known1, known2, child, spouse
//    };
//
// 
//}

int main()
{
    // init setting    
    // init & alloc
    std::locale::global(std::locale("ko_KR.UTF-8"));
    _wsetlocale(LC_ALL, L"korean");
    
    // init
    vector<Comb> combBuffer;
    Node* nodes = new Node[PAL_LEN];
    vector<Link> linkBuffer;
    int* reachLinkNums = new int[PAL_LEN];

    map<const wchar_t*, int, CmpStr> palNameIdxByName;
    for (int i = 0; i < PAL_LEN; i++) {        
        palNameIdxByName.insert({ PAL_NAMES[i], i });
    }

#ifdef TEST
    Test(palIdxByName, combBuffer, nodeBuffer);
#endif

    // summary
    wcout << L"------------ <설명> ------------"<< endl
        << L"최단 루트의 교배식을 출력합니다. 교배할 부모 팰 하나와 교배 될 자식 팰 하나를 입력해주세요." << endl
        << L"\"교배식.txt\"을 토대로 경로를 계산합니다." << endl
        << L"교배 결과를 한 줄에 하나씩, \"+\" 혹은 \"=\" 혹은 \"<띄어쓰기>\"를 기준으로 부모팰, 부모팰, 자식팰 순으로 기록해주세요." << endl
        << L"--------------------------------" << endl << endl;

    // input
    while (true) {

        wchar_t searchParent[NAME_LEN];
        wchar_t searchChild[NAME_LEN];
        wcout << L"검색하려는 부모 팰을 입력해주세요(종료하시려면 q를 입력해주세요.) : ";
        wcin >> searchParent;
        if (wcscmp(searchParent, L"q") == 0) {
            break;
        }

        wcout << L"검색하려는 자식 팰을 입력해주세요(종료하시려면 q를 입력해주세요.) : ";
        if (wcscmp(searchParent, L"q") == 0) {
            break;
        }
        wcin >> searchChild;

        wcout << L"------------결과------------" << endl;

        
        // output
        PrintShortestPath(searchParent, searchChild, palNameIdxByName, combBuffer, nodes, linkBuffer, reachLinkNums);
        wcout << L"----------------------------" << endl << endl;
    }     


    delete[] nodes;
   
}

