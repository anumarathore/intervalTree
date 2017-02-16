#include "intervalTree.h"
#include <stdio.h>
#include <math.h>


const int MIN_INT=-MAX_INT;


#define ITMax(a, b) ( (a > b) ? a : b )

IntervalTreeNode::IntervalTreeNode(){
};

IntervalTreeNode::IntervalTreeNode(Interval * newInterval) 
  : storedInterval (newInterval) ,
    key(newInterval->GetLowPoint()), 
    high(newInterval->GetHighPoint()) , 
    maxHigh(high) {
};
IntervalTreeNode::~IntervalTreeNode(){
};
Interval::Interval(){
};

IntervalTree::IntervalTree()
{
  nil = new IntervalTreeNode;
  nil->left = nil->right = nil->parent = nil;
  nil->red = 0;
  nil->key = nil->high = nil->maxHigh = MIN_INT;
  nil->storedInterval = NULL;
  
  root = new IntervalTreeNode;
  root->parent = root->left = root->right = nil;
  root->key = root->high = root->maxHigh = MAX_INT;
  root->red=0;
  root->storedInterval = NULL;

}


void IntervalTree::LeftRotate(IntervalTreeNode* x) {
  IntervalTreeNode* y;

  y=x->right;
  x->right=y->left;

  if (y->left != nil) y->left->parent=x; 
  y->parent=x->parent;   

  if( x == x->parent->left) {
    x->parent->left=y;
  } else {
    x->parent->right=y;
  }
  y->left=x;
  x->parent=y;

  x->maxHigh=ITMax(x->left->maxHigh,ITMax(x->right->maxHigh,x->high));
  y->maxHigh=ITMax(x->maxHigh,ITMax(y->right->maxHigh,y->high));
#ifdef CHECK_INTERVAL_TREE_ASSUMPTIONS
  CheckAssumptions();
#elif defined(DEBUG_ASSERT)
  Assert(!nil->red,"nil not red in ITLeftRotate");
  Assert((nil->maxHigh=MIN_INT),
   "nil->maxHigh != MIN_INT in ITLeftRotate");
#endif
}


void IntervalTree::RightRotate(IntervalTreeNode* y) {
  IntervalTreeNode* x;


  x=y->left;
  y->left=x->right;

  if (nil != x->right)  x->right->parent=y; 
  x->parent=y->parent;
  if( y == y->parent->left) {
    y->parent->left=x;
  } else {
    y->parent->right=x;
  }
  x->right=y;
  y->parent=x;

  y->maxHigh=ITMax(y->left->maxHigh,ITMax(y->right->maxHigh,y->high));
  x->maxHigh=ITMax(x->left->maxHigh,ITMax(y->maxHigh,x->high));
  CheckAssumptions();

}

/*Inserts z into the tree as if it were a regular binary tree */


void IntervalTree::TreeInsertHelp(IntervalTreeNode* z) {
  
  IntervalTreeNode* x;
  IntervalTreeNode* y;
    
  z->left=z->right=nil;
  y=root;
  x=root->left;
  while( x != nil) {
    y=x;
    if ( x->key > z->key) { 
      x=x->left;
    } else { /* x->key <= z->key */
      x=x->right;
    }
  }
  z->parent=y;
  if ( (y == root) ||
       (y->key > z->key) ) { 
    y->left=z;
  } else {
    y->right=z;
  }

}


/* up to the root fixing the maxHigh fields after */
/*            an insertion or deletion */

void IntervalTree::FixUpMaxHigh(IntervalTreeNode * x) {
  while(x != root) {
    x->maxHigh=ITMax(x->high,ITMax(x->left->maxHigh,x->right->maxHigh));
    x=x->parent;
  }

  CheckAssumptions();
}


IntervalTreeNode * IntervalTree::Insert(Interval * newInterval)
{
  IntervalTreeNode * y;
  IntervalTreeNode * x;
  IntervalTreeNode * newNode;

  x = new IntervalTreeNode(newInterval);
  TreeInsertHelp(x);
  FixUpMaxHigh(x->parent);
  newNode = x;
  x->red=1;
  while(x->parent->red) { 
    if (x->parent == x->parent->parent->left) {
      y=x->parent->parent->right;
      if (y->red) {
  x->parent->red=0;
  y->red=0;
  x->parent->parent->red=1;
  x=x->parent->parent;
      } else {
  if (x == x->parent->right) {
    x=x->parent;
    LeftRotate(x);
  }
  x->parent->red=0;
  x->parent->parent->red=1;
  RightRotate(x->parent->parent);
      } 
    } else { 
      y=x->parent->parent->left;
      if (y->red) {
  x->parent->red=0;
  y->red=0;
  x->parent->parent->red=1;
  x=x->parent->parent;
      } else {
  if (x == x->parent->left) {
    x=x->parent;
    RightRotate(x);
  }
  x->parent->red=0;
  x->parent->parent->red=1;
  LeftRotate(x->parent->parent);
      } 
    }
  }
  root->left->red=0;
  return(newNode);
  CheckAssumptions();

}

/*  This function returns the successor of x or NULL if no 
            successor exists. */

  
IntervalTreeNode * IntervalTree::GetSuccessorOf(IntervalTreeNode * x) const
{ 
  IntervalTreeNode* y;

  if (nil != (y = x->right)) { /* assignment to y is intentional */
    while(y->left != nil) { /* returns the minium of the right subtree of x */
      y=y->left;
    }
    return(y);
  } else {
    y=x->parent;
    while(x == y->right) { /* sentinel used instead of checking for nil */
      x=y;
      y=y->parent;
    }
    if (y == root) return(nil);
    return(y);
  }
}



