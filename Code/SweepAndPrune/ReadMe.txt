Sweep-and-prune library 1.0
===========================

This is a quick and easy sweep-and-prune library going with the
following article: www.codercorner.com/SAP.pdf

I made it as easy as possible: no static lib, no DLL, no dependencies
on any external libraries. Just a few source files to copy in your
own project.

This is an array-based sweep-and-prune, which should be more
efficient than the previously released version in OPCODE 1.3 - which
was based on linked lists.

As usual, if all the objects are moving each frame (in particular at
high speed), you should instead use "box pruning". It's available here:
www.codercorner.com/BoxPruning.zip

"Multi-SAP", as described in the above article, will be released
later. I still have to rewrite that code in a releasable form.

------------

How to use the library should be quite obvious:

1) Add objects to the SAP using this function:

	udword	AddObject(void* object, uword guid, const AABB& box);
	
object	= pointer to your game/simulation object
guid	= index associated to this "object". Should be 0-based and 16-bit only.
box		= AABB around the "object"

You get a SAP handle in return;

You can add objects at any time.

------------

2) Update objects each frame using this function:

	bool	UpdateObject(udword handle, const AABB& box);
	
handle	= SAP handle returned from AddObject
box		= new AABB around the object

------------

3) Remove an object using this function:

	bool	RemoveObject(udword handle);

handle	= SAP handle returned from AddObject

You can remove objects at any time.

------------

4) Call incremental results using this function:

	udword	DumpPairs(SAP_CreatePair create_cb, SAP_DeletePair delete_cb, void* cb_user_data, ASAP_Pair** pairs=null);
	
create_cb		= callback called for each created pair (or null)
delete_cb		= callback called for each removed pair (or null)
cb_user_data	= user-data passed to callbacks (or null)
pairs			= returned array of active pairs (or null)

You can use this function in two ways:

a) using callbacks. Then "pairs" can be null, and you will get incremental
results via the "create_cb" and "delete_cb" callbacks.

b) without callbacks. Then "create_cb" and "delete_cb" should be null,
and "pairs" can be used instead. The function returns the total number of
pairs in the system, and the full list of pairs is returned in "pairs",
as a contiguous linear array.

------------


- Pierre Terdiman
February, 16, 2008
