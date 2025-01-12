/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright 2002 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FREEBSD_IRBT_H
#define _FREEBSD_IRBT_H

#include <stddef.h>
// Copy from freebsd, files are as follows:
// sys/sys/tree.h

/* Macros that define a rank-balanced tree */
#define RB_HEAD(name, type)						\
struct name {								\
	struct type *rbh_root; /* root of the tree */			\
}

#define RB_INITIALIZER(root)						\
	{ NULL }

#define RB_INIT(root) do {						\
	(root)->rbh_root = NULL;					\
} while (/*CONSTCOND*/ 0)

#define RB_ENTRY(type)							\
struct {								\
	struct type *rbe_link[3];					\
}

/*
 * With the expectation that any object of struct type has an
 * address that is a multiple of 4, and that therefore the
 * 2 least significant bits of a pointer to struct type are
 * always zero, this implementation sets those bits to indicate
 * that the left or right child of the tree node is "red".
 */
#define _RB_LINK(elm, dir, field)	(elm)->field.rbe_link[dir]
#define _RB_UP(elm, field)		_RB_LINK(elm, 0, field)
#define _RB_L				((size_t)1)
#define _RB_R				((size_t)2)
#define _RB_LR				((size_t)3)
#define _RB_BITS(elm)			(*(size_t *)&elm)
#define _RB_BITSUP(elm, field)		_RB_BITS(_RB_UP(elm, field))
#define _RB_PTR(elm)			(__typeof(elm))			\
					((size_t)elm & ~_RB_LR)

#define RB_PARENT(elm, field)		_RB_PTR(_RB_UP(elm, field))
#define RB_LEFT(elm, field)		_RB_LINK(elm, _RB_L, field)
#define RB_RIGHT(elm, field)		_RB_LINK(elm, _RB_R, field)
#define RB_ROOT(head)			(head)->rbh_root
#define RB_EMPTY(head)			(RB_ROOT(head) == NULL)

#define RB_SET_PARENT(dst, src, field) do {				\
	_RB_BITSUP(dst, field) = (size_t)src |			\
	    (_RB_BITSUP(dst, field) & _RB_LR);				\
} while (/*CONSTCOND*/ 0)

#define RB_SET(elm, parent, field) do {					\
	_RB_UP(elm, field) = parent;					\
	RB_LEFT(elm, field) = RB_RIGHT(elm, field) = NULL;		\
} while (/*CONSTCOND*/ 0)

/*
 * Either RB_AUGMENT or RB_AUGMENT_CHECK is invoked in a loop at the root of
 * every modified subtree, from the bottom up to the root, to update augmented
 * node data.  RB_AUGMENT_CHECK returns true only when the update changes the
 * node data, so that updating can be stopped short of the root when it returns
 * false.
 */
#ifndef RB_AUGMENT_CHECK
#ifndef RB_AUGMENT
#define RB_AUGMENT_CHECK(x) 0
#else
#define RB_AUGMENT_CHECK(x) (RB_AUGMENT(x), 1)
#endif
#endif

#define RB_UPDATE_AUGMENT(elm, field) do {				\
	__typeof(elm) rb_update_tmp = (elm);				\
	while (RB_AUGMENT_CHECK(rb_update_tmp) &&			\
	    (rb_update_tmp = RB_PARENT(rb_update_tmp, field)) != NULL)	\
		;							\
} while (0)

#define RB_SWAP_CHILD(head, par, out, in, field) do {			\
	if (par == NULL)						\
		RB_ROOT(head) = (in);					\
	else if ((out) == RB_LEFT(par, field))				\
		RB_LEFT(par, field) = (in);				\
	else								\
		RB_RIGHT(par, field) = (in);				\
} while (/*CONSTCOND*/ 0)

/*
 * RB_ROTATE macro partially restructures the tree to improve balance. In the
 * case when dir is _RB_L, tmp is a right child of elm.  After rotation, elm
 * is a left child of tmp, and the subtree that represented the items between
 * them, which formerly hung to the left of tmp now hangs to the right of elm.
 * The parent-child relationship between elm and its former parent is not
 * changed; where this macro once updated those fields, that is now left to the
 * caller of RB_ROTATE to clean up, so that a pair of rotations does not twice
 * update the same pair of pointer fields with distinct values.
 */