IntervalTreeNode * IntervalTree::GetPredecessorOf(IntervalTreeNode * x) const {
  IntervalTreeNode* y;

  if (nil != (y = x->left)) { 
    while(y->right != nil) { /* returns the maximum of the left subtree of x */
      y=y->right;
    }
    return(y);
  } else {
    y=x->parent;
    while(x == y->left) { 
      if (y == root) return(nil); 
      x=y;
      y=y->parent;
    }
    return(y);
  }
}

/*Performs rotations and changes colors to restore red-black */
/*             properties after a node is deleted */


void IntervalTree::DeleteFixUp(IntervalTreeNode* x) {
  IntervalTreeNode * w;
  IntervalTreeNode * rootLeft = root->left;

  while( (!x->red) && (rootLeft != x)) {
    if (x == x->parent->left) {
      w=x->parent->right;
      if (w->red) {
  w->red=0;
  x->parent->red=1;
  LeftRotate(x->parent);
  w=x->parent->right;
      }
      if ( (!w->right->red) && (!w->left->red) ) { 
  w->red=1;
  x=x->parent;
      } else {
  if (!w->right->red) {
    w->left->red=0;
    w->red=1;
    RightRotate(w);
    w=x->parent->right;
  }
  w->red=x->parent->red;
  x->parent->red=0;
  w->right->red=0;
  LeftRotate(x->parent);
  x=rootLeft; /* this is to exit while loop */
      }
    } else { 
      w=x->parent->left;
      if (w->red) {
  w->red=0;
  x->parent->red=1;
  RightRotate(x->parent);
  w=x->parent->left;
      }
      if ( (!w->right->red) && (!w->left->red) ) { 
  w->red=1;
  x=x->parent;
      } else {
  if (!w->left->red) {
    w->right->red=0;
    w->red=1;
    LeftRotate(w);
    w=x->parent->left;
  }
  w->red=x->parent->red;
  x->parent->red=0;
  w->left->red=0;
  RightRotate(x->parent);
  x=rootLeft; /* this is to exit while loop */
      }
    }
  }
  x->red=0;

  CheckAssumptions();

}


/* Deletes z from tree and but don't call destructor */
/*             Then calls FixUpMaxHigh to fix maxHigh fields then calls */
/*             ITDeleteFixUp to restore red-black properties */


Interval * IntervalTree::DeleteNode(IntervalTreeNode * z){
  IntervalTreeNode* y;
  IntervalTreeNode* x;
  Interval * returnValue = z->storedInterval;

  y= ((z->left == nil) || (z->right == nil)) ? z : GetSuccessorOf(z);
  x= (y->left == nil) ? y->right : y->left;
  if (root == (x->parent = y->parent)) { 
    root->left=x;
  } else {
    if (y == y->parent->left) {
      y->parent->left=x;
    } else {
      y->parent->right=x;
    }
  }
  if (y != z) { 

    //y is the node to splice out and x is its child 
  
    y->maxHigh = MIN_INT;
    y->left=z->left;
    y->right=z->right;
    y->parent=z->parent;
    z->left->parent=z->right->parent=y;
    if (z == z->parent->left) {
      z->parent->left=y; 
    } else {
      z->parent->right=y;
    }
    FixUpMaxHigh(x->parent); 
    if (!(y->red)) {
      y->red = z->red;
      DeleteFixUp(x);
    } else
      y->red = z->red; 
    delete z;
    CheckAssumptions();
  } else {
    FixUpMaxHigh(x->parent);
    if (!(y->red)) DeleteFixUp(x);
    delete y;
    CheckAssumptions();
  }
  return returnValue;
}


int Overlap(int a1, int a2, int b1, int b2) {
  if (a1 <= b1) {
    return( (b1 <= a2) );
  } else {
    return( (a1 <= b2) );
  }
}




int IntervalTree::CheckMaxHighFieldsHelper(IntervalTreeNode * y, 
            const int currentHigh,
            int match) const
{
  if (y != nil) {
    match = CheckMaxHighFieldsHelper(y->left,currentHigh,match) ?
      1 : match;
    VERIFY(y->high <= currentHigh);
    if (y->high == currentHigh)
      match = 1;
    match = CheckMaxHighFieldsHelper(y->right,currentHigh,match) ?
      1 : match;
  }
  return match;
}

void IntervalTree::CheckMaxHighFields(IntervalTreeNode * x) const {
  if (x != nil) {
    CheckMaxHighFields(x->left);
    if(!(CheckMaxHighFieldsHelper(x,x->maxHigh,0) > 0)) {
      ExitProgramMacro("error found in CheckMaxHighFields.\n");
    }
    CheckMaxHighFields(x->right);
  }
}

void IntervalTree::CheckAssumptions() const {
 VERIFY(nil->key == MIN_INT);
 VERIFY(nil->high == MIN_INT);
 VERIFY(nil->maxHigh == MIN_INT);
 VERIFY(root->key == MAX_INT);
 VERIFY(root->high == MAX_INT);
 VERIFY(root->maxHigh == MAX_INT);
 VERIFY(nil->storedInterval == NULL);
 VERIFY(root->storedInterval == NULL);
 VERIFY(nil->red == 0);
 VERIFY(root->red == 0);
 CheckMaxHighFields(root->left);
}
 


