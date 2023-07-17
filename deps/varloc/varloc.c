/*
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2007 Davi E. M. Arnaut <davi@haxent.com.br>
 */

#include <argp.h>
#include <malloc.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dwarves.h"
#include "varloc.h"
#include "dutil.h"

static int verbose;

static struct conf_fprintf conf = {
	.emit_stats = 1,
};

static struct conf_load conf_load = {
	.conf_fprintf = &conf,
    .get_addr_info = 1,
    .extra_dbg_info = 1,
};

void process_extvar(struct variable *var, struct cu *cu);

//struct extvar {
//	struct extvar		*next;
//	const char 		*name;
//	const struct variable	*var;
//	const struct cu 	*cu;
//};

//struct extfun {
//	struct extfun		*next;
//	const char		*name;
//	const struct function	*fun;
//	const struct cu		*cu;
//};

//static void *tree;

//static void oom(const char *msg)
//{
//	fprintf(stderr, "pglobal: out of memory (%s)\n", msg);
//	exit(EXIT_FAILURE);
//}

//static struct extvar *extvar__new(const struct variable *var,
//				  const struct cu *cu)
//{
//	struct extvar *gvar = malloc(sizeof(*gvar));

//	if (gvar != NULL) {
//		gvar->next = NULL;
//		gvar->var  = var;
//		gvar->cu   = cu;
//		gvar->name = variable__name(var);
//	}

//	return gvar;
//}

//static struct extfun *extfun__new(struct function *fun,
//				  const struct cu *cu)
//{
//	struct extfun *gfun = malloc(sizeof(*gfun));

//	if (gfun != NULL) {
//		gfun->next = NULL;
//		gfun->fun  = fun;
//		gfun->cu   = cu;
//		gfun->name = function__name(fun);
//	}

//	return gfun;
//}

//static int extvar__compare(const void *a, const void *b)
//{
//	const struct extvar *ga = a, *gb = b;
//	return strcmp(ga->name, gb->name);
//}

//static int extfun__compare(const void *a, const void *b)
//{
//	const struct extfun *ga = a, *gb = b;
//	return strcmp(ga->name, gb->name);
//}

//static void extvar__add(const struct variable *var, const struct cu *cu)
//{
//	struct extvar **nodep, *gvar = extvar__new(var, cu);

//	if (gvar != NULL) {
//		nodep = tsearch(gvar, &tree, extvar__compare);
//		if (nodep == NULL)
//			oom("tsearch");
//		else if (*nodep != gvar) {
//			if (gvar->var->declaration) {
//				gvar->next = (*nodep)->next;
//				(*nodep)->next = gvar;
//			} else {
//				gvar->next = *nodep;
//				*nodep = gvar;
//			}
//		}
//	}
//}

//static void extfun__add(struct function *fun, const struct cu *cu)
//{
//	struct extfun **nodep, *gfun = extfun__new(fun, cu);

//	if (gfun != NULL) {
//		nodep = tsearch(gfun, &tree, extfun__compare);
//		if (nodep == NULL)
//			oom("tsearch");
//		else if (*nodep != gfun) {
//			gfun->next = (*nodep)->next;
//			(*nodep)->next = gfun;
//		}
//	}
//}





static int cu_extvar_iterator(struct cu *cu, void *cookie __maybe_unused)
{
	struct tag *pos;
	uint32_t id;

	cu__for_each_variable(cu, id, pos) {
		struct variable *var = tag__variable(pos);
		if (var->external)
            process_extvar(var,cu);
//			extvar__add(var, cu);
	}
	return 0;
}

//static int cu_extfun_iterator(struct cu *cu, void *cookie __maybe_unused)
//{
//	struct function *pos;
//	uint32_t id;

//	cu__for_each_function(cu, id, pos)
//		if (pos->external)
//			extfun__add(pos, cu);
//	return 0;
//}

//static inline const struct extvar *node__variable(const void *nodep)
//{
//	return *((const struct extvar **)nodep);
//}

