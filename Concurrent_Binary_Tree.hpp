#ifndef CONCURRENT_BINARY_TREE_HPP
#define CONCURRENT_BINARY_TREE_HPP

#include <stack>
#include <set>
#include <shared_mutex>
#include <atomic>
#include <compare>

template <typename T>
class Node {
private:
	T value;
	std::shared_ptr<Node<T>> leftChildPtr;
	std::shared_ptr<Node<T>> rightChildPtr;
	mutable std::shared_mutex smtx;

public:
	Node(T value_arg) {
		value = value_arg;
		leftChildPtr = nullptr;
		rightChildPtr = nullptr;
	}
	void lock_mtx_shared() {
		smtx.lock_shared();
	}
	void unlock_mtx_shared() {
		smtx.unlock_shared();
	}
	void lock_mtx_exclusive() {
		smtx.lock();
	}
	void unlock_mtx_exclusive() {
		smtx.unlock();
	}
	bool setLeftChild(T left_child_value) {
		if (left_child_value >= value) {
			return false;
		}
		leftChildPtr = std::make_shared<Node<T>>(left_child_value);
		return true;
	}
	// member overload needed for delete - need to re-assign pointers for deleting node
	bool setLeftChild(std::shared_ptr<Node<T>>& left_child) {
		if (left_child != nullptr and left_child->getValue() >= value) {
			return false;
		}
		leftChildPtr = left_child;
		return true;
	}
	bool setLeftChild(std::nullptr_t) {
		leftChildPtr = nullptr;
		return true;
	}
	bool setRightChild(T right_child_value) {
		if (right_child_value <= value) {
			return false;
		}
		rightChildPtr = std::make_shared<Node<T>>(right_child_value);
		return true;
	}
	// member overload needed for delete - need to re-assign pointers for deleting node
	bool setRightChild(std::shared_ptr<Node<T>>& right_child) {
		if (right_child != nullptr and right_child->getValue() < value) {
			return false;
		}
		rightChildPtr = right_child;
		return true;
	}
	bool setRightChild(std::nullptr_t) {
		rightChildPtr = nullptr;
		return true;
	}
	std::shared_ptr<Node<T>>& getLeftChild() {
		return leftChildPtr;
	}
	std::shared_ptr<Node<T>>& getRightChild() {
		return rightChildPtr;
	}
	T getValue() {
		return value;
	}
	void setValue(T new_value) {
		value = new_value;
	}
	std::strong_ordering operator<=>(const Node<T>& other) const {
		if (auto cmp = value <=> other.value; cmp != 0) {
			return cmp;
		}
	}
	bool operator==(const Node<T>& other) const {
		if (value == other.value) {
			return true;
		}
		return false;
	}
};

