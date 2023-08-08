#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dwarves.h"
#include "varloc.h"


char* varloc_node_types[] = {
    "BASE",
    "STRUCT",
    "ENUM",
    "POINTER",
    "UNION",
    "ARRAY"
};


void for_each_var_loop(varloc_node_t* root, void(*func)(varloc_node_t*)){
    if (root == NULL){
        return;
    }
    func(root);
    if (root->child != NULL){
        for_each_var_loop(root->child, func);
    }
    if (root->next != NULL){
        for_each_var_loop(root->next, func);
    }
}

void varloc_delete_tree(varloc_node_t* root){
    if (root == NULL){
        return;
    }
    if (root->child != NULL){
        varloc_delete_tree(root->child);
    }
    if (root->next != NULL){
        varloc_delete_tree(root->next);
    }
    free(root);
}



void print_var_node(varloc_node_t* var){
//    for (int i = 0; i < var_loop_level; i++){
//        printf("  ");
//    }
    printf("%s %s %s base:%x off:%d size:%d sign:%d type:%d items:%d\n",
           varloc_node_types[var->var_type],
           var->name,
           var->ctype_name,
           var->address.base,
           var->address.offset_bits,
           var->address.size_bits,
           var->is_signed,
           var->type_size,
           var->n_items
           );
}

varloc_node_t* last_var_node = NULL;
varloc_node_t* tree_base = NULL;
int indent = 0;


char* var_node_get_type_name(varloc_node_t* node){
    return varloc_node_types[node->var_type];
}

varloc_node_t* var_node_get_parent(varloc_node_t* child){
    if (child == NULL){
        return NULL;
    }
    while (child->parent == NULL){
        child = child->previous;
        if (child == NULL){
            return NULL;
        }
    }
    return child->parent;
}

varloc_node_t* var_node_get_child_at_index(varloc_node_t* parent, uint32_t index){
    varloc_node_t* child = parent->child;
    for (uint_fast32_t i = 0; i < index; i++){
        if(child != NULL){
            child = child->next;
        }
        else{
            return NULL;
        }
    }
    return child;
}

int var_node_get_child_index(varloc_node_t* child){
    int row_n = 0;
    while(child->parent == NULL){
        child = child->previous;
        row_n++;
        if (child == NULL){
            return -1;
        }
    }
    return row_n;
}


uint32_t var_node_get_address(varloc_node_t* node){
    uint64_t offset = node->address.offset_bits;
    varloc_node_t* parent = var_node_get_parent(node);
    while (parent != NULL){
        if (parent->address.base == 0){
            offset += parent->address.offset_bits;
            parent = var_node_get_parent(parent);
        }
        else{
            // top level variable with address
            offset += parent->address.base;
            return offset;
        }
    }
    return offset;
}

varloc_node_t* new_var_node(){
    varloc_node_t* ret = malloc(sizeof(*ret));
    if (ret == NULL){
        printf("varloc_node_t malloc failed! exiting...\n");
        exit(1);
    }
    else{
        memset(ret, 0, sizeof(*ret));
    }
    return ret;
}

varloc_node_t* new_child(varloc_node_t* parent){
    varloc_node_t* child = new_var_node();
    if (child == NULL){
        printf("varloc_node_t malloc failed! exiting...\n");
        exit(1);
    }
    else{
        // if parent already has child link to child next
        if (parent->child == NULL){
            parent->child = child;
            child->parent = parent;
        }
        else {
            varloc_node_t* node = parent->child;
            while(node->next != NULL){
                node = node->next;
            }
            node->next = child;
            child->previous = node;
        }
    }
    return child;
}

varloc_node_t* new_sibling(varloc_node_t* var){
    varloc_node_t* sibling = new_var_node();
    if (sibling == NULL){
        printf("varloc_node_t malloc failed! exiting...\n");
        exit(1);
    }
    else{
        var->next = sibling;
    }
    return sibling;
}


// hacked together some functions based on dwarves_fprintf.c

void parse_class(struct class *class, const struct cu *cu, varloc_node_t *node);

static void parse_type(struct tag *type, const struct cu *cu,
                       char* name, varloc_node_t* node);

