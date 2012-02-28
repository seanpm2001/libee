/**
 * @file event.c
 * Implements event object methods.
 *//* Libee - An Event Expression Library inspired by CEE
 * Copyright 2010,2011 by Rainer Gerhards and Adiscon GmbH.
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
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <libestr.h>

#include "collection/collection.h"
#include "libee/libee.h"
#include "libee/internal.h"
#include "libee/field.h"
#include "libee/value.h"
#include "libee/tagbucket.h"

#define ERR_ABORT {r = 1; goto done; }

#define CHECK_FIELD \
	if(event->objID != ObjID_EVENT) { \
		r = -1; \
		goto done; \
	}


struct ee_event*
ee_newEvent(ee_ctx __attribute__((unused)) ctx)
{
	struct ee_event *event;
	if((event = malloc(sizeof(struct ee_event))) == NULL)
		goto done;
	if(col_create_collection(&event->data, "[ROOT]", 0) != 0) {
		free(event);
		event = NULL;
	}

	event->objID = ObjID_EVENT;
	event->ctx = ctx;
	event->tags = NULL;

done:
	return event;
}


void
ee_deleteEvent(struct ee_event *event)
{
	assert(event != NULL);assert(event->objID == ObjID_EVENT);
	if(event->tags != NULL)
		ee_deleteTagbucket(event->tags);
	if(event->data != NULL)
		col_destroy_collection(event->data);
	free(event);
}


int
ee_assignTagbucketToEvent(struct ee_event *event, struct ee_tagbucket *tagbucket)
{
	int r = 0;

	if(event == NULL || tagbucket == NULL) {
		r = EE_EINVAL;
		goto done;
	}

	if(event->tags != NULL)
		ee_deleteTagbucket(event->tags);

	event->tags = tagbucket;
	
done:
	return r;
}


int
ee_addTagToEvent(struct ee_event *event, es_str_t *tag)
{
	int r = -1;

	assert(event != NULL);assert(event->objID == ObjID_EVENT);
	if(event->tags == NULL)
		if((event->tags = ee_newTagbucket(event->ctx)) == NULL)
			goto done;

	r = ee_addTagToBucket(event->tags, es_strdup(tag));
	
done:
	return r;
}


#if 0
int
ee_addFieldToEvent(struct ee_event *event, struct ee_field *field)
{
	int r;

	assert(event != NULL);assert(event->objID == ObjID_EVENT);
	if(event->fields == NULL) {
		CHKN(event->fields = ee_newFieldbucket(event->ctx));
	}

	r = ee_addFieldToBucket(event->fields, field);
	
done:
	return r;
}
#endif


int
ee_addStrFieldToEvent(struct ee_event *event, char *fieldname, es_str_t *value)
{
	int r = -1;
	char *cstr = NULL;

	assert(event != NULL);assert(event->objID == ObjID_EVENT);
//printf("addStrField: %s/%s\n", fieldname, es_str2cstr(value, NULL));

	cstr = es_str2cstr(value, NULL);
	if(col_add_str_property(event->data, NULL, fieldname, cstr, 0) != 0)
		goto done;
	r = 0;

done:
	if(cstr != NULL)
		free(cstr);
	return r;
}

int
ee_EventHasTag(struct ee_event *event, es_str_t *tagname)
{
	int r;
	if(event->tags == NULL)
		r = 0;
	else
		r = ee_TagbucketHasTag(event->tags, tagname);
	return r;
}


struct ee_field*
ee_getEventField(struct ee_event *event, es_str_t *name)
{
#warning need to implement?
	return NULL; //return(ee_getBucketField(event->fields, name));
}


/* TODO: this function should use the default encoder. However, none of
 * that plumbing currently exists. So the current implementation is just
 * a quick skeleton, which needs to be extended severely (but it still
 * is useful).
 */
int
ee_getEventFieldAsString(struct ee_event *event, es_str_t *name, es_str_t **strVal)
{
	int r = EE_ERR;
	struct ee_field *f;
	struct ee_tagbucket_listnode *tag;
	int needComma = 0;

	/* checking event.tags is a hack and will be removed with the
	 * next version when I change the internal representation of
	 * tags (to keep them in line with current CEE developments).
	 * TODO -- rgerhards, 2011-04-12
	 */
	if(!es_strbufcmp(name, (unsigned char*) "event.tags", 10)) {
		if(event->tags == NULL) {
			r = EE_NOTFOUND;
			goto done;
		}
		if(*strVal == NULL) {
			CHKN(*strVal = es_newStr(16));
		}
		for(tag = event->tags->root ; tag != NULL ; tag = tag->next) {
			if(needComma) {
				CHKR(es_addChar(strVal, ','));
			} else {
				needComma = 1;
			}
			CHKR(es_addStr(strVal, tag->name));
		}
	} else {
		f = (struct ee_field*) ee_getBucketField(event->data, name);
		if(f == NULL) {
			r = EE_NOTFOUND;
			goto done;
		}
		CHKR(ee_getFieldAsString(f, strVal));
	}

done:
	return r;
}


void
ee_EventGetTagbucket(struct ee_event *event, struct ee_tagbucket **tagbucket)
{
	*tagbucket = event->tags;
}
