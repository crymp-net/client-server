#pragma once

#include <functional>
#include <map>
#include <stack>
#include <string>
#include <string_view>
#include <variant>

#include "PathTools.h"

template<class T, class Compare = std::less<void>>
struct PathTree
{
	struct Node;
	using DirectoryNode = std::map<std::string, Node, Compare>;

	struct Node : public std::variant<DirectoryNode, T>
	{
	};

	Node root;

	Node* AddDirectory(std::string_view path, bool& added, Node* node = nullptr)
	{
		if (!node)
		{
			node = &this->root;
		}

		while (!path.empty())
		{
			const auto [pathComponent, remainingPath] = PathTools::SplitFirstPathComponent(path);

			DirectoryNode* dirNode = std::get_if<DirectoryNode>(node);
			if (!dirNode)
			{
				return nullptr;
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

		return node;
	}

	DirectoryNode* AddDirectoryNode(std::string_view path, bool& added, Node* node = nullptr)
	{
		return std::get_if<DirectoryNode>(this->AddDirectory(path, added, node));
	}

	T* AddNode(std::string_view path, bool& added, Node* node = nullptr)
	{
		node = this->AddDirectory(path, added, node);

		return added ? &node->emplace<T>() : std::get_if<T>(node);
	}

	Node* Find(std::string_view path, Node* node = nullptr)
	{
		if (!node)
		{
			node = &this->root;
		}

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

	DirectoryNode* FindDirectoryNode(std::string_view path, Node* node = nullptr)
	{
		return std::get_if<DirectoryNode>(this->Find(path, node));
	}

	T* FindNode(std::string_view path, Node* node = nullptr)
	{
		return std::get_if<T>(this->Find(path, node));
	}

	void Erase(std::string_view path, Node* node = nullptr)
	{
		if (!node)
		{
			node = &this->root;
		}

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
				return;
			}

			const auto it = dirNode->find(pathComponent);
			if (it == dirNode->end())
			{
				return;
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
				return;
			}

			stack.pop();
		}
	}

	template<class Predicate>
	void EraseIf(Predicate predicate, Node* node = nullptr)
	{
		if (!node)
		{
			node = &this->root;
		}

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