#define RB_ROTATE(elm, tmp, dir, field) do {				\
	if ((_RB_LINK(elm, dir ^ _RB_LR, field) =			\
	    _RB_LINK(tmp, dir, field)) != NULL)				\
		RB_SET_PARENT(_RB_LINK(tmp, dir, field), elm, field);	\
	_RB_LINK(tmp, dir, field) = (elm);				\
	RB_SET_PARENT(elm, tmp, field);					\
} while (/*CONSTCOND*/ 0)

/* Generates prototypes and inline functions */
#define	RB_PROTOTYPE(name, type, field, cmp)				\
	RB_PROTOTYPE_INTERNAL(name, type, field, cmp,)
#define	RB_PROTOTYPE_STATIC(name, type, field, cmp)			\
	RB_PROTOTYPE_INTERNAL(name, type, field, cmp, __unused static)
#define RB_PROTOTYPE_INTERNAL(name, type, field, cmp, attr)		\
	RB_PROTOTYPE_RANK(name, type, attr)				\
	RB_PROTOTYPE_INSERT_COLOR(name, type, attr);			\
	RB_PROTOTYPE_REMOVE_COLOR(name, type, attr);			\
	RB_PROTOTYPE_INSERT_FINISH(name, type, attr);			\
	RB_PROTOTYPE_INSERT(name, type, attr);				\
	RB_PROTOTYPE_REMOVE(name, type, attr);				\
	RB_PROTOTYPE_FIND(name, type, attr);				\
	RB_PROTOTYPE_NFIND(name, type, attr);				\
	RB_PROTOTYPE_NEXT(name, type, attr);				\
	RB_PROTOTYPE_INSERT_NEXT(name, type, attr);			\
	RB_PROTOTYPE_PREV(name, type, attr);				\
	RB_PROTOTYPE_INSERT_PREV(name, type, attr);			\
	RB_PROTOTYPE_MINMAX(name, type, attr);				\
	RB_PROTOTYPE_REINSERT(name, type, attr);
#ifdef _RB_DIAGNOSTIC
#define RB_PROTOTYPE_RANK(name, type, attr)				\
	attr int name##_RB_RANK(struct type *);
#else
#define RB_PROTOTYPE_RANK(name, type, attr)
#endif
#define RB_PROTOTYPE_INSERT_COLOR(name, type, attr)			\
	attr struct type *name##_RB_INSERT_COLOR(struct name *,		\
	    struct type *, struct type *)
#define RB_PROTOTYPE_REMOVE_COLOR(name, type, attr)			\
	attr struct type *name##_RB_REMOVE_COLOR(struct name *,		\
	    struct type *, struct type *)
#define RB_PROTOTYPE_REMOVE(name, type, attr)				\
	attr struct type *name##_RB_REMOVE(struct name *, struct type *)
#define RB_PROTOTYPE_INSERT_FINISH(name, type, attr)			\
	attr struct type *name##_RB_INSERT_FINISH(struct name *,	\
	    struct type *, struct type **, struct type *)
#define RB_PROTOTYPE_INSERT(name, type, attr)				\
	attr struct type *name##_RB_INSERT(struct name *, struct type *)
#define RB_PROTOTYPE_FIND(name, type, attr)				\
	attr struct type *name##_RB_FIND(struct name *, struct type *)
#define RB_PROTOTYPE_NFIND(name, type, attr)				\
	attr struct type *name##_RB_NFIND(struct name *, struct type *)
#define RB_PROTOTYPE_NEXT(name, type, attr)				\
	attr struct type *name##_RB_NEXT(struct type *)
#define RB_PROTOTYPE_INSERT_NEXT(name, type, attr)			\
	attr struct type *name##_RB_INSERT_NEXT(struct name *,		\
	    struct type *, struct type *)
#define RB_PROTOTYPE_PREV(name, type, attr)				\
	attr struct type *name##_RB_PREV(struct type *)
#define RB_PROTOTYPE_INSERT_PREV(name, type, attr)			\
	attr struct type *name##_RB_INSERT_PREV(struct name *,		\
	    struct type *, struct type *)
#define RB_PROTOTYPE_MINMAX(name, type, attr)				\
	attr struct type *name##_RB_MINMAX(struct name *, int)
#define RB_PROTOTYPE_REINSERT(name, type, attr)			\
	attr struct type *name##_RB_REINSERT(struct name *, struct type *)