//static inline const struct extfun *node__function(const void *nodep)
//{
//	return *((const struct extfun **)nodep);
//}

//static inline struct tag *extvar__tag(const struct extvar *gvar)
//{
//    return (struct tag *)gvar->var;
//}

//static inline struct tag *extfun__tag(const struct extfun *gfun)
//{
//    return (struct tag *)gfun->fun;
//}


/*
 * -------------------------------------------------------------------------
 */

void class_print(struct class *class, const struct cu *cu,
                 const struct conf_fprintf *conf, FILE *fp);

static void print_type(struct tag *type, const struct cu *cu,
                       char* name, const struct conf_fprintf *conf);


void process_extvar(struct variable *gvar, struct cu *cu){
    uint32_t count = 0;
    struct tag *tag;

    if (gvar == NULL)
        return;

    tag = &gvar->ip.tag; //extvar__tag(var);

    struct conf_fprintf cfg = {0};
    cfg.expand_types = 1;
    //    cfg.expand_pointers = 1;
    cfg.rel_offset = 1;
    //    tag__fprintf(tag, gvar->cu, &cfg, stdout);

    //    for (pos = gvar->next; pos; pos = pos->next)
    //        count++;
    //    printf("; /* %u */\n\n", count);

    char name_buf[100];
    //    ++tag->recursivity_level;


    if (tag->tag == DW_TAG_variable){
        const struct variable *var = tag__variable(tag);
        const char *name = variable__name(var);
        const char *type_name = variable__type_name(var, cu, name_buf, 100);

        const struct tag *type_tag = cu__type(cu, var->ip.tag.type);
        int base_type = tag__is_base_type(type_tag, cu);
        printf("\ngot var %s %s %d %x : ",
               name,
               type_name,
               base_type,
               var->ip.addr);

        if (!base_type){
            //            typedef__fprintf(&var->ip.tag, gvar->cu, &cfg, stdout);
            //            tag__fprintf(tag, gvar->cu, &cfg, stdout);
            printf("\n");
            print_type(type_tag, cu, NULL, &cfg);
        }
    }
}


static void print_array(const struct tag *tag,
                        const struct cu *cu, const char *name,
                        const struct conf_fprintf *conf)
{
    struct array_type *at = tag__array_type(tag);
    struct tag *type = cu__type(cu, tag->type);
    unsigned long long flat_dimensions = 0;
    int i;

    if (type == NULL)
        return; // tag__id_not_found_fprintf(fp, tag->type);

    /* Zero sized arrays? */
    if (at->dimensions >= 1 && at->nr_entries[0] == 0 && tag__is_const(type))
        type = cu__type(cu, type->type);

    print_type(type, cu, name, conf);
    for (i = 0; i < at->dimensions; ++i) {
        if (conf->flat_arrays || at->is_vector) {
            /*
             * Seen on the Linux kernel on tun_filter:
             *
             * __u8   addr[0][ETH_ALEN];
             */
            if (at->nr_entries[i] == 0 && i == 0)
                break;
            if (!flat_dimensions)
                flat_dimensions = at->nr_entries[i];
            else
                flat_dimensions *= at->nr_entries[i];
        } else {
            bool single_member = conf->last_member && conf->first_member;

            if (at->nr_entries[i] != 0 || !conf->last_member || single_member || conf->union_member)
                printf("[%u]", at->nr_entries[i]);
            else
                printf("[]");
        }
    }

    if (at->is_vector) {
        type = tag__follow_typedef(tag, cu);

        if (flat_dimensions == 0)
            flat_dimensions = 1;
        printf(" __attribute__ ((__vector_size__ (%llu)))",
               flat_dimensions * tag__size(type, cu));
    } else if (conf->flat_arrays) {
        bool single_member = conf->last_member && conf->first_member;

        if (flat_dimensions != 0 || !conf->last_member || single_member || conf->union_member)
            printf("[%llu]", flat_dimensions);
        else
            printf("[]");
    }

    return;
}

