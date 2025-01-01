#include <iostream>
#include <vector>
#include <fstream>
#include <queue>

using namespace std;

static const int NAME_LEN = 32;
static const wchar_t* DELIMITER = L"+= ";
static const wchar_t* INPUT_PATH = L"./교배식.txt";

struct Comb {
    wchar_t parents[2][NAME_LEN];
    wchar_t child[NAME_LEN];
};

struct Node {
    const wchar_t* name;
    vector<const wchar_t*> spouses;
    vector<const wchar_t*> childs;
};

struct Link {
    Node* pNext;
    Link* pPrevLink;
    int linkNum;
};


void InitSetting(int* pCombMaxNum, int* pNodeMaxNum, int* pLinkBufferMaxNum) {
    // init setting
    *pCombMaxNum = 4096;
    *pNodeMaxNum = 200;
    *pLinkBufferMaxNum = 2048;

    // overlay setting
    ifstream settingFile("./setting.txt");
    if (settingFile.is_open() == true) {
        const int bufferLen = 1024;
        char buffer[bufferLen];
        while (settingFile.eof() == false) {
            settingFile.getline(buffer, bufferLen);

            char* remain;
            char* tok = strtok_s(buffer, ": ", &remain);
            if (tok == nullptr || remain == nullptr) {
                continue;
            }

            if (strcmp(tok, "COMB_MAX_NUM") == 0) {
                *pCombMaxNum = atoi(remain);
            }
            else if (strcmp(tok, "NODE_MAX_NUM") == 0) {
                *pNodeMaxNum = atoi(remain);
            }
            else if (strcmp(tok, "LINK_BUFFER_MAX_NUM") == 0) {
                *pLinkBufferMaxNum = atoi(remain);
            }
        }

        settingFile.close();
    }

}

bool ReadFile(int* pCombNum, Comb* pCombs, int combCapacity, const wchar_t* path, const wchar_t** pErrMsg) {
    if (pCombs == nullptr || pCombNum == nullptr) {
        return false;
    }

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
    while (file.eof() == false) {        
        file.getline(buffer, bufferSize);
        lineIdx++;
        
        wchar_t* parents[2]{ nullptr, };
        wchar_t* child = nullptr;        
        wchar_t* context = nullptr;
        parents[0] = wcstok_s(buffer, DELIMITER, &context);
        if (parents[0] == nullptr || context == nullptr) {
            wcout << lineIdx << "줄에 틀린 형식으로 작성되어있습니다. 이 줄은 무시됩니다." << endl
                << "내용 : " << buffer << endl;
            continue;
        }
        parents[1] = wcstok_s(context, DELIMITER, &child);
        if (parents[1] == nullptr || child == nullptr) {
            wcout << lineIdx << "줄에 틀린 형식으로 작성되어있습니다. 이 줄은 무시됩니다." << endl
                << "내용 : " << buffer << endl;
            continue;
        }

        if (combIdx >= combCapacity) {
            *pErrMsg = L"조합 수 최대치를 넘었습니다. 프로그램을 수정해주세요.";
            file.close();
            return false;
        }

        Comb comb;
        wcscpy_s(comb.parents[0], parents[0]);
        wcscpy_s(comb.parents[1], parents[1]);
        wcscpy_s(comb.child, child);
        
        pCombs[combIdx++] = comb;        
    }    

    *pCombNum = combIdx;

    file.close();

    return true;
}

Node* FindNode(Node* nodes, int nodeNum, const wchar_t* name) {
    Node* pFinded = nullptr;
    for (int nodeIdx = 0; nodeIdx < nodeNum; nodeIdx++) {
        if (wcscmp(nodes[nodeIdx].name, name) == 0) {
            pFinded = &nodes[nodeIdx];
            break;
        }
    }

    return pFinded;
}

bool ConstructNode(int* pNodeNum, Node* nodes, int nodeCapacity, const Comb* combs, int combNum, const wchar_t** pErrMsg) {
    int nodeNum = 0;
    for (int combIdx = 0; combIdx < combNum; combIdx++) {
        const Comb& comb = combs[combIdx];
        
        for (int parentIdx = 0; parentIdx < 2; parentIdx++) {
            Node* pFinded = FindNode(nodes, nodeNum, comb.parents[parentIdx]);

            if (pFinded == nullptr) {                
                if (nodeNum >= nodeCapacity) {
                    *pErrMsg = L"노드 수 최대치를 넘었습니다. 프로그램을 수정해주세요.";
                    return false;
                }

                nodes[nodeNum].name = comb.parents[parentIdx];          
                nodeNum++;

                pFinded = &nodes[nodeNum - 1];
            }

            pFinded->spouses.push_back(comb.parents[parentIdx == 0 ? 1 : 0]);
            pFinded->childs.push_back(comb.child);
        }        
    }

    *pNodeNum = nodeNum;
    return true;
}

