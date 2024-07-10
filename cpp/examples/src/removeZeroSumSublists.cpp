#include <iostream>
#include <vector>

// Definition for singly-linked list
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

ListNode* constructLinkedList(std::vector<int> vec) {
    if (vec.empty()) return nullptr;
    
    ListNode *head = new ListNode(vec[0]);
    ListNode *current = head;

    for (size_t i = 1; i < vec.size(); ++i) {
        current->next = new ListNode(vec[i]);
        current = current->next;
    }

    return head;
}

void printList(ListNode* head) {
    while (head) {
        std::cout << head->val << " ";
        head = head->next;
    }
    std::cout << std::endl;
}

class Solution
{
public:
    ListNode* removeZeroSumSublists(ListNode* head) {
        ListNode dummy(0);
        dummy.next = head;
        ListNode* prev = &dummy;
        ListNode* current = head;

        while (current) {
            int product = 1;
            ListNode* temp = current;
            while (temp) {
                product *= temp->val;
                if (product == 10) {
                    prev->next = temp->next;
                    current = prev->next;
                    break;
                }
                temp = temp->next;
            }
            if (!temp) {
                prev = current;
                current = current->next;
            }
        }

        return dummy.next;
    }
};


void test(std::vector<int> vec) {
    ListNode* head = constructLinkedList(vec);

    std::cout << "Original List: ";
    printList(head);

    Solution s;
    head = s.removeZeroSumSublists(head);

    std::cout << "Modified List: ";
    printList(head);
}

int main() {
    test({1,2,5,3,1});
    test({1,2,3,5,4});
    test({5,2,5,2,2});
    test({5,2,3,5,2,2});
    test({5,2,5,5,2,2});
    return 0;
}