/* Main rb operation.
 * Moves node close to the key of elm to top
 */
#define	RB_GENERATE(name, type, field, cmp)				\
	RB_GENERATE_INTERNAL(name, type, field, cmp,)
#define	RB_GENERATE_STATIC(name, type, field, cmp)			\
	RB_GENERATE_INTERNAL(name, type, field, cmp, __unused static)
#define RB_GENERATE_INTERNAL(name, type, field, cmp, attr)		\
	RB_GENERATE_RANK(name, type, field, attr)			\
	RB_GENERATE_INSERT_COLOR(name, type, field, attr)		\
	RB_GENERATE_REMOVE_COLOR(name, type, field, attr)		\
	RB_GENERATE_INSERT_FINISH(name, type, field, attr)		\
	RB_GENERATE_INSERT(name, type, field, cmp, attr)		\
	RB_GENERATE_REMOVE(name, type, field, attr)			\
	RB_GENERATE_FIND(name, type, field, cmp, attr)			\
	RB_GENERATE_NFIND(name, type, field, cmp, attr)			\
	RB_GENERATE_NEXT(name, type, field, attr)			\
	RB_GENERATE_INSERT_NEXT(name, type, field, cmp, attr)		\
	RB_GENERATE_PREV(name, type, field, attr)			\
	RB_GENERATE_INSERT_PREV(name, type, field, cmp, attr)		\
	RB_GENERATE_MINMAX(name, type, field, attr)			\
	RB_GENERATE_REINSERT(name, type, field, cmp, attr)

#ifdef _RB_DIAGNOSTIC
#ifndef RB_AUGMENT
#define _RB_AUGMENT_VERIFY(x) RB_AUGMENT_CHECK(x)
#else
#define _RB_AUGMENT_VERIFY(x) 0
#endif
#define RB_GENERATE_RANK(name, type, field, attr)			\
/*									\
 * Return the rank of the subtree rooted at elm, or -1 if the subtree	\
 * is not rank-balanced, or has inconsistent augmentation data.
 */									\
attr int								\
name##_RB_RANK(struct type *elm)					\
{									\
	struct type *left, *right, *up;					\
	int left_rank, right_rank;					\
									\
	if (elm == NULL)						\
		return (0);						\
	up = _RB_UP(elm, field);					\
	left = RB_LEFT(elm, field);					\
	left_rank = ((_RB_BITS(up) & _RB_L) ? 2 : 1) +			\
	    name##_RB_RANK(left);					\
	right = RB_RIGHT(elm, field);					\
	right_rank = ((_RB_BITS(up) & _RB_R) ? 2 : 1) +			\
	    name##_RB_RANK(right);					\
	if (left_rank != right_rank ||					\
	    (left_rank == 2 && left == NULL && right == NULL) ||	\
	    _RB_AUGMENT_VERIFY(elm))					\
		return (-1);						\
	return (left_rank);						\
}
#else
#define RB_GENERATE_RANK(name, type, field, attr)
#endif