template <typename T>
class Concurrent_Binary_Tree {
private:
	std::shared_ptr<Node<T>> root;
	size_t cachedDepth;
	std::atomic_size_t numNodes;
	mutable std::shared_mutex root_smtx;
	
public:
	Concurrent_Binary_Tree() {
		root = nullptr;
		cachedDepth = 0;
		numNodes = 0;
	}
	~Concurrent_Binary_Tree()
	{
		std::stack<std::shared_ptr<Node<T>>> deleteStack;
		std::deque<std::shared_ptr<Node<T>>> nodeQueue;
		if (root != nullptr) {
			nodeQueue.push_back(root);
			while (!nodeQueue.empty()) {
				deleteStack.push(nodeQueue.front());
				if (nodeQueue.front()->getLeftChild() != nullptr) {
					nodeQueue.push_back(nodeQueue.front()->getLeftChild());
				}
				if (nodeQueue.front()->getRightChild() != nullptr) {
					nodeQueue.push_back(nodeQueue.front()->getRightChild());
				}
				nodeQueue.pop_front();
			}
			while (!deleteStack.empty()) {
				deleteStack.top()->setLeftChild(nullptr);
				deleteStack.top()->setRightChild(nullptr);
				deleteStack.pop();
			}
			root = nullptr;
		}
	}
	bool addIterative(T valueToAdd) {
		root_smtx.lock();
		if (root == nullptr) {
			root = std::make_shared<Node<T>>(valueToAdd);
			root_smtx.unlock();
			numNodes++;
			return true;
		}
		else {
			root_smtx.unlock();
		}
		root_smtx.lock_shared();
		std::shared_ptr<Node<T>> currentNode = root;
		currentNode->lock_mtx_exclusive();
		root_smtx.unlock_shared();
		
		while (1) {
			if(valueToAdd > currentNode->getValue() and currentNode->getRightChild() == nullptr) {
				currentNode->setRightChild(valueToAdd);
				currentNode->unlock_mtx_exclusive();
				numNodes++;
				return true;
			}
			else if(valueToAdd > currentNode->getValue() and currentNode->getRightChild() != nullptr) {
				currentNode->getRightChild()->lock_mtx_exclusive();
				currentNode->unlock_mtx_exclusive();
				currentNode = currentNode->getRightChild();
			}
			else if(valueToAdd < currentNode->getValue() and currentNode->getLeftChild() == nullptr) {
				currentNode->setLeftChild(valueToAdd);
				currentNode->unlock_mtx_exclusive();
				numNodes++;
				return true;
			}
			else if(valueToAdd < currentNode->getValue() and currentNode->getLeftChild() != nullptr) {
				currentNode->getLeftChild()->lock_mtx_exclusive();
				currentNode->unlock_mtx_exclusive();
				currentNode = currentNode->getLeftChild();
			}
			else if(valueToAdd == currentNode->getValue()) {
				currentNode->unlock_mtx_exclusive();
				return false;
			}
		}
	}
	bool deleteIterative(T valueToDelete) {
		root_smtx.lock();
		if (root == nullptr) {
			root_smtx.unlock();
			return false;
		}
		std::shared_ptr<Node<T>> currentNode = root;
		currentNode->lock_mtx_exclusive();
		root_smtx.unlock();

		std::shared_ptr<Node<T>> followNode = nullptr;

		while (1) {
			if(valueToDelete > currentNode->getValue() and currentNode->getRightChild() == nullptr) {
				if (followNode != nullptr) {
					followNode->unlock_mtx_exclusive();
				}
				currentNode->unlock_mtx_exclusive();
				return false;
			}
			else if(valueToDelete < currentNode->getValue() and currentNode->getLeftChild() == nullptr) {
				if (followNode != nullptr) {
					followNode->unlock_mtx_exclusive();
				}
				currentNode->unlock_mtx_exclusive();
				return false;
			}
			else if(currentNode->getValue() == valueToDelete) {
				std::shared_ptr<Node<T>> mmFollower;
				std::shared_ptr<Node<T>> mmLead;

				// get min from right subtree
				if(currentNode->getRightChild() != nullptr) {
					currentNode->getRightChild()->lock_mtx_exclusive();

					mmFollower = currentNode;
					mmLead = currentNode->getRightChild();

					while (1) {
						if (mmLead->getLeftChild() != nullptr) {
							mmLead->getLeftChild()->lock_mtx_exclusive();
							if (mmFollower != currentNode) {
								mmFollower->unlock_mtx_exclusive();
							}
							mmFollower = mmLead;
							mmLead = mmLead->getLeftChild();
						}
						else {
							break;
						}
					}
					// replace currentNode value with min in mmLead
					currentNode->setValue(mmLead->getValue());
					// ensure parent of min now points past it
					if (mmLead->getRightChild() != nullptr) {
						mmLead->getRightChild()->lock_mtx_shared();
					}
					if (mmFollower->getLeftChild() == mmLead) {
						mmFollower->setLeftChild(mmLead->getRightChild());
					}
					else if (mmFollower->getRightChild() == mmLead) {
						mmFollower->setRightChild(mmLead->getRightChild());
					}
					if (mmLead->getRightChild() != nullptr) {
						mmLead->getRightChild()->unlock_mtx_shared();
					}

					if (mmFollower != currentNode) {
						mmFollower->unlock_mtx_exclusive();
					}
					mmLead->unlock_mtx_exclusive();
				}
				// if no min, get max from left subtree
				else if(currentNode->getLeftChild() != nullptr) {
					currentNode->getLeftChild()->lock_mtx_exclusive();

					mmFollower = currentNode;
					mmLead = currentNode->getLeftChild();

					while (1) {
						if (mmLead->getRightChild() != nullptr) {
							mmLead->getRightChild()->lock_mtx_exclusive();
							if (mmFollower != currentNode) {
								mmFollower->unlock_mtx_exclusive();
							}
							mmFollower = mmLead;
							mmLead = mmLead->getRightChild();
						}
						else {
							break;
						}
					}
					// replace currentNode value with min in mmLead
					currentNode->setValue(mmLead->getValue());
					// ensure parent of max now points past it
					if (mmLead->getLeftChild() != nullptr) {
						mmLead->getLeftChild()->lock_mtx_shared();
					}
					if (mmFollower->getLeftChild() == mmLead) {
						mmFollower->setLeftChild(mmLead->getLeftChild());
					}
					else if(mmFollower->getRightChild() == mmLead) {
						mmFollower->setRightChild(mmLead->getLeftChild());
					}
					if (mmLead->getLeftChild() != nullptr) {
						mmLead->getLeftChild()->unlock_mtx_shared();
					}
					if (mmFollower != currentNode) {
						mmFollower->unlock_mtx_exclusive();
					}
					mmLead->unlock_mtx_exclusive();
				}
				else {
					//currentNode is value to delete
					//left and right child both nullptr
					root_smtx.lock();
					if(root == currentNode) {
						root = nullptr;
						root_smtx.unlock();
					}
					else {
						root_smtx.unlock();
						if (followNode->getLeftChild() == currentNode) {
							followNode->setLeftChild(nullptr);
						}
						else if (followNode->getRightChild() == currentNode) {
							followNode->setRightChild(nullptr);
						}
					}
				}
				if (followNode != nullptr) {
					followNode->unlock_mtx_exclusive();
				}
				currentNode->unlock_mtx_exclusive();
				numNodes--;
				return true;
			}
			else if(valueToDelete > currentNode->getValue() and currentNode->getRightChild() != nullptr) {
				if (followNode != nullptr) {
					followNode->unlock_mtx_exclusive();
				}
				currentNode->getRightChild()->lock_mtx_exclusive();
				followNode = currentNode;
				currentNode = currentNode->getRightChild();
			}
			else if(valueToDelete < currentNode->getValue() and currentNode->getLeftChild() != nullptr) {
				if (followNode != nullptr) {
					followNode->unlock_mtx_exclusive();
				}
				currentNode->getLeftChild()->lock_mtx_exclusive();
				followNode = currentNode;
				currentNode = currentNode->getLeftChild();
			}
		}
	}
	bool containsIterative(T valueToSearch) {
		if (root == nullptr) {
			return false;
		}
		std::deque<std::shared_ptr<Node<T>>> nodeQueue;
		nodeQueue.push_back(root);
		while (!nodeQueue.empty()) {
			if (nodeQueue.front()->getLeftChild() != nullptr) {
				nodeQueue.push_back(nodeQueue.front()->getLeftChild());
			}
			if (nodeQueue.front()->getRightChild() != nullptr) {
				nodeQueue.push_back(nodeQueue.front()->getRightChild());
			}
			if (nodeQueue.front()->getValue() == valueToSearch) {
				return true;
			}
			nodeQueue.pop_front();
		}
		return false;
	}
	T getRootValue() {
		return root->getValue();
	}
	size_t getDepth() {
		if (root == nullptr) {
			return 0;
		}
		size_t maxDepth = 1;
		size_t searchingDepth = 1;
		std::set<T> traversed{};
		std::stack<std::shared_ptr<Node<T>>> nodeStack;
		nodeStack.push(root);
		traversed.insert(root->getValue());
		while (!nodeStack.empty()) {
			if (nodeStack.top()->getLeftChild() != nullptr and !traversed.contains(nodeStack.top()->getLeftChild()->getValue())) {
				nodeStack.push(nodeStack.top()->getLeftChild());
				traversed.insert(nodeStack.top()->getValue());
				searchingDepth++;
			}
			else if (nodeStack.top()->getRightChild() != nullptr and !traversed.contains(nodeStack.top()->getRightChild()->getValue())) {
				nodeStack.push(nodeStack.top()->getRightChild());
				traversed.insert(nodeStack.top()->getValue());
				searchingDepth++;
			}
			else {
				if (searchingDepth > maxDepth) {
					maxDepth = searchingDepth;
				}
				nodeStack.pop();
				searchingDepth--;
			}
		}
		cachedDepth = maxDepth;
		return maxDepth;
	}

