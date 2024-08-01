#pragma once

#include <functional>
#include <map>
#include <stack>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>

#include "PathTools.h"

template<class T, class Compare = std::less<void>>
struct PathTree
{
	struct Node;
	using DirectoryNode = std::map<std::string, Node, Compare>;

	// directory is the default
	struct Node : public std::variant<DirectoryNode, T>
	{
	};

	Node root;

	std::tuple<Node*, bool> AddDirectory(std::string_view path, Node* baseNode = nullptr)
	{
		bool added = false;
		Node* node = (baseNode) ? baseNode : &this->root;

		while (!path.empty())
		{
			const auto [pathComponent, remainingPath] = PathTools::SplitFirstPathComponent(path);

			DirectoryNode* dirNode = std::get_if<DirectoryNode>(node);
			if (!dirNode)
			{
				return {};
			}

			// a directory node is added
#ifdef __cpp_lib_associative_heterogeneous_insertion
			const auto result = dirNode->try_emplace(pathComponent);
#else
			const auto result = dirNode->try_emplace(std::string(pathComponent));
#endif

			added = result.second;
			node = &result.first->second;
			path = remainingPath;
		}

		return {node, added};
	}

	std::tuple<DirectoryNode*, bool> AddDirectoryNode(std::string_view path, Node* baseNode = nullptr)
	{
		auto [node, added] = this->AddDirectory(path, baseNode);

		return {std::get_if<DirectoryNode>(node), added};
	}

	std::tuple<T*, bool> AddNode(std::string_view path, Node* baseNode = nullptr)
	{
		auto [node, added] = this->AddDirectory(path, baseNode);

		return {added ? &node->emplace<T>() : std::get_if<T>(node), added};
	}

	Node* Find(std::string_view path, Node* baseNode = nullptr)
	{
		Node* node = (baseNode) ? baseNode : &this->root;

		while (!path.empty())
		{
			const auto [pathComponent, remainingPath] = PathTools::SplitFirstPathComponent(path);

			DirectoryNode* dirNode = std::get_if<DirectoryNode>(node);
			if (!dirNode)
			{
				return nullptr;
			}

			const auto it = dirNode->find(pathComponent);
			if (it == dirNode->end())
			{
				return nullptr;
			}

			node = &it->second;
			path = remainingPath;
		}

		return node;
	}

	DirectoryNode* FindDirectoryNode(std::string_view path, Node* baseNode = nullptr)
	{
		return std::get_if<DirectoryNode>(this->Find(path, baseNode));
	}

	T* FindNode(std::string_view path, Node* baseNode = nullptr)
	{
		return std::get_if<T>(this->Find(path, baseNode));
	}

	std::tuple<T*, std::string_view> FindNodeInPath(std::string_view path, Node* baseNode = nullptr)
	{
		Node* node = (baseNode) ? baseNode : &this->root;

		while (!path.empty())
		{
			const auto [pathComponent, remainingPath] = PathTools::SplitFirstPathComponent(path);

			DirectoryNode* dirNode = std::get_if<DirectoryNode>(node);
			if (!dirNode)
			{
				break;
			}

			const auto it = dirNode->find(pathComponent);
			if (it == dirNode->end())
			{
				return {};
			}

			node = &it->second;
			path = remainingPath;
		}

		return {std::get_if<T>(node), path};
	}

	bool Erase(std::string_view path, Node* baseNode = nullptr)
	{
		Node* node = (baseNode) ? baseNode : &this->root;

		struct Entry
		{
			DirectoryNode* dirNode = nullptr;
			typename DirectoryNode::iterator it;
		};

		std::stack<Entry> stack;

		while (!path.empty())
		{
			const auto [pathComponent, remainingPath] = PathTools::SplitFirstPathComponent(path);

			DirectoryNode* dirNode = std::get_if<DirectoryNode>(node);
			if (!dirNode)
			{
				return false;
			}

			const auto it = dirNode->find(pathComponent);
			if (it == dirNode->end())
			{
				return false;
			}

			stack.emplace(dirNode, it);

			node = &it->second;
			path = remainingPath;
		}

		while (!stack.empty())
		{
			Entry& entry = stack.top();

			entry.dirNode->erase(entry.it);

			if (!entry.dirNode->empty())
			{
				break;
			}

			stack.pop();
		}

		return true;
	}

	template<class Predicate>
	void EraseIf(Predicate predicate, Node* baseNode = nullptr)
	{
		Node* node = (baseNode) ? baseNode : &this->root;

		struct Branch
		{
			Node* node = nullptr;
			bool processed = false;
		};

		std::stack<Branch> stack;
		stack.emplace(node);

		while (!stack.empty())
		{
			Branch& branch = stack.top();

			DirectoryNode* dirNode = std::get_if<DirectoryNode>(branch.node);
			if (!dirNode)
			{
				stack.pop();
				continue;
			}

			if (branch.processed)
			{
				// remove empty directories
				std::erase_if(*dirNode, [](const DirectoryNode::value_type& entry) -> bool {
					const DirectoryNode* subdirNode = std::get_if<DirectoryNode>(&entry.second);
					return subdirNode && subdirNode->empty();
				});

				stack.pop();
				continue;
			}

			for (auto it = dirNode->begin(); it != dirNode->end();)
			{
				T* leaf = std::get_if<T>(&it->second);
				if (!leaf)
				{
					stack.emplace(&it->second);
					++it;
					continue;
				}

				if (predicate(*leaf))
				{
					it = dirNode->erase(it);
					continue;
				}

				++it;
			}

			branch.processed = true;
		}
	}
};