#define RB_GENERATE_INSERT_COLOR(name, type, field, attr)		\
attr struct type *							\
name##_RB_INSERT_COLOR(struct name *head,				\
    struct type *parent, struct type *elm)				\
{									\
	/*								\
	 * Initially, elm is a leaf.  Either its parent was previously	\
	 * a leaf, with two black null children, or an interior node	\
	 * with a black non-null child and a red null child. The        \
	 * balance criterion "the rank of any leaf is 1" precludes the  \
	 * possibility of two red null children for the initial parent. \
	 * So the first loop iteration cannot lead to accessing an      \
	 * uninitialized 'child', and a later iteration can only happen \
	 * when a value has been assigned to 'child' in the previous    \
	 * one.								\
	 */								\
	struct type *child, *child_up, *gpar;				\
	size_t elmdir, sibdir;					\
									\
	do {								\
		/* the rank of the tree rooted at elm grew */		\
		gpar = _RB_UP(parent, field);				\
		elmdir = RB_RIGHT(parent, field) == elm ? _RB_R : _RB_L; \
		if (_RB_BITS(gpar) & elmdir) {				\
			/* shorten the parent-elm edge to rebalance */	\
			_RB_BITSUP(parent, field) ^= elmdir;		\
			return (NULL);					\
		}							\
		sibdir = elmdir ^ _RB_LR;				\
		/* the other edge must change length */			\
		_RB_BITSUP(parent, field) ^= sibdir;			\
		if ((_RB_BITS(gpar) & _RB_LR) == 0) {			\
			/* both edges now short, retry from parent */	\
			child = elm;					\
			elm = parent;					\
			continue;					\
		}							\
		_RB_UP(parent, field) = gpar = _RB_PTR(gpar);		\
		if (_RB_BITSUP(elm, field) & elmdir) {			\
			/*						\
			 * Exactly one of the edges descending from elm \
			 * is long. The long one is in the same		\
			 * direction as the edge from parent to elm,	\
			 * so change that by rotation.  The edge from	\
			 * parent to z was shortened above.  Shorten	\
			 * the long edge down from elm, and adjust	\
			 * other edge lengths based on the downward	\
			 * edges from 'child'.				\
			 *						\
			 *	     par		 par		\
			 *	    /	\		/   \		\
			 *	  elm	 z	       /     z		\
			 *	 /  \		     child		\
			 *	/  child	     /	 \		\
			 *     /   /  \		   elm	  \		\
			 *    w	  /    \	  /   \    y		\
			 *	 x      y	 w     \		\
			 *				x		\
			 */						\
			RB_ROTATE(elm, child, elmdir, field);		\
			child_up = _RB_UP(child, field);		\
			if (_RB_BITS(child_up) & sibdir)		\
				_RB_BITSUP(parent, field) ^= elmdir;	\
			if (_RB_BITS(child_up) & elmdir)		\
				_RB_BITSUP(elm, field) ^= _RB_LR;	\
			else						\
				_RB_BITSUP(elm, field) ^= elmdir;	\
			/* if child is a leaf, don't augment elm,	\
			 * since it is restored to be a leaf again. */	\
			if ((_RB_BITS(child_up) & _RB_LR) == 0)		\
				elm = child;				\
		} else							\
			child = elm;					\
									\
		/*							\
		 * The long edge descending from 'child' points back	\
		 * in the direction of 'parent'. Rotate to make		\
		 * 'parent' a child of 'child', then make both edges	\
		 * of 'child' short to rebalance.			\
		 *							\
		 *	     par		 child			\
		 *	    /	\		/     \			\
		 *	   /	 z	       x       par		\
		 *	child			      /	  \		\
		 *	 /  \			     /	   z		\
		 *	x    \			    y			\
		 *	      y						\
		 */							\
		RB_ROTATE(parent, child, sibdir, field);		\
		_RB_UP(child, field) = gpar;				\
		RB_SWAP_CHILD(head, gpar, parent, child, field);	\
		/*							\
		 * Elements rotated down have new, smaller subtrees,	\
		 * so update augmentation for them.			\
		 */							\
		if (elm != child)					\
			(void)RB_AUGMENT_CHECK(elm);			\
		(void)RB_AUGMENT_CHECK(parent);				\
		return (child);						\
	} while ((parent = gpar) != NULL);				\
	return (NULL);							\
}

#ifndef RB_STRICT_HST
/*
 * In REMOVE_COLOR, the HST paper, in figure 3, in the single-rotate case, has
 * 'parent' with one higher rank, and then reduces its rank if 'parent' has
 * become a leaf.  This implementation always has the parent in its new position
 * with lower rank, to avoid the leaf check.  Define RB_STRICT_HST to 1 to get
 * the behavior that HST describes.
 */
#define RB_STRICT_HST 0
#endif