static void parse_member(struct class_member *member, bool union_member,
                         struct tag *type, const struct cu *cu,
                         struct conf_fprintf *conf, varloc_node_t* node);

static void parse_union(struct type *type, const struct cu *cu,
                        const struct conf_fprintf *conf, varloc_node_t* node);

static void parse_array(const struct tag *tag,
                        const struct cu *cu, const char *name,
                        const struct conf_fprintf *conf, varloc_node_t* node);

void parse_extvar(struct variable *var, struct cu *cu);


static struct conf_fprintf conf = {
	.emit_stats = 1,
};

static struct conf_load conf_load = {
	.conf_fprintf = &conf,
    .get_addr_info = 1,
    .extra_dbg_info = 1,
};


static int cu_extvar_iterator(struct cu *cu, void *cookie __maybe_unused)
{
	struct tag *pos;
	uint32_t id;

	cu__for_each_variable(cu, id, pos) {
		struct variable *var = tag__variable(pos);
		if (var->external)
            parse_extvar(var,cu);
	}
	return 0;
}


void parse_extvar(struct variable *gvar, struct cu *cu){
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

    //    ++tag->recursivity_level;


    if (tag->tag == DW_TAG_variable){
        const struct variable *var = tag__variable(tag);
        const char *name = variable__name(var);

        varloc_node_t *var_node = new_var_node();
        const char *type_name = variable__type_name(var, cu, var_node->ctype_name, 100);

        const struct tag *type_tag = cu__type(cu, var->ip.tag.type);
        int base_type = tag__is_base_type(type_tag, cu);
        printf("\n\ngot var %s %s %d %x : ",
               name,
               type_name,
               base_type,
               var->ip.addr);



        if (last_var_node != NULL){
            last_var_node->next = var_node;
            var_node->previous = last_var_node;
        }
        else{
            tree_base = var_node;
        }
        last_var_node = var_node;

//        if (!base_type){
//            printf("\n");
            parse_type(type_tag, cu, NULL, var_node);
//        }
//        else{
//            var_node->var_type = BASE;
//        }
        strcpy(var_node->name, name);
//        var_node->name = name;
        var_node->address.base = var->ip.addr;
//        var_node->address.base = var->ip;
    }
}


static void parse_union(struct type *type, const struct cu *cu,
                             const struct conf_fprintf *conf, varloc_node_t* node)
{
    struct class_member *pos;
    size_t printed = 0;
    int indent = conf->indent;
    struct conf_fprintf uconf;
    uint32_t initial_union_cacheline;
    uint32_t cacheline = 0; /* This will only be used if this is the outermost union */

//    if (indent >= (int)sizeof(tabs))
//        indent = sizeof(tabs) - 1;

//    if (conf->prefix != NULL)
//        printed += fprintf(fp, "%s ", conf->prefix);
//    printed += fprintf(fp, "union%s%s {\n", type__name(type) ? " " : "",
//                       type__name(type) ?: "");

    uconf = *conf;
    uconf.indent = indent + 1;

    /*
     * If structs embedded in unions, nameless or not, have a size which isn't
     * isn't a multiple of the union size, then it must be packed, even if
     * it has no holes nor padding, as an array of such unions would have the
     * natural alignments of non-multiple structs inside it broken.
     */
    union__infer_packed_attributes(type, cu);

    /*
     * We may be called directly or from tag__fprintf, so keep sure
     * we keep track of the cacheline we're in.
     *
     * If we're being called from an outer structure, i.e. union within
     * struct, class or another union, then this will already have a
     * value and we'll continue to use it.
     */
    if (uconf.cachelinep == NULL)
        uconf.cachelinep = &cacheline;
    /*
     * Save the cacheline we're in, then, after each union member, get
     * back to it. Else we'll end up showing cacheline boundaries in
     * just the first of a multi struct union, for instance.
     */
    initial_union_cacheline = *uconf.cachelinep;
    type__for_each_member(type, pos) {

        struct tag *pos_type = cu__type(cu, pos->tag.type);

        if (pos_type == NULL) {
//            printed += fprintf(fp, "%.*s", uconf.indent, tabs);
//            printed += tag__id_not_found_fprintf(fp, pos->tag.type);
            continue;
        }

        uconf.union_member = 1;
        printf("%.*s", uconf.indent, tabs);
        parse_member(pos, true, pos_type, cu, &uconf, node);
//        fputc('\n', fp);
        ++printed;
        *uconf.cachelinep = initial_union_cacheline;
    }

//    return printed + fprintf(fp, "%.*s}%s%s", indent, tabs,
//                             conf->suffix ? " " : "", conf->suffix ?: "");
}


