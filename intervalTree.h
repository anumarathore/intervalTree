#include<math.h>
#include<limits.h>
#include<iostream.h>

#ifndef MAX_INT
#define MAX_INT INT_MAX 
#endif


class Interval {
public:
  Interval();
  virtual ~Interval();
  virtual int GetLowPoint() const = 0;
  virtual int GetHighPoint() const = 0;
  };

class IntervalTreeNode {
  friend class IntervalTree;
public:
  void Print(IntervalTreeNode*,
	     IntervalTreeNode*) const;
  IntervalTreeNode();
  IntervalTreeNode(Interval *);
  ~IntervalTreeNode();
protected:
  Interval * storedInterval;
  int key;
  int high;
  int maxHigh;
  int red; //if red=0 then the node is black 
  IntervalTreeNode * left;
  IntervalTreeNode * right;
  IntervalTreeNode * parent;
};


class IntervalTree {
public:
  IntervalTree();
  Interval * DeleteNode(IntervalTreeNode *);
  IntervalTreeNode * Insert(Interval *);
  IntervalTreeNode * GetPredecessorOf(IntervalTreeNode *) const;
  IntervalTreeNode * GetSuccessorOf(IntervalTreeNode *) const;
  void CheckAssumptions() const;
protected:
  // A sentinel is used for root and for nil.  
  IntervalTreeNode * root;
  IntervalTreeNode * nil;
  void LeftRotate(IntervalTreeNode *);
  void RightRotate(IntervalTreeNode *);
  void TreeInsertHelp(IntervalTreeNode *);
  void FixUpMaxHigh(IntervalTreeNode *);
  void DeleteFixUp(IntervalTreeNode *);
  void CheckMaxHighFields(IntervalTreeNode *) const;
  int CheckMaxHighFieldsHelper(IntervalTreeNode * y, 
			const int currentHigh,
			int match) const;
private:
  unsigned int currentParent;
  };


#endif