#define RB_GENERATE_REMOVE_COLOR(name, type, field, attr)		\
attr struct type *							\
name##_RB_REMOVE_COLOR(struct name *head,				\
    struct type *parent, struct type *elm)				\
{									\
	struct type *gpar, *sib, *up;					\
	size_t elmdir, sibdir;					\
									\
	if (RB_RIGHT(parent, field) == elm &&				\
	    RB_LEFT(parent, field) == elm) {				\
		/* Deleting a leaf that is an only-child creates a	\
		 * rank-2 leaf. Demote that leaf. */			\
		_RB_UP(parent, field) = _RB_PTR(_RB_UP(parent, field));	\
		elm = parent;						\
		if ((parent = _RB_UP(elm, field)) == NULL)		\
			return (NULL);					\
	}								\
	do {								\
		/* the rank of the tree rooted at elm shrank */		\
		gpar = _RB_UP(parent, field);				\
		elmdir = RB_RIGHT(parent, field) == elm ? _RB_R : _RB_L; \
		_RB_BITS(gpar) ^= elmdir;				\
		if (_RB_BITS(gpar) & elmdir) {				\
			/* lengthen the parent-elm edge to rebalance */	\
			_RB_UP(parent, field) = gpar;			\
			return (NULL);					\
		}							\
		if (_RB_BITS(gpar) & _RB_LR) {				\
			/* shorten other edge, retry from parent */	\
			_RB_BITS(gpar) ^= _RB_LR;			\
			_RB_UP(parent, field) = gpar;			\
			gpar = _RB_PTR(gpar);				\
			continue;					\
		}							\
		sibdir = elmdir ^ _RB_LR;				\
		sib = _RB_LINK(parent, sibdir, field);			\
		up = _RB_UP(sib, field);				\
		_RB_BITS(up) ^= _RB_LR;					\
		if ((_RB_BITS(up) & _RB_LR) == 0) {			\
			/* shorten edges descending from sib, retry */	\
			_RB_UP(sib, field) = up;			\
			continue;					\
		}							\
		if ((_RB_BITS(up) & sibdir) == 0) {			\
			/*						\
			 * The edge descending from 'sib' away from	\
			 * 'parent' is long.  The short edge descending	\
			 * from 'sib' toward 'parent' points to 'elm*'	\
			 * Rotate to make 'sib' a child of 'elm*'	\
			 * then adjust the lengths of the edges		\
			 * descending from 'sib' and 'elm*'.		\
			 *						\
			 *	     par		 par		\
			 *	    /	\		/   \		\
			 *	   /	sib	      elm    \		\
			 *	  /	/ \	            elm*	\
			 *	elm   elm* \	            /  \	\
			 *	      /	\   \		   /    \	\
			 *	     /   \   z		  /      \	\
			 *	    x	  y		 x      sib	\
			 *				        /  \	\
			 *				       /    z	\
			 *				      y		\
			 */						\
			elm = _RB_LINK(sib, elmdir, field);		\
			/* elm is a 1-child.  First rotate at elm. */	\
			RB_ROTATE(sib, elm, sibdir, field);		\
			up = _RB_UP(elm, field);			\
			_RB_BITSUP(parent, field) ^=			\
			    (_RB_BITS(up) & elmdir) ? _RB_LR : elmdir;	\
			_RB_BITSUP(sib, field) ^=			\
			    (_RB_BITS(up) & sibdir) ? _RB_LR : sibdir;	\
			_RB_BITSUP(elm, field) |= _RB_LR;		\
		} else {						\
			if ((_RB_BITS(up) & elmdir) == 0 &&		\
			    RB_STRICT_HST && elm != NULL) {		\
				/* if parent does not become a leaf,	\
				   do not demote parent yet. */		\
				_RB_BITSUP(parent, field) ^= sibdir;	\
				_RB_BITSUP(sib, field) ^= _RB_LR;	\
			} else if ((_RB_BITS(up) & elmdir) == 0) {	\
				/* demote parent. */			\
				_RB_BITSUP(parent, field) ^= elmdir;	\
				_RB_BITSUP(sib, field) ^= sibdir;	\
			} else						\
				_RB_BITSUP(sib, field) ^= sibdir;	\
			elm = sib;					\
		}							\
									\
		/*							\
		 * The edge descending from 'elm' away from 'parent'	\
		 * is short.  Rotate to make 'parent' a child of 'elm', \
		 * then lengthen the short edges descending from	\
		 * 'parent' and 'elm' to rebalance.			\
		 *							\
		 *	     par		 elm			\
		 *	    /	\		/   \			\
		 *	   e	 \	       /     \			\
		 *		 elm	      /	      \			\
		 *		/  \	    par	       s		\
		 *	       /    \	   /   \			\
		 *	      /	     \	  e	\			\
		 *	     x	      s		 x			\
		 */							\
		RB_ROTATE(parent, elm, elmdir, field);			\
		RB_SET_PARENT(elm, gpar, field);			\
		RB_SWAP_CHILD(head, gpar, parent, elm, field);		\
		/*							\
		 * An element rotated down, but not into the search	\
		 * path has a new, smaller subtree, so update		\
		 * augmentation for it.					\
		 */							\
		if (sib != elm)						\
			(void)RB_AUGMENT_CHECK(sib);			\
		return (parent);					\
	} while (elm = parent, (parent = gpar) != NULL);		\
	return (NULL);							\
}

