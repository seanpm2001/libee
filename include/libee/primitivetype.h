/**
 * @file primitivetype.h
 * @brief Primitive data types.
 * @class ee_primitiveType primitivetype.h
 *
 *//*
 *
 * Libee - An Event Expression Library inspired by CEE
 * Copyright 2010 by Rainer Gerhards and Adiscon GmbH.
 *
 * This file is part of libee.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * A copy of the LGPL v2.1 can be found in the file "COPYING" in this distribution.
 */
#ifndef LIBEE_PRIMITIVETYPE_H_INCLUDED
#define	LIBEE_PRIMITIVETYPE_H_INCLUDED

/**
 * Representation of a primitive data type.
 *
 * @extends ee_obj
 *
 * A primitive data type is also called a "base type" in CEE. It is at the root
 * of all type hierarchie. Note that libee primitives include types which do
 * not seem to be very primitive from a generic programming language type of
 * view. For us, primitives are those that are very frequently used on logs.
 * We also try to avoid regexes, as these can be very time-consuming. So we
 * have things like IPv4Address or FQDN names as primitive types.
 */
struct ee_primitiveType {
	struct ee_obj o;	/*<< the base object */
	int (*parse)(ee_ctx ctx, char *p, struct ee_field* newField);
};

/**
 * Constructor for the ee_primitiveType object.
 *
 * @memberof ee_primitiveType
 * @public
 *
 * @return new library context or NULL if an error occured
 */
struct ee_primitiveType* ee_newPrimitiveType(void);

/**
 * Destructor for the ee_primitiveType object.
 *
 * @memberof ee_primitiveType
 * @public
 *
 * @param primitiveType The primitiveType to be discarded.
 */
void ee_deletePrimitiveType(struct ee_primitiveType *primitiveType);


#endif /* #ifndef LIBEE_PRIMITIVETYPE_H_INCLUDED */