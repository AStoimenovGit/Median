
#ifndef _AVLTree_h_
#define _AVLTree_h_

#undef min
#undef max
#include <algorithm>

#include "Median.h"

#define	OPTIMIZE

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
struct LessOrEqual
{
	constexpr bool operator () (const T& left, const T& right) const
	{
		return	left <= right;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
struct MoreOrEqual
{
	constexpr bool operator () (const T& left, const T& right) const
	{
		return	left >= right;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Balanced binary search tree
 */
template <class T, class Compare = LessOrEqual<T>>
class AVLTree
	: public Median<T, Compare>
{
	using BaseClass = Median<T, Compare>;

public:
	AVLTree();
	virtual ~AVLTree();

	virtual void	Clear();
	virtual void	Insert(const T& value);

	virtual bool	GetMedian(T& median) const;

private:
	class Node
	{
	public:
		Node(const T& value);
		~Node();

		Node(const Node&) = delete;
		Node&	operator = (const Node&) = delete;

		void	Insert(const T& value);

		T		GetValue() const;

		int		GetHeight() const;
		static int GetHeight(const Node* pNode);

#ifdef OPTIMIZE
		int		GetSize() const;
		static int GetSize(const Node* pNode);
#endif // OPTIMIZE

		Node*	GetLeft() {
			return m_pLeft;
		}
		const Node* GetLeft() const {
			return m_pLeft;
		}

		Node*	GetRight() {
			return m_pRight;
		}
		const Node* GetRight() const {
			return m_pRight;
		}

		Node*	GetFirst();
		const Node* GetFirst() const {
			return const_cast<Node*>(this)->GetFirst();
		}

		Node*	GetLast();
		const Node* GetLast() const {
			return const_cast<Node*>(this)->GetLast();
		}

		Node*	GetPrev();
		const Node* GetPrev() const {
			return const_cast<Node*>(this)->GetPrev();
		}

		Node*	GetNext();
		const Node* GetNext() const {
			return const_cast<Node*>(this)->GetNext();
		}

	private:
		void	UpdateHeights();
#ifdef OPTIMIZE
		void	UpdateSizes();
#endif // OPTIMIZE

		void	Detach();
		void	AttachNode(Node*& pChild, Node* pNode);
		void	AttachLeftNode(Node* pNode);
		void	AttachRightNode(Node* pNode);

		void	RotateLeft();
		void	RotateRight();

#ifdef OPTIMIZE
		void	Balance(bool propagate = true);
#else // OPTIMIZE
		void	Balance();
#endif // OPTIMIZE

#ifdef _DEBUG
		T		GetLowBound() const;
		T		GetHighBound() const;
		void	CheckBalanced() const;
#endif

	private:
		const T	m_value;
		int		m_height;
#ifdef OPTIMIZE
		int		m_size;
#endif // OPTIMIZE
		Node*	m_pLeft;
		Node*	m_pRight;
		Node*	m_pParent;

		static const Compare s_compare;
	};

	Node*		m_pRoot;
};

template <class T, class Compare>
/*static*/ const Compare AVLTree<T, Compare>::Node::s_compare;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline AVLTree<T, Compare>::AVLTree()
	: m_pRoot()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
/*virtual*/ AVLTree<T, Compare>::~AVLTree()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
/*virtual*/ void AVLTree<T, Compare>::Clear()
{
	BaseClass::Clear();

	delete m_pRoot;
	m_pRoot = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
/*virtual*/ void AVLTree<T, Compare>::Insert(const T& value)
{
	BaseClass::Insert(value);

	if (m_pRoot)
		m_pRoot->Insert(value);
	else
		m_pRoot = new Node(value);

	assert(BaseClass::m_size == m_pRoot->GetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
/*virtual*/ bool AVLTree<T, Compare>::GetMedian(T& median) const
{
	if (!BaseClass::m_size)
		return false;

	assert(m_pRoot);
#ifdef OPTIMIZE
	assert(std::abs(Node::GetSize(m_pRoot->GetLeft()) - Node::GetSize(m_pRoot->GetRight())) <= 1);
	if (BaseClass::m_size % 2)
	{
		median = m_pRoot->GetValue();
	}
	else if (Node::GetSize(m_pRoot->GetLeft()) > Node::GetSize(m_pRoot->GetRight()))
	{
		median = (m_pRoot->GetValue() + m_pRoot->GetPrev()->GetValue()) / static_cast<T>(2);
	}
	else
	{
		median = (m_pRoot->GetValue() + m_pRoot->GetNext()->GetValue()) / static_cast<T>(2);
	}
#else // OPTIMIZE
	const Node*	pNode = m_pRoot->GetFirst();
	const Node*	pPrevNode = nullptr;
	int	steps = BaseClass::m_size / 2;

	while (steps--)
	{
		pPrevNode = pNode;
		pNode = pNode->GetNext();
	}

	if (BaseClass::m_size % 2)
	{
		median = pNode->GetValue();
	}
	else if (pPrevNode)
	{
		median = (pPrevNode->GetValue() + pNode->GetValue()) / static_cast<T>(2);
	}
	else
	{
		median = pNode->GetValue();
	}
#endif // OPTIMIZE

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline AVLTree<T, Compare>::Node::Node(const T& value)
	: m_value(value)
	, m_height()
#ifdef OPTIMIZE
	, m_size(1)
#endif // OPTIMIZE
	, m_pLeft()
	, m_pRight()
	, m_pParent()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline AVLTree<T, Compare>::Node::~Node()
{
	Detach();
	delete m_pLeft;
	delete m_pRight;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
void AVLTree<T, Compare>::Node::Insert(const T& value)
{
	if (s_compare(m_value, value))
	{
		if (m_pRight)
		{
			m_pRight->Insert(value);
		}
		else
		{
			AttachRightNode(new Node(value));
			Balance();
		}
	}
	else
	{
		if (m_pLeft)
		{
			m_pLeft->Insert(value);
		}
		else
		{
			AttachLeftNode(new Node(value));
			Balance();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline T AVLTree<T, Compare>::Node::GetValue() const
{
	return m_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline int AVLTree<T, Compare>::Node::GetHeight() const
{
	return m_height;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef OPTIMIZE
template <class T, class Compare>
inline int AVLTree<T, Compare>::Node::GetSize() const
{
	return m_size;
}
#endif // OPTIMIZE

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
/*static*/ inline int AVLTree<T, Compare>::Node::GetHeight(const Node* pNode)
{
	if (pNode)
		return pNode->GetHeight();
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef OPTIMIZE
template <class T, class Compare>
/*static*/ inline int AVLTree<T, Compare>::Node::GetSize(const Node* pNode)
{
	if (pNode)
		return pNode->GetSize();
	else
		return 0;
}
#endif // OPTIMIZE

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
typename AVLTree<T, Compare>::Node* AVLTree<T, Compare>::Node::GetFirst()
{
	if (m_pLeft)
		return m_pLeft->GetFirst();
	else
		return this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
typename AVLTree<T, Compare>::Node* AVLTree<T, Compare>::Node::GetLast()
{
	if (m_pRight)
		return m_pRight->GetLast();
	else
		return this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
typename AVLTree<T, Compare>::Node* AVLTree<T, Compare>::Node::GetPrev()
{
	if (m_pLeft)
		return m_pLeft->GetLast();
	else if (!m_pParent)
		return nullptr;
	else if (m_pParent->m_pLeft == this)
		return m_pParent->m_pParent;
	else
		return m_pParent;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
typename AVLTree<T, Compare>::Node* AVLTree<T, Compare>::Node::GetNext()
{
	if (m_pRight)
		return m_pRight->GetFirst();
	else if (!m_pParent)
		return nullptr;
	else if (m_pParent->m_pRight == this)
		return m_pParent->m_pParent;
	else
		return m_pParent;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
void AVLTree<T, Compare>::Node::UpdateHeights()
{
	m_height = 1 + std::max(GetHeight(m_pLeft), GetHeight(m_pRight));

	if (m_pParent)
		m_pParent->UpdateHeights();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef OPTIMIZE
template <class T, class Compare>
void AVLTree<T, Compare>::Node::UpdateSizes()
{
	m_size = GetSize(m_pLeft) + 1 + GetSize(m_pRight);

	if (m_pParent)
		m_pParent->UpdateSizes();
}
#endif // OPTIMIZE

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline void AVLTree<T, Compare>::Node::Detach()
{
	if (nullptr == m_pParent)
	{
	}
	else if (this == m_pParent->m_pLeft)
	{
		m_pParent->m_pLeft = nullptr;
	}
	else if (this == m_pParent->m_pRight)
	{
		m_pParent->m_pRight = nullptr;
	}

	if (m_pParent)
	{
		m_pParent->UpdateHeights();
#ifdef OPTIMIZE
		m_pParent->UpdateSizes();
#endif // OPTIMIZE
	}

	m_pParent = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline void AVLTree<T, Compare>::Node::AttachNode(Node*& pChild, Node* pNode)
{
	delete pChild;
	pChild = nullptr;

	if (pNode)
	{
		pNode->Detach();
		pChild = pNode;
		pChild->m_pParent = this;
		pChild->UpdateHeights();
#ifdef OPTIMIZE
		pChild->UpdateSizes();
#endif // OPTIMIZE
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline void AVLTree<T, Compare>::Node::AttachLeftNode(Node* pNode)
{
	AttachNode(m_pLeft, pNode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline void AVLTree<T, Compare>::Node::AttachRightNode(Node* pNode)
{
	AttachNode(m_pRight, pNode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
void AVLTree<T, Compare>::Node::RotateLeft()
{
	if (nullptr == m_pRight)
		return;

	const T	rightValue = m_pRight->GetValue();
	const T thisValue = GetValue();

	Node*	pLeft = m_pLeft;
	if (pLeft)
		pLeft->Detach();

	Node*	pRightLeft = m_pRight->m_pLeft;
	if (pRightLeft)
		pRightLeft->Detach();

	Node*	pRightRight = m_pRight->m_pRight;
	if (pRightRight)
		pRightRight->Detach();

	const_cast<T&>(m_value) = rightValue;

	delete m_pRight;
	m_pRight = nullptr;

	AttachLeftNode(new Node(thisValue));
	m_pLeft->AttachLeftNode(pLeft);
	m_pLeft->AttachRightNode(pRightLeft);
	AttachRightNode(pRightRight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline void AVLTree<T, Compare>::Node::RotateRight()
{
	if (nullptr == m_pLeft)
		return;

	const T	leftValue = m_pLeft->GetValue();
	const T thisValue = GetValue();

	Node*	pRight = m_pRight;
	if (pRight)
		pRight->Detach();

	Node*	pLeftLeft = m_pLeft->m_pLeft;
	if (pLeftLeft)
		pLeftLeft->Detach();

	Node*	pLeftRight = m_pLeft->m_pRight;
	if (pLeftRight)
		pLeftRight->Detach();

	delete m_pLeft;
	m_pLeft = nullptr;

	const_cast<T&>(m_value) = leftValue;

	AttachRightNode(new Node(thisValue));
	m_pRight->AttachRightNode(pRight);
	m_pRight->AttachLeftNode(pLeftRight);
	AttachLeftNode(pLeftLeft);

	const_cast<T&>(m_value) = leftValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
#ifdef OPTIMIZE
void AVLTree<T, Compare>::Node::Balance(bool propagate)
#else // OPTIMIZE
void AVLTree<T, Compare>::Node::Balance()
#endif // OPTIMIZE
{
	auto	leftHeight = GetHeight(m_pLeft);
	auto	rightHeight = GetHeight(m_pRight);

	if (leftHeight - rightHeight > 1)
	{
		if (m_height == 2 && m_pLeft && m_pLeft->m_pRight)
			m_pLeft->RotateLeft();

		RotateRight();
	}
	else if (rightHeight - leftHeight > 1)
	{
		if (m_height == 2 && m_pRight && m_pRight->m_pLeft)
			m_pRight->RotateRight();

		RotateLeft();
	}

#ifdef OPTIMIZE
	auto	leftSize = GetSize(m_pLeft);
	auto	rightSize = GetSize(m_pRight);
	assert(std::abs(leftSize - rightSize) <= 2);

	if (leftSize - rightSize > 1)
	{
		const T thisValue = GetValue();

		Node*	pPrev = GetPrev();
		assert(pPrev);
		const T	prevValue = pPrev->GetValue();

		assert(!pPrev->m_pRight);
		if (Node* pPrevLeft = pPrev->m_pLeft)
		{
			pPrevLeft->Detach();

			Node*	pPrevParent = pPrev->m_pParent;
			assert(pPrevParent);
			delete pPrev;

			if (pPrevParent == this)
				pPrevParent->AttachLeftNode(pPrevLeft);
			else
				pPrevParent->AttachRightNode(pPrevLeft);
		}
		else
		{
			delete pPrev;
		}
		m_pLeft->Balance(false);

		const_cast<T&>(m_value) = prevValue;
		Insert(thisValue);
	}
	else if(rightSize - leftSize > 1)
	{
		const T thisValue = GetValue();

		Node*	pNext = GetNext();
		assert(pNext);
		const T	nextValue = pNext->GetValue();

		assert(!pNext->m_pLeft);
		if (Node* pNextRight = pNext->m_pRight)
		{
			pNextRight->Detach();

			Node*	pNextParent = pNext->m_pParent;
			assert(pNextParent);
			delete pNext;

			if (pNextParent == this)
				pNextParent->AttachRightNode(pNextRight);
			else
				pNextParent->AttachLeftNode(pNextRight);
		}
		else
		{
			delete pNext;
		}
		m_pRight->Balance(false);

		const_cast<T&>(m_value) = nextValue;
		Insert(thisValue);
	}
#endif // OPTIMIZE

#ifdef _DEBUG
	CheckBalanced();
#endif

#ifdef OPTIMIZE
	if (propagate && m_pParent)
		m_pParent->Balance();
#else // OPTIMIZE
	if (m_pParent)
		m_pParent->Balance();
#endif // OPTIMIZE
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

template <class T, class Compare>
T AVLTree<T, Compare>::Node::GetLowBound() const
{
	T	lowBound = m_value;

	if (m_pLeft)
	{
		T	leftLow = m_pLeft->GetLowBound();
		if (s_compare(leftLow, lowBound))
			lowBound = leftLow;
	}

	if (m_pRight)
	{
		T	rightLow = m_pRight->GetLowBound();
		if (s_compare(rightLow, lowBound))
			lowBound = rightLow;
	}

	return lowBound;
}

template <class T, class Compare>
T AVLTree<T, Compare>::Node::GetHighBound() const
{
	T	highBound = m_value;

	if (m_pLeft)
	{
		T	leftHigh = m_pLeft->GetHighBound();
		if (s_compare(leftHigh, highBound))
			highBound = leftHigh;
	}

	if (m_pRight)
	{
		T	rightHigh = m_pRight->GetHighBound();
		if (s_compare(rightHigh, highBound))
			highBound = rightHigh;
	}

	return highBound;
}

template <class T, class Compare>
inline void AVLTree<T, Compare>::Node::CheckBalanced() const
{
	assert(std::abs(GetHeight(m_pLeft) - GetHeight(m_pRight)) <= 1);
#ifdef OPTIMIZE
	assert(std::abs(GetSize(GetLeft()) - GetSize(GetRight())) <= 1);
#endif // OPTIMIZE

	if (m_pLeft)
	{
		assert(s_compare(m_pLeft->GetHighBound(), GetValue()));
		m_pLeft->CheckBalanced();
	}

	if (m_pRight)
	{
		assert(s_compare(GetValue(), m_pRight->GetLowBound()));
		m_pRight->CheckBalanced();
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ! _AVLTree_h_	