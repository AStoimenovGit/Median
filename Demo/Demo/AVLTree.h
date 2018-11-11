
#ifndef _AVLTree_h_
#define _AVLTree_h_

#undef min
#undef max
#include <algorithm>

#include "Median.h"

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

		void	Detach();
		void	AttachNode(Node*& pChild, Node* pNode);
		void	AttachLeftNode(Node* pNode);
		void	AttachRightNode(Node* pNode);

		void	RotateLeft();
		void	RotateRight();

		void	Balance();

	#ifdef _DEBUG
		T		GetLowBound() const;
		T		GetHighBound() const;
		void	CheckBalanced() const;
	#endif

	private:
		const T	m_value;
		int		m_height;
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
/*virtual*/ bool AVLTree<T, Compare>::GetMedian(T& median) const
{
	if (!BaseClass::m_size)
		return false;

	assert(m_pRoot);

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
	else if(pPrevNode)
	{
		median = (pPrevNode->GetValue() + pNode->GetValue()) / 2;
	}
	else
	{
		median = pNode->GetValue();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
inline AVLTree<T, Compare>::Node::Node(const T& value)
	: m_value(value)
	, m_height()
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

template <class T, class Compare>
/*static*/ inline int AVLTree<T, Compare>::Node::GetHeight(const Node* pNode)
{
	if (pNode)
		return pNode->GetHeight();
	else
		return -1;
}

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
	else if(this == m_pParent->m_pRight)
	{
		m_pParent->m_pRight = nullptr;
	}

	if (m_pParent)
		m_pParent->UpdateHeights();

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
	if(pLeft)
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
void AVLTree<T, Compare>::Node::Balance()
{
	auto	leftHeight = m_pLeft ? m_pLeft->m_height : -1;
	auto	rightHeight = m_pRight ? m_pRight->m_height : -1;

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

#ifdef _DEBUG
	CheckBalanced();
#endif

	if (m_pParent)
		m_pParent->Balance();
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
	auto	leftHeight = m_pLeft ? m_pLeft->m_height : -1;
	auto	rightHeight = m_pRight ? m_pRight->m_height : -1;

	// assert(std::abs(leftHeight - rightHeight) <= 1);

	if (m_pLeft)
	{
		assert(s_compare(m_pLeft->GetHighBound(), GetValue()));
		m_pLeft->CheckBalanced();
	}

	if(m_pRight)
	{
		assert(s_compare(GetValue(), m_pRight->GetLowBound()));
		m_pRight->CheckBalanced();
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ! _AVLTree_h_