static void print_member(struct class_member *member, bool union_member,
                                    struct tag *type, const struct cu *cu,
                                    struct conf_fprintf *conf)
{
    printf("\n");
    const int size = member->byte_size;
    int member_alignment_printed = 0;
    struct conf_fprintf sconf = *conf;
    uint32_t offset = member->byte_offset;
    size_t printed = 0, printed_cacheline = 0;
    const char *cm_name = class_member__name(member),
        *name = cm_name;

    if (!sconf.rel_offset) {
        offset += sconf.base_offset;
        if (!union_member)
            sconf.base_offset = offset;
    }

    printf("size %d offset %d", size, offset);
    if (member->bitfield_offset < 0)
        offset += member->byte_size;

//    if (!conf->suppress_comments)
//        printed_cacheline = class__fprintf_cacheline_boundary(conf, offset, fp);

    if (member->tag.tag == DW_TAG_inheritance) {
        name = "<ancestor>";
        printf("/* ");
    }

    if (member->is_static)
        printf("static ");

    /* For struct-like constructs, the name of the member cannot be
     * conflated with the name of its type, otherwise __attribute__ are
     * printed in the wrong order.
     */
    if (tag__is_union(type) || tag__is_struct(type) ||
        tag__is_enumeration(type))
    {
        print_type(type, cu, NULL, &sconf);
        if (name) {
            if (!type__name(tag__type(type))){
                printf(" ");
            }
            printf("%s", name);
        }
    } else {
        print_type(type, cu, name, &sconf);
    }

    if (member->is_static) {
        if (member->const_value != 0)
            printf(" = %", member->const_value);
    } else if (member->bitfield_size != 0) {
        printf(":%u", member->bitfield_size);
    }

//    if (!sconf.suppress_aligned_attribute && member->alignment != 0) {
//        member_alignment_printed = fprintf(fp, " __attribute__((__aligned__(%u)))", member->alignment);
//        printed += member_alignment_printed;
//    }

//    printf(';');

//    if ((tag__is_union(type) || tag__is_struct(type) ||
//         tag__is_enumeration(type)) &&
//        /* Look if is a type defined inline */
//            type__name(tag__type(type)) == NULL) {
//        if (!sconf.suppress_offset_comment) {
//            /* Check if this is a anonymous union */
//            int slen = member_alignment_printed + (cm_name ? (int)strlen(cm_name) : -1);
//            int size_spacing = 5;

//            if (tag__is_struct(type) && tag__class(type)->is_packed && !conf->suppress_packed) {
//                int packed_len = sizeof("__attribute__((__packed__))");
//                slen += packed_len;
//            }

//            printed += fprintf(fp, sconf.hex_fmt ?
//                                       "%*s/* %#5x" :
//                                       "%*s/* %5u",
//                               (sconf.type_spacing +
//                                sconf.name_spacing - slen - 3),
//                               " ", offset);

//            if (member->bitfield_size != 0) {
//                unsigned int bitfield_offset = member->bitfield_offset;

//                if (member->bitfield_offset < 0)
//                    bitfield_offset = member->byte_size * 8 + member->bitfield_offset;

//                printed += fprintf(fp, sconf.hex_fmt ?  ":%#2x" : ":%2u", bitfield_offset);
//                size_spacing -= 3;
//            }

//            printed += fprintf(fp, sconf.hex_fmt ?  " %#*x */" : " %*u */", size_spacing, size);
//        }
//    } else {
//        int spacing = sconf.type_spacing + sconf.name_spacing - printed;

//        if (member->tag.tag == DW_TAG_inheritance) {
//            const size_t p = fprintf(fp, " */");
//            printed += p;
//            spacing -= p;
//        }
//        if (!sconf.suppress_offset_comment) {
//            int size_spacing = 5;

//            printed += fprintf(fp, sconf.hex_fmt ?
//                                       "%*s/* %#5x" : "%*s/* %5u",
//                               spacing > 0 ? spacing : 0, " ",
//                               offset);

//            if (member->bitfield_size != 0) {
//                unsigned int bitfield_offset = member->bitfield_offset;

//                if (member->bitfield_offset < 0)
//                    bitfield_offset = member->byte_size * 8 + member->bitfield_offset;

//                printed += fprintf(fp, sconf.hex_fmt ?
//                                           ":%#2x" : ":%2u",
//                                   bitfield_offset);
//                size_spacing -= 3;
//            }

//            printed += fprintf(fp, sconf.hex_fmt ?
//                                       " %#*x */" : " %*u */",
//                               size_spacing, size);
//        }
//    }
    return;
}