bool SearchShortestPath(Link** ppFindedLink, const wchar_t* searchParent, const wchar_t* searchChild, Node* nodes, int nodeNum, Link* linkBuffer, int linkCapacity, int maxLinkNum, const wchar_t** pErrMsg) {
    int linkNum = 0;
    queue<Link*> q;
    for (int i = 0; i < nodeNum; i++) {
        if (wcscmp(searchParent, nodes[i].name) == 0) {
            if (linkNum >= linkCapacity) {
                *pErrMsg = L"경로 수 최대치를 넘었습니다. 프로그램을 수정해주세요.";
                return false;
            }

            linkBuffer[linkNum].pNext = &nodes[i];
            linkBuffer[linkNum].pPrevLink = nullptr;
            linkBuffer[linkNum].linkNum = 1;
            linkNum++;

            q.push(&linkBuffer[linkNum - 1]);
        }
    }

    // find
    Link* pFindedLink = nullptr;
    while (q.empty() == false) {
        Link* pLink = q.front();
        q.pop();

        if (pLink->linkNum >= maxLinkNum) {
            continue;
        }

        Node* pNode = pLink->pNext;
        int childNum = pNode->childs.size();
        for (int childIdx = 0; childIdx < childNum; childIdx++) {
            if (wcscmp(pNode->childs[childIdx], searchChild) == 0) {
                pFindedLink = pLink;
                goto findLink;
            }

            if (linkBuffer[linkNum].linkNum >= maxLinkNum) {
                continue;
            }

            Node* pNext = FindNode(nodes, nodeNum, pNode->childs[childIdx]);
            if (pNext == nullptr) {
                continue;
            }

            if (linkNum >= linkCapacity) {
                *pErrMsg = L"경로 수 최대치를 넘었습니다. 프로그램을 수정해주세요.";
                return false;
            }

            linkBuffer[linkNum].pNext = pNext;
            linkBuffer[linkNum].pPrevLink = pLink;
            linkBuffer[linkNum].linkNum = pLink->linkNum + 1;
            linkNum++;

            q.push(&linkBuffer[linkNum - 1]);
        }        
    }

findLink:
    *ppFindedLink = pFindedLink;
    return true;
}

void PrintShortestPath(const wchar_t* searchParent, const wchar_t* searchChild, Comb* combs, int combCapacity, Node* nodes, int nodeCapacity, Link* links, int linkCapacity, int maxLinkNum) {    
    const wchar_t* errMsg;

    int combNum;
    bool isSuccess = ReadFile(&combNum, combs, combCapacity, INPUT_PATH, &errMsg);
    if (isSuccess == false) {
        wcout << errMsg << endl;
        return;
    }

    int nodeNum;
    isSuccess = ConstructNode(&nodeNum, nodes, nodeCapacity, combs, combNum, &errMsg);
    if (isSuccess == false) {
        wcout << errMsg << endl;
        return;
    }

    // fill first link
    //  node : link, node : prevNode, int : link num   
    Link* pFindedLink;
    isSuccess = SearchShortestPath(&pFindedLink, searchParent, searchChild, nodes, nodeNum, links, linkCapacity, maxLinkNum, &errMsg);
    if (isSuccess == false) {
        wcout << errMsg << endl;
        return;
    }

    // output
    if (pFindedLink == nullptr) {
        wcout << L"교배식이 없습니다." << endl;
    }
    else {
        vector<Link*> pLinks;
        Link* pLink = pFindedLink;
        while (pLink != nullptr) {
            pLinks.push_back(pLink);
            pLink = pLink->pPrevLink;
        }

        int linkIdx = 1;
        const wchar_t* parent = searchParent;
        for (int i = pLinks.size() - 1; i >= 1; i--) {
            pLink = pLinks[i];

            const wchar_t* child = pLinks[i - 1]->pNext->name;
            const wchar_t* parent2 = nullptr;
            for (int childIdx = 0; childIdx < pLink->pNext->childs.size(); childIdx++) {
                if (wcscmp(pLink->pNext->childs[childIdx], child) == 0) {
                    parent2 = pLink->pNext->spouses[childIdx];
                }
            }

            wcout << linkIdx << "."
                << "\"" << parent << "\"" << L" + " << parent2 << L" = " << child << endl;
            parent = child;
            linkIdx++;
        }

        const wchar_t* child = searchChild;
        const wchar_t* parent2 = nullptr;
        for (int childIdx = 0; childIdx < pLinks[0]->pNext->childs.size(); childIdx++) {
            if (wcscmp(pLinks[0]->pNext->childs[childIdx], child) == 0) {
                parent2 = pLinks[0]->pNext->spouses[childIdx];
            }
        }
        wcout << linkIdx << "."
            << "\"" << parent << "\"" << L" + " << parent2 << L" = " << child << endl;

    }
}

int main()
{
    // init setting
    int COMB_MAX_NUM;
    int NODE_MAX_NUM;
    int LINK_BUFFER_MAX_NUM;
    InitSetting(&COMB_MAX_NUM, &NODE_MAX_NUM, &LINK_BUFFER_MAX_NUM);      

    // init & alloc
    std::locale::global(std::locale("ko_KR.UTF-8"));
    _wsetlocale(LC_ALL, L"korean");

    Comb* combs = new Comb[COMB_MAX_NUM];
    Node* nodes = new Node[NODE_MAX_NUM];
    Link* links = new Link[LINK_BUFFER_MAX_NUM];

    // summary
    wcout << L"------------ <설명> ------------"<< endl
        << L"최단 루트의 교배식을 출력합니다. 교배할 부모 팰 하나와 교배 될 자식 팰 하나를 입력해주세요." << endl
        << L"\"교배식.txt\"을 토대로 경로를 계산합니다." << endl
        << L"교배 결과를 한 줄에 하나씩, \"+\" 혹은 \"=\" 혹은 \"<띄어쓰기>\"를 기준으로 부모팰, 부모팰, 자식팰 순으로 기록해주세요." << endl
        << L"--------------------------------" << endl << endl;

    // input
    while (true) {
        int maxLinkNum = 7;

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
        PrintShortestPath(searchParent, searchChild, combs, COMB_MAX_NUM, nodes, NODE_MAX_NUM, links, LINK_BUFFER_MAX_NUM, maxLinkNum);
        wcout << L"----------------------------" << endl << endl;
    }      
    
    // dealloc
    delete[] combs;
    delete[] nodes;
    delete[] links;
}