	size_t getCachedDepth() {
		return cachedDepth;
	}
	
	size_t getNumNodes() {
		return numNodes;
	}
	
	T getMax() {
		root_smtx.lock_shared();
		std::shared_ptr<Node<T>> currentNode = root;
		currentNode->lock_mtx_shared();
		root_smtx.unlock_shared();
		while (currentNode->getRightChild() != nullptr) {
			currentNode->getRightChild()->lock_mtx_shared();
			auto temp = currentNode->getRightChild();
			currentNode->unlock_mtx_shared();
			currentNode = temp;
		}
		auto maxToReturn = currentNode->getValue();
		currentNode->unlock_mtx_shared();
		return maxToReturn;
	}
	T getMin() {
		root_smtx.lock_shared();
		Node<T> *currentNode = root.get();
		currentNode->lock_mtx_shared();
		root_smtx.unlock_shared();
		while (currentNode->getLeftChild() != nullptr) {
			currentNode->getLeftChild()->lock_mtx_shared();
			currentNode->unlock_mtx_shared();
			currentNode = currentNode->getLeftChild().get();
		}
		auto maxToReturn = currentNode->getValue();
		currentNode->unlock_mtx_shared();
		return maxToReturn;
	}
	std::vector<T> getPreorderTraversal() {
		//Preorder Traversal
		std::vector<T> preorderTraversal;
		if (root == nullptr) {
			return preorderTraversal;
		}
		std::stack<std::shared_ptr<Node<T>>> nodeStack;
		nodeStack.push(root);
		std::set<T> traversed{};
		while (!nodeStack.empty()) {
			if (!traversed.contains(nodeStack.top()->getValue())) {
				preorderTraversal.push_back(nodeStack.top()->getValue());
				traversed.insert(nodeStack.top()->getValue());
			}
			if (nodeStack.top()->getLeftChild() != nullptr and !traversed.contains(nodeStack.top()->getLeftChild()->getValue())) {
				nodeStack.push(nodeStack.top()->getLeftChild());
			}
			else if (nodeStack.top()->getRightChild() != nullptr and !traversed.contains(nodeStack.top()->getRightChild()->getValue())) {
				std::shared_ptr<Node<T>> temp = nodeStack.top()->getRightChild();
				nodeStack.pop();
				nodeStack.push(temp);
			}
			else {
				nodeStack.pop();
			}
		}
		return preorderTraversal;
	}
	std::vector<T> getReversePreorderTraversal() {
		//reverse preorder Traversal
		std::vector<T> reversePreorderTraversal;
		if (root == nullptr) {
			return reversePreorderTraversal;
		}
		std::stack<std::shared_ptr<Node<T>>> nodeStack;
		nodeStack.push(root);
		std::set<T> traversed{};
		while (!nodeStack.empty()) {
			if (!traversed.contains(nodeStack.top()->getValue())) {
				reversePreorderTraversal.push_back(nodeStack.top()->getValue());
				traversed.insert(nodeStack.top()->getValue());
			}
			if (nodeStack.top()->getRightChild() != nullptr and !traversed.contains(nodeStack.top()->getRightChild()->getValue())) {
				nodeStack.push(nodeStack.top()->getRightChild());
			}
			else if (nodeStack.top()->getLeftChild() != nullptr and !traversed.contains(nodeStack.top()->getLeftChild()->getValue())) {
				std::shared_ptr<Node<T>> temp = nodeStack.top()->getLeftChild();
				nodeStack.pop();
				nodeStack.push(temp);
			}
			else {
				nodeStack.pop();
			}
		}
		return reversePreorderTraversal;
	}
	std::vector<T> getPostorderTraversal() {
		//post order traversal
		std::vector<T> postorderTraversal;
		if (root == nullptr) {
			return postorderTraversal;
		}
		std::stack<std::shared_ptr<Node<T>>> nodeStack;
		nodeStack.push(root);
		std::set<T> traversed{};
		while (!nodeStack.empty()) {
			if (nodeStack.top()->getLeftChild() != nullptr and !traversed.contains(nodeStack.top()->getLeftChild()->getValue())) {
				nodeStack.push(nodeStack.top()->getLeftChild());
			}
			else if (nodeStack.top()->getRightChild() != nullptr and !traversed.contains(nodeStack.top()->getRightChild()->getValue())) {
				nodeStack.push(nodeStack.top()->getRightChild());
			}
			else if (!traversed.contains(nodeStack.top()->getValue())) {
				postorderTraversal.push_back(nodeStack.top()->getValue());
				traversed.insert(nodeStack.top()->getValue());
				nodeStack.pop();
			}
		}
		return postorderTraversal;
	}
	std::vector<T> getInorderTraversal() {
		// In order traversal
		// go all the way to the left
		// then visit left, then visit middle
		std::vector<T> inorderTraversal;
		if (root == nullptr) {
			return inorderTraversal;
		}
		std::stack<std::shared_ptr<Node<T>>> nodeStack;
		nodeStack.push(root);
		std::set<T> traversed{};
		while (!nodeStack.empty()) {
			while (nodeStack.top()->getLeftChild() != nullptr and !traversed.contains(nodeStack.top()->getLeftChild()->getValue())) {
				nodeStack.push(nodeStack.top()->getLeftChild());
			}
			inorderTraversal.push_back(nodeStack.top()->getValue());
			traversed.insert(nodeStack.top()->getValue());
			if (nodeStack.top()->getRightChild() != nullptr and !traversed.contains(nodeStack.top()->getRightChild()->getValue())) {
				std::shared_ptr<Node<T>> temp = nodeStack.top()->getRightChild();
				nodeStack.pop();
				nodeStack.push(temp);
			}
			else {
				nodeStack.pop();
			}
		}
		return inorderTraversal;
	}
	std::vector<T> getReverseInorderTraversal() {
		// reverse In order traversal
		// go all the way to the right
		// then visit middle, then visit left
		std::vector<T> reverseInorderTraversal;
		if (root == nullptr) {
			return reverseInorderTraversal;
		}
		std::stack<std::shared_ptr<Node<T>>> nodeStack;
		nodeStack.push(root);
		std::set<T> traversed{};
		while (!nodeStack.empty()) {
			while (nodeStack.top()->getRightChild() != nullptr and !traversed.contains(nodeStack.top()->getRightChild()->getValue())) {
				nodeStack.push(nodeStack.top()->getRightChild());
			}
			reverseInorderTraversal.push_back(nodeStack.top()->getValue());
			traversed.insert(nodeStack.top()->getValue());
			if (nodeStack.top()->getLeftChild() != nullptr and !traversed.contains(nodeStack.top()->getLeftChild()->getValue())) {
				std::shared_ptr<Node<T>> temp = nodeStack.top()->getLeftChild();
				nodeStack.pop();
				nodeStack.push(temp);
			}
			else {
				nodeStack.pop();
			}
		}
		return reverseInorderTraversal;
	}
	std::vector<T> getBreadthFirstTraversal() {
		std::vector<T> breadthFirstTraversal;
		if (root == nullptr) {
			return breadthFirstTraversal;
		}
		std::deque<std::shared_ptr<Node<T>>> nodeQueue;
		nodeQueue.push_back(root);
		while (!nodeQueue.empty()) {
			breadthFirstTraversal.push_back(nodeQueue.front()->getValue());
			if (nodeQueue.front()->getLeftChild() != nullptr) {
				nodeQueue.push_back(nodeQueue.front()->getLeftChild());
			}
			if (nodeQueue.front()->getRightChild() != nullptr) {
				nodeQueue.push_back(nodeQueue.front()->getRightChild());
			}
			nodeQueue.pop_front();
		}
		return breadthFirstTraversal;
	}
};

#endif // CONCURRENT_BINARY_TREE_HPP