static void print_type(struct tag *type, const struct cu *cu, char* name, const struct conf_fprintf *conf){
    if (name == NULL){
        name = "test_name";
    }
    char tbf[128];
    char namebf[256];
    char namebfptr[258];
    struct type *ctype;
    struct tag *type_expanded = NULL;
    int typedef_expanded = 0;
    struct conf_fprintf tconf = {
//        .type_spacing = conf->type_spacing,
    };
    int expand_types = 1;
    int expand_pointers = 1;

    // expand pointers
    if (expand_pointers){
        int nr_indirections = 0;

        while (tag__is_pointer(type) && type->type != 0) {
            struct tag *ttype = cu__type(cu, type->type);
            if (ttype == NULL)
                return;
            else {
                int printed = tag__has_type_loop(type, ttype,
                                             NULL, 0, stdout);
                if (printed)
                    return;
            }
            type = ttype;
            ++nr_indirections;
        }

        if (nr_indirections > 0) {
            const size_t len = strlen(name);
            if (len + nr_indirections >= sizeof(namebf))
                return;
            memset(namebf, '*', nr_indirections);
            memcpy(namebf + nr_indirections, name, len);
            namebf[len + nr_indirections] = '\0';
            name = namebf;
            printf(" POINTER!  %s ", name);
        }
        else{
            nr_indirections = 1;
        }

        expand_types = nr_indirections;
        /* Avoid loops */
        if (type->recursivity_level != 0){
            expand_types = 0;
        }
        ++type->recursivity_level;
        type_expanded = type;

    }

    // expand types
    if (expand_types){
        while (tag__is_typedef(type)) {
            struct tag *type_type;
            int n;

            ctype = tag__type(type);
            if (typedef_expanded)
                printf(" -> %s", type__name(ctype));
            else {
                printf("/* typedef %s", type__name(ctype));
                typedef_expanded = 1;
            }
            type_type = cu__type(cu, type->type);
            if (type_type == NULL)
                return;
            n = tag__has_type_loop(type, type_type, NULL, 0, stdout);
            if (n)
                return;
            type = type_type;
        }
        if (typedef_expanded){
            printf(" */ ");
        }
    }

    tconf = *conf;

    if (tag__is_struct(type) || tag__is_union(type) ||
        tag__is_enumeration(type)) {
    inner_struct:
        tconf.prefix	   = NULL;
        tconf.suffix	   = name;
        tconf.emit_stats   = 0;
        tconf.suppress_offset_comment = 1;
    }

    const char *modifier;

next_type:
    switch (type->tag) {
    case DW_TAG_pointer_type: {
//        type_id_t ptype_id = skip_llvm_annotations(cu, type->type);

        if (type->type != 0) {
            int n;
            struct tag *ptype = cu__type(cu, type->type);
            if (ptype == NULL)
                return;
            n = tag__has_type_loop(type, ptype, NULL, 0, stdout);
            if (n)
                return;
//            if (ptype->tag == DW_TAG_subroutine_type) {
//                printed += ftype__fprintf(tag__ftype(ptype),
//                                          cu, name, 0, 1,
//                                          tconf.type_spacing, true,
//                                          &tconf, fp);
//                break;
//            }
            if ((tag__is_struct(ptype) || tag__is_union(ptype) ||
                 tag__is_enumeration(ptype)) && type__name(tag__type(ptype)) == NULL) {
                if (name == namebfptr)
                    return;
                snprintf(namebfptr, sizeof(namebfptr), "* %.*s", (int)sizeof(namebfptr) - 3, name);
                tconf.rel_offset = 1;
                name = namebfptr;
                type = ptype;
                tconf.type_spacing -= 8;
                goto inner_struct;
            }
        }
        /* Fall Thru */
    }
    default:
    print_default:
        printf("%-*s %s", tconf.type_spacing,
                           tag__name(type, cu, tbf, sizeof(tbf), &tconf),
                           name);
        break;
    case DW_TAG_subroutine_type:
//        printed += ftype__fprintf(tag__ftype(type), cu, name, 0, 0,
//                                  tconf.type_spacing, true, &tconf, fp);
        printf("ftype__fprintf");
        break;
    case DW_TAG_atomic_type:
        modifier = "_Atomic";
        goto print_modifier;
    case DW_TAG_const_type:
        modifier = "const";
    print_modifier: {
//        if (!conf->skip_emitting_modifier) {
//            size_t modifier_printed = fprintf(fp, "%s ", modifier);
//            tconf.type_spacing -= modifier_printed;
//            printed		   += modifier_printed;
//        }

        struct tag *ttype = cu__type(cu, type->type);
        if (ttype) {
            type = ttype;
            goto next_type;
        }
    }
        goto print_default;

    case DW_TAG_array_type:
//        printed += array_type__fprintf(type, cu, name, &tconf, fp);
        print_array(type, cu, name, &tconf);
        break;
    case DW_TAG_string_type:
//        printed += string_type__fprintf(type, name, &tconf, fp);
        printf("string_type__fprintf");
        break;
    case DW_TAG_class_type:
    case DW_TAG_structure_type:
        ctype = tag__type(type);

//        if (type__name(ctype) != NULL && !expand_types) {
//            printed += fprintf(fp, "%s %-*s %s",
//                               (type->tag == DW_TAG_class_type &&
//                                !tconf.classes_as_structs) ? "class" : "struct",
//                               tconf.type_spacing - 7,
//                               type__name(ctype), name ?: "");
//        } else {
            struct class *cclass = tag__class(type);

//           if (!tconf.suppress_comments)
//                class__find_holes(cclass);
            tconf.type_spacing -= 8;
//            printed += __class__fprintf(cclass, cu, &tconf, fp);
            class_print(cclass, cu, &tconf, stdout);
//            printf("__class__fprintf");
//        }
        break;
    case DW_TAG_union_type:
        ctype = tag__type(type);

//        if (type__name(ctype) != NULL && !expand_types) {
//            printed += fprintf(fp, "union %-*s %s", tconf.type_spacing - 6, type__name(ctype), name ?: "");
//        } else {
            tconf.type_spacing -= 8;
//            printed += union__fprintf(ctype, cu, &tconf, fp);
            printf("union__fprintf");
//        }
        break;
    case DW_TAG_enumeration_type:
        ctype = tag__type(type);

        if (type__name(ctype) != NULL)
            printf("enum %-*s %s", tconf.type_spacing - 5, type__name(ctype), name ?: "");
        else
//            printed += enumeration__fprintf(type, &tconf, fp);
            printf("enumeration__fprintf");
        break;
    case DW_TAG_LLVM_annotation: {
        struct tag *ttype = cu__type(cu, type->type);
        if (ttype) {
            type = ttype;
            goto next_type;
        }
//        goto out_type_not_found;
        return;
    }
    }
out:
    if (type_expanded)
        --type_expanded->recursivity_level;

    return;
}

