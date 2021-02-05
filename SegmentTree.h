#ifndef SEGMENTTREE_SEGMENTTREE_H
#define SEGMENTTREE_SEGMENTTREE_H

#include <algorithm>
#include <memory>
#include <functional>
#include <utility>

namespace algo {
  template<typename T, T(*Union)(T, T), T default_value = T()>
  class SegmentTree {
    class TreeNode {
      T data = default_value;
      std::shared_ptr<TreeNode> left_son = nullptr;
      std::shared_ptr<TreeNode> right_son = nullptr;
      TreeNode* ancestor;

      void Update() {
        if (!left_son && !right_son) {
          throw "Requested an update on a node with no children";
        }
        if (!left_son) {
          data = right_son->data;
          return;
        }
        if (!right_son) {
          data = left_son->data;
          return;
        }
        data = Union(left_son->data, right_son->data);
      }
      void UpdateAncestors() {
        decltype(ancestor) current_node = ancestor;
        while (current_node) {
          current_node->Update();
          current_node = current_node->ancestor;
        }
      }
      void CreateRightSon() {
        if (!right_son) {
          right_son.reset(new TreeNode(this));
        }
      }
      void CreateLeftSon() {
        if (!left_son) {
          left_son.reset(new TreeNode(this));
        }
      }
      // constant reference constructor
      TreeNode(const TreeNode& node) :
              data(node.data),
              left_son(node.left_son),
              right_son(node.right_son),
              ancestor(node.ancestor) {}
      // std::move constructor
      TreeNode(TreeNode&& node) {
        data = std::move(node.data);
        // pointer reassignment
        left_son.reset(node.left_son);
        right_son.reset(node.right_son);
        ancestor = node.ancestor;
        node.left_son.reset();
        node.right_son.reset();
      }
      // initialize with an ancestor
      TreeNode(TreeNode* anc) : ancestor(anc) {};
      TreeNode() = default;
    public:
      ~TreeNode() {
        left_son.reset();
        right_son.reset();
      }
      friend class SegmentTree;
    public:
      // Assigning a new data value to the node
      TreeNode& operator=(const T& new_data) {
        data = new_data;
        UpdateAncestors();
      }
      TreeNode& operator=(T&& new_data) {
        data = std::move(new_data);
        UpdateAncestors();
	return *this;
      }
    };
    std::shared_ptr<TreeNode> root;
    int l_border = 0;
    int r_border = 0;
    // getting a segment's union
    T Get(TreeNode* node, int l, int r, int req_l, int req_r) {
      if (!node || l > req_r || r < req_l) {
        return default_value;
      }
      if (l >= req_l && r <= req_r) {
        return node->data;
      }
      int mid = (l + r) / 2;
      return Union(Get(node->left_son.get(), l, mid, req_l, req_r),
                   Get(node->right_son.get(), mid + 1, r, req_l, req_r));
    }

  public:
    // initialize a segment tree on a segment [l, r]
    SegmentTree(int l, int r) {
      if (l > r) {
        throw "Invalid segment";
      }
      root.reset(new TreeNode());
      l_border = l;
      r_border = r;
    }
    ~SegmentTree() {
      root.reset();
    }
    TreeNode& operator[](int index) {
      TreeNode* current_node = root.get();
      int l = l_border;
      int r = r_border;
      while (l != index || r != index) {
        int mid = (l + r) / 2;
        if (mid >= index) {
          current_node->CreateLeftSon();
          current_node = current_node->left_son.get();
	  r = mid;
        }
        else {
          current_node->CreateRightSon();
          current_node = current_node->right_son.get();
	  l = mid + 1;
        }
      }
      return *current_node;
    }
    T Get(int l, int r) {
      return Get(root.get(), l_border, r_border, l, r);
    }
  };

};
// namespace algo

#endif //SEGMENTTREE_SEGMENTTREE_H
