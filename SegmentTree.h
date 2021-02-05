//
// Created by Pavel Sankin on 04.02.2021.
//

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
      // segment borders corresponding to this node
      int l = 0, r = 0;

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
        if (l == r) {
          return;
        }
        if (!right_son) {
          right_son.reset(new TreeNode(this, (l + r) / 2 + 1, r));
        }
      }
      void CreateLeftSon() {
        if (l == r) {
          return;
        }
        if (!left_son) {
          left_son.reset(new TreeNode(this, l, (l + r) / 2));
        }
      }
      // constant reference constructor
      TreeNode(const TreeNode& node) :
              data(node.data),
              left_son(node.left_son),
              right_son(node.right_son),
              ancestor(node.ancestor),
              l(node.l),
              r(node.r)  {}
      // std::move constructor
      TreeNode(TreeNode&& node) {
        data = std::move(node.data);
        l = node.l;
        r = node.r;
        // pointer reassignment
        left_son.reset(node.left_son);
        right_son.reset(node.right_son);
        ancestor = node.ancestor;
        node.left_son.reset();
        node.right_son.reset();
        node.ancestor.reset();
      }
      // initialize with an ancestor and a segment
      TreeNode(TreeNode* anc, int l_, int r_) :
              ancestor(anc), l(l_), r(r_) {};
      TreeNode() = default;
    public:
      ~TreeNode() {
        left_son.reset();
        right_son.reset();
      }
      friend class SegmentTree;
    public:
      // Assigning new data values to the node
      TreeNode& operator=(const T& new_data) {
        data = new_data;
        UpdateAncestors();
      }
      TreeNode& operator=(T&& new_data) {
        data = std::move(new_data);
        UpdateAncestors();
      }
    };
    std::shared_ptr<TreeNode> root;
    // getting a value on a segment
    T Get(TreeNode* node, int req_l, int req_r) {
      if (!node || node->l > req_r || node->r < req_l) {
        return default_value;
      }
      if (node->l >= req_l && node->r <= req_r) {
        return node->data;
      }
      return Union(Get(node->left_son.get(), req_l, req_r),
                   Get(node->right_son.get(), req_l, req_r));
    }

  public:
    // initialize a segment tree on a segment [l, r]
    SegmentTree(int l, int r) {
      if (l > r) {
        throw "Invalid segment";
      }
      root.reset(new TreeNode());
      root->l = l;
      root->r = r;
    }
    ~SegmentTree() {
      TreeNode* v = root.get();

      // going over the tree with a cycle to avoid recursion
      while(v->left_son || v->right_son ||
            v->ancestor) {
        // first delete all the sons
        if (v->left_son) {
          v = v->left_son.get();
          continue;
        }
        if (v->right_son) {
          v = v->right_son.get();
          continue;
        }
        v = v->ancestor;
        if (v->left_son) {
          v->left_son.reset();
        }
        else {
          v->right_son.reset();
        }
      }
      root.reset();
    }

    TreeNode& operator[](int index) {
      TreeNode* current_node = root.get();
      while (current_node->l != index || current_node->r != index) {
        int mid = (current_node->l + current_node->r) / 2;
        if (mid >= index) {
          current_node->CreateLeftSon();
          current_node = current_node->left_son.get();
        }
        else {
          current_node->CreateRightSon();
          current_node = current_node->right_son.get();
        }
      }
      return *current_node;
    }
    T Get(int l, int r) {
      return Get(root.get(), l, r);
    }
  };

};
// namespace algo

#endif //SEGMENTTREE_SEGMENTTREE_H