void class_print(struct class *class, const struct cu *cu,
                               const struct conf_fprintf *conf, FILE *fp)
{
    struct type *type = &class->type;
    size_t last_size = 0, size;
    uint8_t newline = 0;
    uint16_t nr_paddings = 0;
    uint16_t nr_forced_alignments = 0, nr_forced_alignment_holes = 0;
    uint32_t sum_forced_alignment_holes = 0;
    uint32_t sum_bytes = 0, sum_bits = 0;
    uint32_t sum_holes = 0;
    uint32_t sum_paddings = 0;
    uint32_t sum_bit_holes = 0;
    uint32_t cacheline = 0;
    int size_diff = 0;
    int first = 1;
    struct class_member *pos, *last = NULL;
    struct tag *tag_pos;
//    const char *current_accessibility = NULL;
    struct conf_fprintf cconf = *conf;// : conf_fprintf__defaults;
//    const uint16_t t = type->namespace.tag.tag;
//    size_t printed = fprintf(fp, "%s%s%s%s%s",
//                             cconf.prefix ?: "", cconf.prefix ? " " : "",
//                             ((cconf.classes_as_structs ||
//                               t == DW_TAG_structure_type) ? "struct" :
//                                  t == DW_TAG_class_type ? "class" :
//                                  "interface"),
//                             type__name(type) ? " " : "",
//                             type__name(type) ?: "");
//    int indent = cconf.indent;

//    if (indent >= (int)sizeof(tabs))
//        indent = sizeof(tabs) - 1;

//    if (cconf.cachelinep == NULL)
//        cconf.cachelinep = &cacheline;

//    cconf.indent = indent + 1;
//    cconf.no_semicolon = 0;

    class__infer_packed_attributes(class, cu);

    /* First look if we have DW_TAG_inheritance */
    type__for_each_tag(type, tag_pos) {

//        const char *accessibility;

        if (tag_pos->tag != DW_TAG_inheritance)
            continue;

        if (first) {
            printf(" :");
            first = 0;
        } else
            printf(",");

        pos = tag__class_member(tag_pos);

        if (pos->virtuality == DW_VIRTUALITY_virtual)
            printf(" virtual");

//        accessibility = tag__accessibility(tag_pos);
//        if (accessibility != NULL)
//           printf(" %s", accessibility);

        struct tag *pos_type = cu__type(cu, tag_pos->type);
        if (pos_type != NULL)
            printf(" %s",
                 type__name(tag__type(pos_type)));
        else
            tag__id_not_found_fprintf(fp, tag_pos->type);
    }

//    fprintf(fp, " {\n");

    type__for_each_tag(type, tag_pos) {

//        const char *accessibility = tag__accessibility(tag_pos);

//        if (accessibility != NULL &&
//            accessibility != current_accessibility) {
//            current_accessibility = accessibility;
//            printf("%.*s%s:\n\n",
//                               cconf.indent - 1, tabs,
//                               accessibility);
//        }

        if (tag_pos->tag != DW_TAG_member &&
            tag_pos->tag != DW_TAG_inheritance) {
//            if (!cconf.show_only_data_members) {
//                tag__fprintf(tag_pos, cu, &cconf, stdout);
                printf("tag__fprintf");
//            }
            continue;
        }
        pos = tag__class_member(tag_pos);

        if (!cconf.suppress_aligned_attribute && pos->alignment != 0) {
            uint32_t forced_alignment_hole = last ? last->hole : class->pre_hole;

            if (forced_alignment_hole != 0) {
                ++nr_forced_alignment_holes;
                sum_forced_alignment_holes += forced_alignment_hole;
            }
            ++nr_forced_alignments;
        }
        /*
         * These paranoid checks doesn't make much sense on
         * DW_TAG_inheritance, have to understand why virtual public
         * ancestors make the offset go backwards...
         */
        if (last != NULL && tag_pos->tag == DW_TAG_member &&
            /*
         * kmemcheck bitfield tricks use zero sized arrays as markers
         * all over the place.
         */
                last_size != 0) {
            if (last->bit_hole != 0 && pos->bitfield_size) {
                uint8_t bitfield_size = last->bit_hole;
                struct tag *pos_type = cu__type(cu, pos->tag.type);

                if (pos_type == NULL) {
                    printf("%.*s", cconf.indent, tabs);
//                    tag__id_not_found_fprintf(fp, pos->tag.type);
                    continue;
                }
                /*
                 * Now check if this isn't something like 'unsigned :N' with N > 0,
                 * i.e. _explicitely_ adding a bit hole.
                 */
                if (last->byte_offset != pos->byte_offset) {
                    printf("\n%.*s/* Force alignment to the next boundary: */\n", cconf.indent, tabs);
                    bitfield_size = 0;
                }

//                printf("%.*s", cconf.indent, tabs);
                printf(" print type");
                print_type(pos_type, cu, NULL, conf);
//                type__fprintf(pos_type, cu, "", &cconf, fp);
                printf(":%u;\n", bitfield_size);
            }
        }

//        if (newline) {
//            fputc('\n', fp);
//            newline = 0;
//            ++printed;
//        }

        struct tag *pos_type = cu__type(cu, pos->tag.type);
        if (pos_type == NULL) {
//            printed += fprintf(fp, "%.*s", cconf.indent, tabs);
//            printed += tag__id_not_found_fprintf(fp, pos->tag.type);
            continue;
        }

        cconf.last_member = list_is_last(&tag_pos->node, &type->namespace.tags);
        cconf.first_member = last == NULL;

        size = pos->byte_size;
//        printf("%.*s", cconf.indent, tabs);
//        printf("struct_member__fprintf ");
        print_member(pos, false, pos_type, cu, &cconf);
//        struct_member__fprintf(pos, pos_type, cu, &cconf, fp);



//        fputc('\n', fp);
//        ++printed;

        /* XXX for now just skip these */
        if (tag_pos->tag == DW_TAG_inheritance)
            continue;
#if 0
        /*
         * This one was being skipped but caused problems with:
         * http://article.gmane.org/gmane.comp.debugging.dwarves/185
         * http://www.spinics.net/lists/dwarves/msg00119.html
         */
        if (pos->virtuality == DW_VIRTUALITY_virtual)
            continue;
#endif

        if (pos->bitfield_size) {
            sum_bits += pos->bitfield_size;
        } else {
            sum_bytes += pos->byte_size;
        }

        if (last == NULL || /* First member */
                                /*
             * Last member was a zero sized array, typedef, struct, etc
             */
                                last_size == 0 ||
            /*
             * We moved to a new offset
             */
                last->byte_offset != pos->byte_offset) {
            last_size = size;
        } else if (last->bitfield_size == 0 && pos->bitfield_size != 0) {
            /*
             * Transitioned from from a non-bitfield to a
             * bitfield sharing the same offset
             */
            /*
             * Compensate by removing the size of the
             * last member that is "inside" this new
             * member at the same offset.
             *
             * E.g.:
             * struct foo {
             * 	u8	a;   / 0    1 /
             * 	int	b:1; / 0:23 4 /
             * }
             */
            last_size = size;
        }

        last = pos;
    }

    /*
     * BTF doesn't have alignment info, for now use this infor from the loader
     * to avoid adding the forced bitfield paddings and have btfdiff happy.
     */
    if (class->padding != 0 && type->alignment == 0 && cconf.has_alignment_info &&
        !cconf.suppress_force_paddings && last != NULL) {
        tag_pos = cu__type(cu, last->tag.type);
        size = tag__size(tag_pos, cu);

        if (is_power_of_2(size) && class->padding > cu->addr_size) {
            int added_padding;
            int bit_size = size * 8;

            printf("\n%.*s/* Force padding: */\n", cconf.indent, tabs);

            for (added_padding = 0; added_padding < class->padding; added_padding += size) {
//                printed += fprintf(fp, "%.*s", cconf.indent, tabs);
                printf(" print type");
                print_type(tag_pos, cu, NULL, &cconf);
                printf(":%u;\n", bit_size);
            }
        }
    }

//    if (!cconf.show_only_data_members)
//        class__vtable_fprintf(class, &cconf, fp);
}

