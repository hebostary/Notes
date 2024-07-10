#include <map>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
public:
    int minOpsForIncreasingLayers(TreeNode* root) {
        if (!root) return 0;
        
        std::queue<TreeNode*> q;
        q.push(root);
        int minOps = 0;
        std::unordered_map<int, std::vector<int>> layerMap; 
        
        while (!q.empty()) {
            int layerSize = q.size();
            std::vector<int> layerValues;
            
            for (int i = 0; i < layerSize; ++i) {
                TreeNode* node = q.front();
                q.pop();
                layerValues.push_back(node->val);
                if (node->left) q.push(node->left);
                if (node->right) q.push(node->right);
            }

            int cnt = getMinOps(layerValues);
            minOps += cnt;
        }
        
        return minOps;
    }

    int getMinOps(std::vector<int> &nums){
        std::map<int, int> mp;    
        std::vector<int> sortedNums(nums);
        sort(sortedNums.begin(), sortedNums.end());
        for (size_t i = 0; i < sortedNums.size(); i++)
            mp[sortedNums[i]] = i ;
    
        int cnt = 0;
        for (size_t i = 0; i < nums.size();i++) {
            if (nums[i] == sortedNums[i])continue;
            std::swap(nums[i], nums[mp[nums[i]]]);
            cnt++;
        }
        return cnt;
    }
};

int main() {
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(3);
    root->right = new TreeNode(2);
    root->left->left = new TreeNode(7);
    root->left->right = new TreeNode(6);
    root->right->left = new TreeNode(5);
    root->right->right = new TreeNode(4);
    
    Solution s;
    int minOps = s.minOpsForIncreasingLayers(root);
    std::cout << "Minimum operations required: " << minOps << std::endl;

    root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    root->right->left = new TreeNode(6);
    root->right->right = new TreeNode(7);
    minOps = s.minOpsForIncreasingLayers(root);
    std::cout << "Minimum operations required: " << minOps << std::endl;

    std::vector<int> vec = {6,4,5,1,2,3};
    minOps = s.getMinOps(vec);
    std::cout << "Minimum operations required: " << minOps << std::endl;    

    return 0;
}