#define _RB_AUGMENT_WALK(elm, match, field)				\
do {									\
	if (match == elm)						\
		match = NULL;						\
} while (RB_AUGMENT_CHECK(elm) &&					\
    (elm = RB_PARENT(elm, field)) != NULL)

#define RB_GENERATE_REMOVE(name, type, field, attr)			\
attr struct type *							\
name##_RB_REMOVE(struct name *head, struct type *out)			\
{									\
	struct type *child, *in, *opar, *parent;			\
									\
	child = RB_LEFT(out, field);					\
	in = RB_RIGHT(out, field);					\
	opar = _RB_UP(out, field);					\
	if (in == NULL || child == NULL) {				\
		in = child = (in == NULL ? child : in);			\
		parent = opar = _RB_PTR(opar);				\
	} else {							\
		parent = in;						\
		while (RB_LEFT(in, field))				\
			in = RB_LEFT(in, field);			\
		RB_SET_PARENT(child, in, field);			\
		RB_LEFT(in, field) = child;				\
		child = RB_RIGHT(in, field);				\
		if (parent != in) {					\
			RB_SET_PARENT(parent, in, field);		\
			RB_RIGHT(in, field) = parent;			\
			parent = RB_PARENT(in, field);			\
			RB_LEFT(parent, field) = child;			\
		}							\
		_RB_UP(in, field) = opar;				\
		opar = _RB_PTR(opar);					\
	}								\
	RB_SWAP_CHILD(head, opar, out, in, field);			\
	if (child != NULL)						\
		_RB_UP(child, field) = parent;				\
	if (parent != NULL) {						\
		opar = name##_RB_REMOVE_COLOR(head, parent, child);	\
		/* if rotation has made 'parent' the root of the same	\
		 * subtree as before, don't re-augment it. */		\
		if (parent == in && RB_LEFT(parent, field) == NULL) {	\
			opar = NULL;					\
			parent = RB_PARENT(parent, field);		\
		}							\
		_RB_AUGMENT_WALK(parent, opar, field);			\
		if (opar != NULL) {					\
			/*						\
			 * Elements rotated into the search path have	\
			 * changed subtrees, so update augmentation for	\
			 * them if AUGMENT_WALK didn't.			\
			 */						\
			(void)RB_AUGMENT_CHECK(opar);			\
			(void)RB_AUGMENT_CHECK(RB_PARENT(opar, field));	\
		}							\
	}								\
	return (out);							\
}

#define RB_GENERATE_INSERT_FINISH(name, type, field, attr)		\
/* Inserts a node into the RB tree */					\
attr struct type *							\
name##_RB_INSERT_FINISH(struct name *head, struct type *parent,		\
    struct type **pptr, struct type *elm)				\
{									\
	struct type *tmp = NULL;					\
									\
	RB_SET(elm, parent, field);					\
	*pptr = elm;							\
	if (parent != NULL)						\
		tmp = name##_RB_INSERT_COLOR(head, parent, elm);	\
	_RB_AUGMENT_WALK(elm, tmp, field);				\
	if (tmp != NULL)						\
		/*							\
		 * An element rotated into the search path has a	\
		 * changed subtree, so update augmentation for it if	\
		 * AUGMENT_WALK didn't.					\
		 */							\
		(void)RB_AUGMENT_CHECK(tmp);				\
	return (NULL);							\
}