static void parse_array(const struct tag *tag,
                        const struct cu *cu, const char *name,
                        const struct conf_fprintf *conf, varloc_node_t* node)
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

    parse_type(type, cu, name, node);
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

            if (at->nr_entries[i] != 0 || !conf->last_member || single_member || conf->union_member){
                printf("[%u]", at->nr_entries[i]);
                node->n_items = at->nr_entries[i];
            }

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

        if (flat_dimensions != 0 || !conf->last_member || single_member || conf->union_member){
            printf("[%llu]", flat_dimensions);
            node->n_items = flat_dimensions;
        }
        else
            printf("[]");
    }

    return;
}

static void parse_member(struct class_member *member, bool union_member,
                                    struct tag *type, const struct cu *cu,
                                    struct conf_fprintf *conf, varloc_node_t *node)
{
    printf("\n");
    printf("%*s%s", indent*4, "", "- ");
    const int size = member->byte_size;
    int member_alignment_printed = 0;
    struct conf_fprintf sconf = *conf;
    uint32_t offset = member->byte_offset;
    size_t printed = 0, printed_cacheline = 0;
    const char *cm_name = class_member__name(member);
//        *name = cm_name;

    if (!sconf.rel_offset) {
        offset += sconf.base_offset;
        if (!union_member)
            sconf.base_offset = offset;
    }

    printf("size %d offset %d bitoffset %d ", size, offset, member->bitfield_offset);
    if (member->bitfield_offset < 0)
        offset += member->byte_size;

//    if (!conf->suppress_comments)
//        printed_cacheline = class__fprintf_cacheline_boundary(conf, offset, fp);

    if (member->tag.tag == DW_TAG_inheritance) {
        cm_name = "<ancestor>";
        printf("/* ");
    }

    if (member->is_static)
        printf("static ");

    /* For struct-like constructs, the name of the member cannot be
     * conflated with the name of its type, otherwise __attribute__ are
     * printed in the wrong order.
     */

    varloc_node_t *child = new_child(node);
    child->address.offset_bits = (offset * 8) + member->bitfield_offset;
    child->address.size_bits = (member->bitfield_size) ? (member->bitfield_size) : (size * 8);