//static void declaration_action__walk(const void *nodep, const VISIT which,
//                                     const int depth __maybe_unused)
//{
//    uint32_t count = 0;
//    struct tag *tag;
//    const struct extvar *gvar = NULL;

//    switch(which) {
//    case preorder:
//        break;
//    case postorder:
//        gvar = node__variable(nodep);
//        break;
//    case endorder:
//        break;
//    case leaf:
//        gvar = node__variable(nodep);
//        break;
//    }

//    if (gvar == NULL)
//        return;

//    tag = extvar__tag(gvar);

//    struct conf_fprintf cfg = {0};
//    cfg.expand_types = 1;
//    //    cfg.expand_pointers = 1;
//    cfg.rel_offset = 1;
//    //    tag__fprintf(tag, gvar->cu, &cfg, stdout);

//    //    for (pos = gvar->next; pos; pos = pos->next)
//    //        count++;
//    //    printf("; /* %u */\n\n", count);

//    char name_buf[100];
//    //    ++tag->recursivity_level;


//    if (tag->tag == DW_TAG_variable){
//        const struct variable *var = tag__variable(tag);
//        const char *name = variable__name(var);
//        const char *type_name = variable__type_name(var, gvar->cu, name_buf, 100);

//        const struct tag *type_tag = cu__type(gvar->cu, var->ip.tag.type);
//        int base_type = tag__is_base_type(type_tag, gvar->cu);
//        printf("\ngot var %s %s %d %x : ",
//               name,
//               type_name,
//               base_type,
//               var->ip.addr);

