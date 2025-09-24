#include <iostream>
#include <cstring>
#include <cstdio>

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

// Omar Tawfig : 20210892
// Ahmed Yousif : 20210893
// Izzaldin Bushra : 20210927

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class Node;

class Childern // Basic node structure for a linked list of Children, where each node contains a pointer to some Node data
{
public:
    Node* data;
    Childern* next;
    Childern()
    {
        data = nullptr;
        next = nullptr;
    }
    Childern(Node* data)
    {
        next = nullptr;
        this->data = data;
    }
    Childern(Node* data, Childern* next)
    {
        this->data = data;
        this->next = next;
    }
};

class Node
{
public:
    int sfxIndex; // The starting index (in the original string) of the suffix ending at this node (Used for leaf nodes only).
    int startIndex; // The starting index (in the original string) of the characters on the edge going to this node.
    Childern* children; // Linked list of child nodes.

    Node()
    {
        children = nullptr;
    }
    Node(int startIndex, int sfxIndex) // Node Class Constructor: Takes two arguments start index and the suffix index.
    {
        children = nullptr;
        this->startIndex = startIndex; // The starting index (in the original string) of the characters on the edge going to this node.
        this->sfxIndex = sfxIndex; // The starting index (in the original string) of the suffix ending at this node.
    }
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


class SuffixTree
{
public:
    Node* root; // The root node of the suffix tree.
    char* ostr;  // the String itself saved as reference.

    SuffixTree(char* str) // Suffix Tree Class Constructor: takes the entered string [saved in character array] as an argument.
    {
        int n;
        ostr = str;
        n = strlen(str);
        root = new Node(-1, -1);
        for(int i = 0; i < n; ++i)
            extendSuffixTree(root, i, i, n);
    }

    void NewChild(Node *node, int startIndex, int sfxIndex) // Creating a new child then adding it to the childern linked list of the node
    {
        Node *newNode;
        Childern* newChild;
        newNode = new Node(startIndex, sfxIndex);
        newChild = new Childern(newNode, node->children);
        node->children = newChild;
    }

    void extendSuffixTree(Node* node, int remaining, int sfxIndex, int ostrLen) // Extending the suffix tree using ukkonen's method.
    {
        bool eqNull;
        eqNull = false;
        Node *child;
        Childern *cChild;
        cChild = node->children;

        while (true)
        {
            if (cChild == nullptr) // Check if the current child is nullptr (no children left)
            {
                eqNull = true;
                break;
            }

            // If the first character of the child matches the incoming character, break the loop
            if (ostr[cChild->data->startIndex] == ostr[remaining]) break;

            cChild = cChild->next; // Move to the next child
        }

        if(eqNull)
        {
            NewChild(node, remaining, sfxIndex); // If there are no children that match the incoming character, create a new child node
            return;
        }
        else
        {
            child = cChild->data; // Else, assign the matched child node
        }


        int edgeLength;
        int edgeStartIndex;                     // Calculating the edge length and start index
        edgeStartIndex = child->startIndex;

        int eLen;

        if (child->sfxIndex != -1)
        {
            eLen = ostrLen - child->startIndex; // if the edge is a leaf, the string length - start index of the child = edge length
        }
        else
        {
            Childern* nextNode;
            nextNode = child->children->next;
            eLen = child->children->data->startIndex;
            while (true)
            {
                if (nextNode == nullptr) break;
                if (eLen > nextNode->data->startIndex)
                {
                    eLen = nextNode->data->startIndex;
                }
                nextNode = nextNode->next;
            }
            eLen -= child->startIndex;                   // Calculate edge length considering internal nodes
        }

        if (eLen > (ostrLen - remaining))
        {
            eLen = ostrLen - remaining;   // Adjusting edge length based on the remaining string length
        }
        edgeLength = eLen;

        int matchLength = 0; // Start to match the remaining string with edge's characters
        while (true)
        {
            if (matchLength == edgeLength) break;
            if (ostr[remaining + matchLength] != ostr[edgeStartIndex + matchLength]) break;
            matchLength++;
        }

        if (matchLength == edgeLength) // If the entire edge matches the remaining string, extend the suffix tree
        {
            extendSuffixTree(child, remaining + matchLength, sfxIndex, ostrLen);
        }
        else // If there's a mismatch, create a new node and modify the tree
        {
            Node *leaf;
            leaf = new Node(child->startIndex + matchLength, child->sfxIndex);
            leaf->children = child->children;
            Childern* chldrn = new Childern(leaf);
            child->children = chldrn;
            child->sfxIndex = -1;
            NewChild(child, remaining + matchLength, sfxIndex);
        }
    }