    if (tag__is_union(type) || tag__is_struct(type) ||
        tag__is_enumeration(type))
    {
        parse_type(type, cu, NULL, child);
        if (cm_name) {
            if (!type__name(tag__type(type))){
                printf(" ");
            }
            strcpy(child->name, cm_name);
//            child->name = cm_name;
            printf("%s", cm_name);
        }
    } else {
        parse_type(type, cu, cm_name, child);
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


static void parse_type(struct tag *type, const struct cu *cu, char* name, varloc_node_t* node){
    if (name == NULL){
        name = "\0";
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
    node->address.size_bits = tag__size(type, cu);
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
            node->var_type = POINTER;
            strcpy(node->name, name);
//            node->name = name;
            name = namebf;
            printf("POINTER! ");
//            printf("POINTER! %s ", name);
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
//            if (typedef_expanded)
//                printf(" -> %s", type__name(ctype));
//            else {
            if(!typedef_expanded){
                strcpy(node->ctype_name, type__name(ctype));
                printf("%s ", type__name(ctype));
            }
            typedef_expanded++;
            type_type = cu__type(cu, type->type);
            if (type_type == NULL)
                return;
            n = tag__has_type_loop(type, type_type, NULL, 0, stdout);
            if (n)
                return;
            type = type_type;
        }
        if (typedef_expanded){
//            printf(" */ ");
        }
    }


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
        if ((node->var_type != POINTER)
//        &&  (!(*node->name))
        ){
            strcpy(node->name, name);
//           node->name = name;
        }
        if(!(*node->ctype_name)){
           tag__name(type, cu, node->ctype_name, sizeof(node->ctype_name), &tconf);
        }
        printf("%s %s",tag__name(type, cu, tbf, sizeof(tbf), &tconf),
                        name);
        break;
    case DW_TAG_subroutine_type:
//        printed += ftype__fprintf(tag__ftype(type), cu, name, 0, 0,
//                                  tconf.type_spacing, true, &tconf, fp);
        printf("%s ", name);
        printf("ftype__fprintf");
        break;
    case DW_TAG_atomic_type:
        modifier = "_Atomic";
        goto print_modifier;
    case DW_TAG_const_type:
        modifier = "const";
    print_modifier: {
        struct tag *ttype = cu__type(cu, type->type);
        if (ttype) {
            type = ttype;
            goto next_type;
        }
    }
        goto print_default;

    case DW_TAG_array_type:        
        node->var_type = ARRAY;
        parse_array(type, cu, name, &tconf, node);
        break;
    case DW_TAG_string_type:
        printf("string_type__fprintf");
        break;
    case DW_TAG_class_type:
    case DW_TAG_structure_type:
        ctype = tag__type(type);
        struct class *cclass = tag__class(type);
        if (node->var_type != POINTER){
            node->var_type = STRUCT;
            if(*name){
                printf("%s ", name);
                strcpy(node->name, name);
//                node->name = name;
            }
        }
        parse_class(cclass, cu, node);
        break;
    case DW_TAG_union_type:
        if (node->var_type != POINTER){
            node->var_type = UNION;
            if(*name){
                printf("%s ", name);
                strcpy(node->name, name);
//                node->name = name;
            }
        }
        ctype = tag__type(type);
        parse_union(ctype,cu, &tconf, node);
        break;
//    case DW_TAG_base_type:
//        node->var_type = BASE;
//        struct base_type* base = tag__base_type(type);
//        node->is_signed =  base->is_signed;
//        node->type_size =  base->bit_size;
//        strcpy(node->name, name);
////        node->name = name;
//        break;

    case DW_TAG_enumeration_type:
        node->var_type = ENUM;
        ctype = tag__type(type);
        if (type__name(ctype) != NULL){
            printf("enum %s", type__name(ctype), name ?: "");
        }
        else{
//            printed += enumeration__fprintf(type, &tconf, fp);
            printf("%s ", name);
            strcpy(node->name, name);
//            node->name = name;
            printf("enumeration__fprintf");

        }
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

void parse_class(struct class *class, const struct cu *cu, varloc_node_t *node)
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
    struct conf_fprintf cconf = {};// : conf_fprintf__defaults;
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
    printf("\n>>>");
    indent++;
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
        if (pos_type != NULL){
            strcpy(node->name, type__name(tag__type(pos_type)));
//            node->name = type__name(tag__type(pos_type));
            printf(" %s",
                 type__name(tag__type(pos_type)));
        }
        else{
            printf("Class tag not found!");
//            tag__id_not_found_fprintf(fp, tag_pos->type);
        }
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
                parse_type(pos_type, cu, NULL, node);
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
        parse_member(pos, false, pos_type, cu, &cconf, node);
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
                parse_type(tag_pos, cu, NULL, node);
                printf(":%u;\n", bit_size);
            }
        }
    }

//    if (!cconf.show_only_data_members)
//        class__vtable_fprintf(class, &cconf, fp);
    printf("\n<<<");
    indent--;
}


varloc_node_t* varloc_open_elf(char* file){
    tree_base = NULL;
    last_var_node = NULL;

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

    int err = cus__load_file(cus, &conf_load, file);
	if (err != 0) {
        cus__fprintf_load_files_err(cus, "pglobal", file, err, stderr);
        goto out_cus_delete;
    }

    cus__for_each_cu(cus, cu_extvar_iterator, NULL, NULL);

out_cus_delete:
    cus__delete(cus);
out_dwarves_exit:
    dwarves__exit();
out:
    return tree_base;
}