#define RB_GENERATE_INSERT(name, type, field, cmp, attr)		\
/* Inserts a node into the RB tree */					\
attr struct type *							\
name##_RB_INSERT(struct name *head, struct type *elm)			\
{									\
	struct type *tmp;						\
	struct type **tmpp = &RB_ROOT(head);				\
	struct type *parent = NULL;					\
									\
	while ((tmp = *tmpp) != NULL) {					\
		parent = tmp;						\
		__typeof(cmp(NULL, NULL)) comp = (cmp)(elm, parent);	\
		if (comp < 0)						\
			tmpp = &RB_LEFT(parent, field);			\
		else if (comp > 0)					\
			tmpp = &RB_RIGHT(parent, field);		\
		else							\
			return (parent);				\
	}								\
	return (name##_RB_INSERT_FINISH(head, parent, tmpp, elm));	\
}

#define RB_GENERATE_FIND(name, type, field, cmp, attr)			\
/* Finds the node with the same key as elm */				\
attr struct type *							\
name##_RB_FIND(struct name *head, struct type *elm)			\
{									\
	struct type *tmp = RB_ROOT(head);				\
	__typeof(cmp(NULL, NULL)) comp;					\
	while (tmp) {							\
		comp = cmp(elm, tmp);					\
		if (comp < 0)						\
			tmp = RB_LEFT(tmp, field);			\
		else if (comp > 0)					\
			tmp = RB_RIGHT(tmp, field);			\
		else							\
			return (tmp);					\
	}								\
	return (NULL);							\
}

#define RB_GENERATE_NFIND(name, type, field, cmp, attr)			\
/* Finds the first node greater than or equal to the search key */	\
attr struct type *							\
name##_RB_NFIND(struct name *head, struct type *elm)			\
{									\
	struct type *tmp = RB_ROOT(head);				\
	struct type *res = NULL;					\
	__typeof(cmp(NULL, NULL)) comp;					\
	while (tmp) {							\
		comp = cmp(elm, tmp);					\
		if (comp < 0) {						\
			res = tmp;					\
			tmp = RB_LEFT(tmp, field);			\
		}							\
		else if (comp > 0)					\
			tmp = RB_RIGHT(tmp, field);			\
		else							\
			return (tmp);					\
	}								\
	return (res);							\
}

#define RB_GENERATE_NEXT(name, type, field, attr)			\
/* ARGSUSED */								\
attr struct type *							\
name##_RB_NEXT(struct type *elm)					\
{									\
	if (RB_RIGHT(elm, field)) {					\
		elm = RB_RIGHT(elm, field);				\
		while (RB_LEFT(elm, field))				\
			elm = RB_LEFT(elm, field);			\
	} else {							\
		while (RB_PARENT(elm, field) &&				\
		    (elm == RB_RIGHT(RB_PARENT(elm, field), field)))	\
			elm = RB_PARENT(elm, field);			\
		elm = RB_PARENT(elm, field);				\
	}								\
	return (elm);							\
}

#if defined(_KERNEL) && defined(DIAGNOSTIC)
#define _RB_ORDER_CHECK(cmp, lo, hi) do {				\
	KASSERT((cmp)(lo, hi) < 0, ("out of order insertion"));		\
} while (0)
#else
#define _RB_ORDER_CHECK(cmp, lo, hi) do {} while (0)
#endif

