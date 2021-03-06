#ifndef SEGMENTTREE_H_
#define SEGMENTTREE_H_

#include <algorithm>
#include <memory>
#include <functional>
#include <utility>

namespace algo {
  template<typename T, T(*Union)(T, T), T default_value = T()>
  class SegmentTree {
    protected:
    class TreeNode {
      friend class Iterator;
      friend class SegmentTree;

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
          right_son.reset(new TreeNode(this, false));
        }
      }
      void CreateLeftSon() {
        if (!left_son) {
          left_son.reset(new TreeNode(this, true));
        }
      }
      bool const is_left_son = false;
      // as default the copy is simple
      TreeNode(const TreeNode& node) :
              data(node.data),
              left_son(node.left_son),
              right_son(node.right_son),
              ancestor(node.ancestor),
              is_left_son(node.is_left_son) {}
      // but this copies the entire subtree
      static TreeNode* StrictCopy(const TreeNode& other) {
        TreeNode* result = new TreeNode(other.data);
        if (other.left_son) {
          result->left_son.reset(StrictCopy(*other.left_son));
        }
        if (other.right_son) {
          result->right_son.reset(StrictCopy(*other.right_son));
        }
        return result;
      }
      // std::move constructor
      TreeNode(TreeNode&& node) {
        data = std::move(node.data);
        is_left_son = node.is_left_son;
        // pointer reassignment
        left_son.reset(node.left_son);
        right_son.reset(node.right_son);
        ancestor = node.ancestor;
      }
      // initialize with an ancestor
      TreeNode(TreeNode* anc, bool is_left) :
              ancestor(anc),
              is_left_son(is_left) {}
      TreeNode() = default;
      explicit TreeNode(const T& data_init) {
        data = data_init;
      }

     public:
      ~TreeNode() {
        left_son.reset();
        right_son.reset();
      }
      // Assigning a new data value to the node
      TreeNode& operator=(const T& new_data) {
        data = new_data;
        UpdateAncestors();
        return *this;
      }
      TreeNode& operator=(T&& new_data) {
        data = std::move(new_data);
        UpdateAncestors();
        return *this;
      }
      friend class SegmentTree;
    };

    /* The iterator allows the user to efficiently iterate through
       all the defined elements in the segment tree.
       Iterating through all the elements will work in O(n), where
       n is the amount of nodes in the tree.
       If the tree is fully defined n will equal at most twice
       the amount of elements in the tree. */
    class Iterator : public std::iterator<std::input_iterator_tag, T> {
      friend class SegmentTree;
     private:
      TreeNode* node;
      Iterator(TreeNode* node_ptr) : node(node_ptr) {}
     public:
      typename Iterator::reference operator*() const {
        return node->data;
      }
      Iterator& operator++() {
        while(!node->is_left_son || !node->ancestor->right_son) {
          if (!node->ancestor) {
            node = nullptr;
            return *this;
          }
          node = node->ancestor;
        }
        node = node->ancestor;
        node = node->right_son.get();
        while(node->left_son) {
          node = node->left_son.get();
        }
        return *this;
      }
      bool operator==(const Iterator& other) const {
        return node == other.node;
      }
      bool operator!=(const Iterator& other) const {
        return node != other.node; 
      }
    };
    std::shared_ptr<TreeNode> root;
    int l_border = 0;
    int r_border = 0;
    // getting a segment's union
    T Get(TreeNode* node, int l, int r, int req_l, int req_r) const {
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
    SegmentTree(const SegmentTree& other) {
      root.reset(TreeNode::StrictCopy(*other.root));
      l_border = other.l_border;
      r_border = other.r_border;
    }
    ~SegmentTree() {
      root.reset();
    }
    SegmentTree& operator=(const SegmentTree& other) {
      root.reset();
      new (this) SegmentTree(other);
      return *this;
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
    T Get(int l, int r) const {
      return Get(root.get(), l_border, r_border, l, r);
    }
    Iterator end() {
      return Iterator(nullptr);
    }
    Iterator begin() {
      if (root->left_son) {
        TreeNode* v = root.get();
        while(v->left_son) {
          v = v->left_son.get();
        }
        return Iterator(v);
      }
      else {
        return Iterator(nullptr);
      }
    }    
  };
};
// namespace algo

#endif // SEGMENTTREE_SEGMENTTREE_H