    void printResult(const Childern* occurs) const
    {
        if (occurs == nullptr) return; // If the input node (suffix) is nullptr, return

        printResult(occurs->next); // Traverse the rest of the linked list
        Node* child;
        child = occurs->data; // Get the node associated with the current Childern node

        if (child->sfxIndex != -1)  // Check if the node represents a leaf node (end of a suffix). if yes, print
        {
            std::cout << child->sfxIndex << " ";
        }
        else // If it's not a leaf node, recursively traverse its children
        {
            printResult(child->children);
        }
    }

    void Search(const char *pattern) const
    {
        int m = strlen(pattern);


        Node *cNode;
        cNode = root; // Start from the root of the suffix tree
        int position;
        position = 0; // Position var to track the pattern position during search

        while (true)
        {
            if (position >= m) break; // If the search reaches the end of the pattern, exit the loop
            Node *child;

            bool eqNull;
            eqNull = false;
            Childern *cChild;
            cChild = cNode->children; // Start from the children of the current node

            while (true)
            {
                if (cChild == nullptr)
                {
                    eqNull = true;
                    break; // If no match is found in the children, exit the loop
                }

                if (ostr[cChild->data->startIndex] == pattern[position]) break; // Match found, break the loop

                cChild = cChild->next; // If non-of the "if" conditions broke, move to the next child
            }

            if(eqNull)
            {
                printf("\n \"%s\" Not Found in \"%s\"\n", pattern, ostr);
                return; // If no match is found, print not found and return
            }
            else
            {
                child = cChild->data; // Assign the matched child node
            }

            int eLen;
            int ostrLen;
            int edgeStart;   // Determine the length of the edge and search pattern for matching
            int edgeLength;
            edgeStart = child->startIndex;

            ostrLen = strlen(ostr);
            if (child->sfxIndex != -1)
            {
                eLen = ostrLen - child->startIndex; // edge length if the edge was a leaf as described previously
            }
            else
            {
                Childern* nextNode;
                nextNode = child->children->next;
                eLen = child->children->data->startIndex;
                while (true)
                {
                    if (nextNode == nullptr) break;
                    if (eLen > nextNode->data->startIndex)
                    {
                        eLen = nextNode->data->startIndex;     // Calculate edge length considering internal nodes
                    }
                    nextNode = nextNode->next;
                }
                eLen -= child->startIndex;
            }

            if (eLen > (m - position)) // Handling out of bound
            {
                eLen = m - position;
            }
            edgeLength = eLen;


            int matchLength = 0;  // Check for a match between the edge and the remaining pattern
            while (true)
            {
                if (matchLength == edgeLength) break;
                if (pattern[position + matchLength] != ostr[edgeStart + matchLength]) break;
                matchLength++;
            }

            if (matchLength != edgeLength)
            {
                printf("\n \"%s\" Not Found in \"%s\"\n", pattern, ostr);
                return; // If the edge doesn't fully match the pattern, print not found and return
            }

            position += edgeLength; // Increment the position with the current edge length
            cNode = child; // Go to the next child
        }


        // If the pattern is found, print its occurrences in the tree

        printf("\n\"%s\" in \"%s\":\n", pattern, ostr);
        if (cNode->sfxIndex != -1)
        {
            printf("%d", cNode->sfxIndex); // If it was a leaf no need to traverse
        }
        else // Else, traverse and print indices of the occurrences
        {
            printResult(cNode->children);
        }
        printf("\n");
    }
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <random>

int main()
{
    char alphabets[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                          'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
                          'v', 'w', 'x', 'y', 'z'};
    char *rString;
    char *userSfx;
    int sfxLen;

//    SuffixTree TA_Object("");
//    TA_Object.Search("");

    for (int it = 0; it < 10; it++) // 10 Test Cases
    {

        // Generate a random integer to determine the randomized string length

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(1, 9999);
        int n = distr(gen);
        int i = 0;

        rString = new char[n];

        while (i < n)
        {
            int temp = rd() % 26;
            rString[i] = alphabets[temp];
            i++;
        }

        rString[n] = '$';
        rString[n + 1] = '\0';

        std::printf("The randomized word is: %s \n", rString);
        for (int ew = 0; ew<3; ew++) // Three search patterns
        {
            std::cout << "Enter the suffix len: ";
            std::cin >> sfxLen;
            while (sfxLen > n)
            {
                std::printf("Suffix len should not be more than %i chars. Enter The suffix len again: ", n);
                std::cin >> sfxLen;
            }
            std::cin.ignore();
            userSfx = new char[sfxLen];
            std::printf("Enter the suffix you want to found it occurrences in the word %s: ", rString);
            std::cin.getline(userSfx, sfxLen + 1);
            SuffixTree obj(rString);
            obj.Search(userSfx);
            std::cout << '\n';
        }

        delete[] rString;
        delete[] userSfx;
    }

    return 0;
}