#define RB_GENERATE_INSERT_NEXT(name, type, field, cmp, attr)		\
/* Inserts a node into the next position in the RB tree */		\
attr struct type *							\
name##_RB_INSERT_NEXT(struct name *head,				\
    struct type *elm, struct type *next)				\
{									\
	struct type *tmp;						\
	struct type **tmpp = &RB_RIGHT(elm, field);			\
									\
	_RB_ORDER_CHECK(cmp, elm, next);				\
	if (name##_RB_NEXT(elm) != NULL)				\
		_RB_ORDER_CHECK(cmp, next, name##_RB_NEXT(elm));	\
	while ((tmp = *tmpp) != NULL) {					\
		elm = tmp;						\
		tmpp = &RB_LEFT(elm, field);				\
	}								\
	return (name##_RB_INSERT_FINISH(head, elm, tmpp, next));	\
}

#define RB_GENERATE_PREV(name, type, field, attr)			\
/* ARGSUSED */								\
attr struct type *							\
name##_RB_PREV(struct type *elm)					\
{									\
	if (RB_LEFT(elm, field)) {					\
		elm = RB_LEFT(elm, field);				\
		while (RB_RIGHT(elm, field))				\
			elm = RB_RIGHT(elm, field);			\
	} else {							\
		while (RB_PARENT(elm, field) &&				\
		    (elm == RB_LEFT(RB_PARENT(elm, field), field)))	\
			elm = RB_PARENT(elm, field);			\
		elm = RB_PARENT(elm, field);				\
	}								\
	return (elm);							\
}

#define RB_GENERATE_INSERT_PREV(name, type, field, cmp, attr)		\
/* Inserts a node into the prev position in the RB tree */		\
attr struct type *							\
name##_RB_INSERT_PREV(struct name *head,				\
    struct type *elm, struct type *prev)				\
{									\
	struct type *tmp;						\
	struct type **tmpp = &RB_LEFT(elm, field);			\
									\
	_RB_ORDER_CHECK(cmp, prev, elm);				\
	if (name##_RB_PREV(elm) != NULL)				\
		_RB_ORDER_CHECK(cmp, name##_RB_PREV(elm), prev);	\
	while ((tmp = *tmpp) != NULL) {					\
		elm = tmp;						\
		tmpp = &RB_RIGHT(elm, field);				\
	}								\
	return (name##_RB_INSERT_FINISH(head, elm, tmpp, prev));	\
}

#define RB_GENERATE_MINMAX(name, type, field, attr)			\
attr struct type *							\
name##_RB_MINMAX(struct name *head, int val)				\
{									\
	struct type *tmp = RB_ROOT(head);				\
	struct type *parent = NULL;					\
	while (tmp) {							\
		parent = tmp;						\
		if (val < 0)						\
			tmp = RB_LEFT(tmp, field);			\
		else							\
			tmp = RB_RIGHT(tmp, field);			\
	}								\
	return (parent);						\
}

#define	RB_GENERATE_REINSERT(name, type, field, cmp, attr)		\
attr struct type *							\
name##_RB_REINSERT(struct name *head, struct type *elm)			\
{									\
	struct type *cmpelm;						\
	if (((cmpelm = RB_PREV(name, head, elm)) != NULL &&		\
	    cmp(cmpelm, elm) >= 0) ||					\
	    ((cmpelm = RB_NEXT(name, head, elm)) != NULL &&		\
	    cmp(elm, cmpelm) >= 0)) {					\
		/* XXXLAS: Remove/insert is heavy handed. */		\
		RB_REMOVE(name, head, elm);				\
		return (RB_INSERT(name, head, elm));			\
	}								\
	return (NULL);							\
}									\

#define RB_NEGINF	-1
#define RB_INF	1

#define RB_INSERT(name, x, y)	name##_RB_INSERT(x, y)
#define RB_INSERT_NEXT(name, x, y, z)	name##_RB_INSERT_NEXT(x, y, z)
#define RB_INSERT_PREV(name, x, y, z)	name##_RB_INSERT_PREV(x, y, z)
#define RB_REMOVE(name, x, y)	name##_RB_REMOVE(x, y)
#define RB_FIND(name, x, y)	name##_RB_FIND(x, y)
#define RB_NFIND(name, x, y)	name##_RB_NFIND(x, y)
#define RB_NEXT(name, x, y)	name##_RB_NEXT(y)
#define RB_PREV(name, x, y)	name##_RB_PREV(y)
#define RB_MIN(name, x)		name##_RB_MINMAX(x, RB_NEGINF)
#define RB_MAX(name, x)		name##_RB_MINMAX(x, RB_INF)
#define RB_REINSERT(name, x, y)	name##_RB_REINSERT(x, y)

#define RB_FOREACH(x, name, head)					\
	for ((x) = RB_MIN(name, head);					\
	     (x) != NULL;						\
	     (x) = name##_RB_NEXT(x))

#define RB_FOREACH_FROM(x, name, y)					\
	for ((x) = (y);							\
	    ((x) != NULL) && ((y) = name##_RB_NEXT(x), (x) != NULL);	\
	     (x) = (y))

#define RB_FOREACH_SAFE(x, name, head, y)				\
	for ((x) = RB_MIN(name, head);					\
	    ((x) != NULL) && ((y) = name##_RB_NEXT(x), (x) != NULL);	\
	     (x) = (y))

#define RB_FOREACH_REVERSE(x, name, head)				\
	for ((x) = RB_MAX(name, head);					\
	     (x) != NULL;						\
	     (x) = name##_RB_PREV(x))

#define RB_FOREACH_REVERSE_FROM(x, name, y)				\
	for ((x) = (y);							\
	    ((x) != NULL) && ((y) = name##_RB_PREV(x), (x) != NULL);	\
	     (x) = (y))

#define RB_FOREACH_REVERSE_SAFE(x, name, head, y)			\
	for ((x) = RB_MAX(name, head);					\
	    ((x) != NULL) && ((y) = name##_RB_PREV(x), (x) != NULL);	\
	     (x) = (y))

#endif