//        if (!base_type){
//            //            typedef__fprintf(&var->ip.tag, gvar->cu, &cfg, stdout);
//            //            tag__fprintf(tag, gvar->cu, &cfg, stdout);
//            printf("\n");
//            print_type(type_tag, gvar->cu, NULL, &cfg);
//        }
//    }
//}


//static void function_action__walk(const void *nodep, const VISIT which,
//                  const int depth __maybe_unused)
//{
//	struct tag *tag;
//	const struct extfun *gfun = NULL;

//	switch(which) {
//	case preorder:
//		break;
//	case postorder:
//		gfun = node__function(nodep);
//		break;
//	case endorder:
//		break;
//	case leaf:
//		gfun = node__function(nodep);
//		break;
//	}

//	if (gfun == NULL)
//		return;

//	tag = extfun__tag(gfun);

//	tag__fprintf(tag, gfun->cu, NULL, stdout);

//	fputs("\n\n", stdout);
//}

//static void free_node(void *nodep)
//{
//	void **node = nodep;
//	free(*node);
//}


int varloc(char* file){
	int err, rc = EXIT_FAILURE;
	if (dwarves__init()) {
		fputs("pglobal: insufficient memory\n", stderr);
		goto out;
	}

	dwarves__resolve_cacheline_size(&conf_load, 0);

	struct cus *cus = cus__new();
	if (cus == NULL) {
		fputs("pglobal: insufficient memory\n", stderr);
		goto out_dwarves_exit;
	}

    err = cus__load_file(cus, &conf_load, file);
	if (err != 0) {
        cus__fprintf_load_files_err(cus, "pglobal", file, err, stderr);
		goto out_cus_delete;
	}

//	if (walk_var) {
        cus__for_each_cu(cus, cu_extvar_iterator, NULL, NULL);
//		twalk(tree, declaration_action__walk);
//	} else if (walk_fun) {
//		cus__for_each_cu(cus, cu_extfun_iterator, NULL, NULL);
//		twalk(tree, function_action__walk);
//	}

//	tdestroy(tree, free_node);
    rc = EXIT_SUCCESS;
out_cus_delete:
	cus__delete(cus);
out_dwarves_exit:
	dwarves__exit();
out:
	return rc;
}
