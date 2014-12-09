
// ==========================================
//
//		LINKED2.CPP
//		List-related objects.
//
//
// ==========================================

#include "/home/bmajoros/SgmlTalk/libsrc/linked2.H"


RTTI_DEFINE_ROOT(link_node)


// **************************************************
// *                                                *
// *                  Methods                       *
// *                                                *
// **************************************************

// ***************** linked_list methods **************

link_node *linked_list::GetLast()
	{
	if(!first) return 0;

	link_node *p=first;
	while(p->next) p=p->next;
    return p;
	}



int linked_list::NumElements(void) {
	link_node *ThisNode;
	int NumMembers=0;

	// Returns the total number of nodes contained in
	// this linked list.

	ThisNode = first;
	while(ThisNode) {
		NumMembers++;
		ThisNode=ThisNode->next;
		}

	return NumMembers;
	}













link_node *linked_list::previous(link_node *ThisNode)
	{
	link_node *TempPtr;

	TempPtr=first;
	if((TempPtr==ThisNode) || !first) return NULL;

	while((TempPtr) && (TempPtr->next!=ThisNode))
		TempPtr=TempPtr->next;
	return TempPtr;
	}

















void linked_list::set_seq(link_node *SetSeq) {
	seq=SetSeq;
	shadow=previous(seq);	// because client must call sequential()
									// before calling del_seq, so seq will
									// move ahead and then shadow will be two
									// behind

	}



linked_list::linked_list(void) : BaseList() { // constructor
	first=shadow=seq=NULL;
	}



linked_list::~linked_list(void) {	// destructor

	destroy();  // destroy all nodes in list
	}




void linked_list::destroy(void) {
	link_node *ThisNode, *NextNode;

	// delete every node in the list

	ThisNode=first;
	while(ThisNode) {
		NextNode=ThisNode->next;
		delete ThisNode;
		ThisNode=NextNode;
		}
	first=NULL;
	}







void linked_list::insert_here(link_node *new_node, link_node *here)
	// insert the new node before the 'here' node
	{
	link_node *prev;

	// must first find node directly before the 'here' node
	if(first==NULL)	// if empty list, then error
		{
		printf("\nERROR: Cannot insert_here() in an empty list\n");
		exit(1);
		}

	if(!here) {
		printf("\nERROR:  Second parameter in insert_here() is NULL\n");
		exit(1);
		}

	if(!new_node) {
		printf("\nERROR:  First parameter in insert_here() is NULL\n");
		exit(1);
		}

	// if inserting before the first node...
	if(here==first) {
		new_node->next=first;
		first=new_node;
		return;
		}

	prev=first;
	while((prev->next!=here)&&(prev->next!=NULL)) // find 'here'
		prev=prev->next;

	new_node->next=prev->next;
	prev->next=new_node;
	}





void linked_list::del_node(link_node *del)
	{
	// delete "del" from the list...del MUST
	// be in the list!!!

	LetGoOf(del); // remove from list
	delete del;	  // dispose of node
	}





void linked_list::list_insert(link_node *new_node)
	// insert new node before first element in list (if any)
	{
	new_node->next=first;
	first=new_node;
	}




void linked_list::list_append(link_node *new_node)
	// append new_node onto end of list
	{
	link_node *this_one;

	new_node->next=NULL;

	if(first==NULL) 		// this is the first entry into the list
		first=new_node;
	else {					// this is not the first entry...
		this_one=first;
		while(this_one->next!=NULL) // find end of list
			this_one=this_one->next;
		this_one->next=new_node;	// attach new node onto end of list
		}
	}



void linked_list::reset_seq(void)
	{
	// reset sequential() to first node

	seq=first;
	shadow=0;
	}




link_node *linked_list::sequential(void)
	{
	link_node *prev;

	// Returns the next node to be output -- allows you to traverse the
	// linked list without manually updating the pointer to the next node

	prev=seq; // this is the node we are going to return (may be NULL)
	if(seq)
		{
		if(!shadow)
			{
			if(seq!=first) shadow=first;
			}
		else if(shadow->next!=seq)
			shadow=shadow->next;

		seq=seq->next;
		}

	return prev;
	}





void linked_list::LetGoOf(link_node *del)
	{
	link_node *prev;

	// This causes the Victim to be removed from the linked list,
	// but the Victim is not deallocated...Thus, this linked_list
	// is relinquishing the Victim to the caller, so the caller
	// becomes the new owner of the Victim and is responsible
	// for ultimately deallocating the Victim when it is no
   // longer needed.

	if(!del) {
		printf("\nERROR:  Attempt to delete NULL\n");
		exit(1);
		}

	// if deleting the first node in the list...
	if(del==first)
		first=del->next;

	else {
		// first, must find the node directly before the victim
		prev=first;
		while(prev && (prev->next!=del)) prev=prev->next;
		if(!prev) {
			printf("\n\nERROR:  Attempt to delete a non-list element\n");
			exit(1);
			}
		prev->next=del->next;
		}
	}






void linked_list::del_seq(void)
	{
	// Completely rewritten in 12/95 (for garbage collector in Epsilon --
	// probably the first use of this method)

	// You muse use this method carefully; do as follows: Call reset_seq().
	// Then call sequential() at least once.  A call to del_seq() will
	// delete (in constant time) the node returned on the last call to
	// sequential().  Do NOT call del_seq() twice in a row without an
	// intervening call to sequential().

	// Precondition: reset_seq() has been called, followed by some nonzero
	//				 number of calls to sequential(), and no other
	//				 modifications to the list have been made since the
	//				 call to reset_seq(), except possibly some calls to
	//				 del_seq().  However, if del_seq() has been called,
	//				 then there has been a call to reset_seq() or
	//				 sequential() which occurred more recently than the call
	//				 to del_seq().  Finally, the most recent call to
	//				 sequential() did not return NULL.

	if(!shadow)
		{
		// Invariant: (#elements >=2) implies (seq points to second element).
		// Invariant: #elements >=1.
		// Invariant: (#elements==1) implies (seq is NULL)
		link_node *victim=first;
		first=first->next;
		delete victim;
		}
	else
		{
		// Invariant: shadow != NULL.
		// Invariant: shadow->next != NULL
		// Invariant: shadow->next->next == seq.
		link_node *victim=shadow->next;
		shadow->next=victim->next;
		delete victim;
		}
	}








// ***************** link_node methods *****************


link_node::link_node(void) {

	// constructor

	next=NULL;
	}




link_node::~link_node(void) {

	// destructor

	}







