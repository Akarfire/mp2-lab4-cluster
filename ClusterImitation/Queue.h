#pragma once
#include <stdexcept>


// List based queue

template<class T>
class TQueue
{
	template<class T>
	struct TNode
	{
		T Data;
		TNode<T>* pNext;

		TNode(const T& InData, TNode<T>* InNext): Data(InData), pNext(InNext) {}
	};

	// Pointer to the Fist and the Last elements
	TNode<T>* pFirst;
	TNode<T>* pLast;

	// The length of the queue
	size_t Len;

	// Returns a pointer to an element at position Pos (starting from the head of the queue)
	TNode<T>* GetElementAtPosition(size_t Pos) const
	{
		if (Pos < 0 || Len <= Pos)
			return nullptr;

		TNode<T>* Node = pFirst;
		size_t i = Pos;

		while (i > 0)
		{
			Node = Node->pNext;
			i--;
		}

		return Node;
	}

public:

	// Construction / Destruction
	TQueue(): Len(0) {}
	~TQueue() 
	{
		TNode<T>* Tmp;
		while (pFirst)
		{
			Tmp = pFirst->pNext;
			delete pFirst;
			pFirst = Tmp;
		}
	}

	// Utility
	bool empty() const { return Len == 0; }
	size_t size() const { return Len; }

	// Methods
	// Puts a new element to the end of the queue
	void Put(const T& InData)
	{
		TNode<T>* NewNode = new TNode<T>(InData, nullptr);

		if (!pFirst)
			pFirst = NewNode;

		if (pLast)
			pLast->pNext = NewNode;

		pLast = NewNode;
		Len++;
	}

	// Returns the value of the queue element at position Pos (starting from the head of the queue)
	const T& Check(size_t Pos = 0) const
	{
		TNode<T>* Node = GetElementAtPosition(Pos);

		if (!Node)
			throw(std::runtime_error("Queue check index out of range!"));

		return Node->Data;
	}

	// Deletes an element of the queue at position Pos (starting from the head of the queue);
	void Pop(size_t Pos = 0)
	{
		if (empty())
			throw(std::runtime_error("Popping empty queue"));

		if (Pos < 0 || Len <= Pos)
			throw(std::runtime_error("Queue pop index out of range!"));

		if (Len == 1)
		{
			delete pFirst;
			pFirst = nullptr;
			pLast = nullptr;
			Len = 0;

			return;
		}

		if (Pos == 0)
		{
			TNode<T>* NewFirst = pFirst->pNext;
			delete pFirst;
			pFirst = NewFirst;
			Len--;

			return;
		}

		if (Pos == size() - 1)
		{
			TNode<T>* PrevNode = GetElementAtPosition(Pos - 1);
			PrevNode->pNext = nullptr;

			delete pLast;
			pLast = PrevNode;

			Len--;

			return;
		}

		TNode<T>* PrevNode = GetElementAtPosition(Pos - 1);
		TNode<T>* Node = PrevNode->pNext;

		if (!PrevNode)
			throw(std::runtime_error("Queue Pop index out of range!"));

		PrevNode->pNext = Node->pNext;
		delete Node;
		Len--;
	}

};