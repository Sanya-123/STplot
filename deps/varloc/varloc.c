#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dwarves.h"
#include "varloc.h"


char* varloc_node_types[] = {
    "Base",
    "Struct",
    "Enum",
    "Pointer",
    "Union",
    "Array"
};


void for_each_var_loop(varloc_node_t* root, void(*func)(varloc_node_t*)){
    if (root == NULL){
        return;
    }
    if (root->child != NULL){
        for_each_var_loop(root->child, func);
    }
    if (root->next != NULL){
        for_each_var_loop(root->next, func);
    }
    func(root);
}

void varloc_delete_tree(varloc_node_t* root){
    if (root == NULL){
        return;
    }
    for_each_var_loop(root, free);
}



void print_var_node(varloc_node_t* var){
//    for (int i = 0; i < var_loop_level; i++){
//        printf("  ");
//    }
    printf("%s %s %s base:%x off:%d size:%d items:%d\n",
           varloc_node_types[var->var_type],
           var->name,
           var->ctype_name,
           var->address.base,
           var->address.offset_bits,
           var->address.size_bits,
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



int get_char_index(char* string, char c) {
    char *e = strchr(string, c);
    if (e == NULL) {
        return -1;
    }
    return (int)(e - string);
}


/*
 * Given variable name "a.b.c.d" composed of node names finds and
 * returns "d" node pointer or NULL if not found.
 * Any node in the link chain ("a", "b", "c") can be provided as root parameter
 *
 * Will also work if there are two nodes with the same name on one or multiple levels
 * (case of anonymous structs with "_")
 *
 * Creates one recursive call for each level ("." symbol in name)
 *
*/
varloc_node_t* var_node_get_by_name(varloc_node_t* root, char* name){
    int idx = get_char_index(name, '.');
    while(root != NULL){
        if (root->var_type == ARRAY){
            varloc_node_t* res = var_node_get_by_name(root->child, name);
            if (res != NULL){
                return res;
            }
        }
        if (idx < 0){
            if(strcmp(name, root->name) == 0){
                return root;
            }
        }
        else{
            if(strncmp(name, root->name, idx) == 0){
                varloc_node_t* res = var_node_get_by_name(root->child, name+idx+1);
                if (res != NULL){
                    return res;
                }
            }
        }
        root = root->next;
    }
    return NULL;
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
            //reach tree root
            return row_n;
        }
    }
    return row_n;
}

uint32_t var_node_get_address(varloc_node_t* node){
    uint32_t offset = node->address.base + (node->address.offset_bits / 8);
    varloc_node_t* parent = var_node_get_parent(node);
    while (parent != NULL){
        if (parent->var_type == POINTER){
            return 0xFFFFFFFF;
        }
        else if (parent->address.base == 0){
            offset += parent->address.offset_bits / 8;
            parent = var_node_get_parent(parent);
        }
        else{
            // top level variable with address
            offset += parent->address.base;
            return offset;
        }
    }
    return offset ;
}


varloc_location_t var_node_get_load_location(varloc_node_t* node){
    varloc_location_t loc = {0};
    if (node){
        varloc_node_t* parent = var_node_get_parent(node);
        if (!parent){
            parent = node;
        }
        uint32_t byte_address = var_node_get_address(parent);
        loc.address.base = (byte_address + (node->address.offset_bits / 8)) & 0xFFFFFFFC;
        loc.address.offset_bits = (((byte_address - loc.address.base) * 8) + node->address.offset_bits ) % 32;
        // loc.address.offset_bits = node->address.offset_bits % 32;
        loc.address.size_bits = node->address.size_bits;
        if (node->is_float){
            loc.type =  VARLOC_FLOAT;
        }
        else if (node->is_signed){
            loc.type = VARLOC_SIGNED;
        }
    }
    return loc;
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
    return child;
}

varloc_node_t* new_sibling(varloc_node_t* var){
    varloc_node_t* sibling = new_var_node();
    var->next = sibling;
    sibling->previous = var;
    return sibling;
}


// hacked together some functions based on dwarves_fprintf.c

void parse_class(struct class *class, const struct cu *cu, varloc_node_t *node);

static void parse_type(struct tag *type, const struct cu *cu,
                       const char* name, varloc_node_t* node);

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
    .emit_stats = 0,
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
        if (var->external || var->has_specification){
            parse_extvar(var,cu);
        }
	}
	return 0;
}


void parse_extvar(struct variable *gvar, struct cu *cu){
    uint32_t count = 0;
    struct tag *tag;

    if (gvar == NULL)
        return;

    if (gvar->has_specification){
        // get address of extern variables
        varloc_node_t *spec_node = last_var_node;
        while (spec_node){
            if (!strcmp(gvar->spec->name, spec_node->name)){
                spec_node->address.base = gvar->ip.addr;
                break;
            }
            spec_node = spec_node->previous;
        }
        return;
    }
    else if (gvar->declaration){
        // get extern variables
        varloc_node_t *spec_node = last_var_node;
        while (spec_node){
            if (!strcmp(gvar->name, spec_node->name)){
                // refuse allready collected nodes
                return;
            }
            spec_node = spec_node->previous;
        }
    }

    tag = &gvar->ip.tag;
    struct conf_fprintf cfg = {0};
    if (tag->tag == DW_TAG_variable){
        const char *name = variable__name(gvar);
        varloc_node_t *var_node = new_var_node();
        const char *type_name = variable__type_name(gvar, cu, var_node->ctype_name, 100);
        struct tag *type_tag = cu__type(cu, gvar->ip.tag.type);

        var_node->address.base = gvar->ip.addr;
        parse_type(type_tag, cu, name, var_node);
        strncpy(var_node->name, name, sizeof(var_node->name));
        // array rename
        if (var_node->var_type == ARRAY){
            char buf[20] = {};
            strcpy(var_node->name, name);
            sprintf(buf, "[%d]", var_node->n_items);
            strcat(var_node->name, buf);
        }

        if (last_var_node != NULL){
            last_var_node->next = var_node;
            var_node->previous = last_var_node;
        }
        else{
            tree_base = var_node;
        }
        last_var_node = var_node;


        var_node->is_anon = 0;
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
//        printf("%.*s", uconf.indent, tabs);
        parse_member(pos, true, pos_type, cu, &uconf, node);
//        fputc('\n', fp);
        ++printed;
        *uconf.cachelinep = initial_union_cacheline;
    }
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

    // varloc_node_t* member = new_child(node);
    // parse_type(type, cu, name, member);

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
//                printf("[%u]", at->nr_entries[i]);
                node->n_items = at->nr_entries[i];
            }

//            else
//                printf("[]");
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
//            printf("[%llu]", flat_dimensions);
            node->n_items = flat_dimensions;
        }
//        else
//            printf("[]");
    }

    // array expension and rename
    char buf[20] = {};
    strcpy(node->name, name);
    sprintf(buf, "[%d]", node->n_items);
    strcat(node->name, buf);

    uint32_t remain_n = 0;
    if (node->n_items > 100){
        remain_n = 100;
    }
    else if(node->n_items > 1){
        remain_n = node->n_items;
    }
    else{
        remain_n = 1;
    }
    for (uint32_t i = 0; i < remain_n; i++){
        varloc_node_t* member = new_child(node);
        parse_type(type, cu, name, member);
        member->address.offset_bits += (member->address.size_bits * (i));
        strcpy(member->name, name);
        sprintf(buf, "[%d]", i);
        strcat(member->name, buf);
        member = member->next;
    };



    return;
}

static void parse_member(struct class_member *member, bool union_member,
                                    struct tag *type, const struct cu *cu,
                                    struct conf_fprintf *conf, varloc_node_t *node)
{
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

//    printf("size %d offset %d bitoffset %d ", size, offset, member->bitfield_offset);
    if (member->bitfield_offset < 0)
        offset += member->byte_size;

//    if (member->is_static)
//        printf("static ");

    /* For struct-like constructs, the name of the member cannot be
     * conflated with the name of its type, otherwise __attribute__ are
     * printed in the wrong order.
     */

    varloc_node_t *child = new_child(node);
    child->address.offset_bits = (offset * 8) + member->bitfield_offset;

    if (tag__is_union(type) || tag__is_struct(type) ||
        tag__is_enumeration(type))
    {
        parse_type(type, cu, NULL, child);
        if (cm_name) {
            strncpy(child->name, cm_name, sizeof(child->name));
        }
    } else {
        parse_type(type, cu, cm_name, child);
    }

    if (member->is_static) {
        if (member->const_value != 0)
            printf(" = %", member->const_value);
    } else if (member->bitfield_size != 0) {
        child->address.size_bits = member->bitfield_size;
    }

    return;
}


static void parse_type(struct tag *type, const struct cu *cu, const char* name, varloc_node_t* node){
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
    node->address.size_bits = tag__size(type, cu) * 8;
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
            name = namebf;
            strncpy(node->name, name, sizeof(node->name));
//            printf("POINTER! ");
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
            if(!typedef_expanded){
                strncpy(node->ctype_name, type__name(ctype), sizeof(node->ctype_name));
//                printf("%s ", type__name(ctype));
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

        if (type->type != 0) {
            int n;
            struct tag *ptype = cu__type(cu, type->type);
            if (ptype == NULL)
                return;
            n = tag__has_type_loop(type, ptype, NULL, 0, stdout);
            if (n)
                return;
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
        ){
            strncpy(node->name, name, sizeof(node->name));
        }
        if(!(*node->ctype_name)){
           tag__name(type, cu, node->ctype_name, sizeof(node->ctype_name), &tconf);
        }
        break;
    case DW_TAG_subroutine_type:
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
        break;
    case DW_TAG_class_type:
    case DW_TAG_structure_type:
        ctype = tag__type(type);
        struct class *cclass = tag__class(type);
        if (node->var_type != POINTER){
            node->var_type = STRUCT;
            if(*name){
                strcpy(node->name, name);
            }
            else{
                sprintf(node->name, "_");
                node->is_anon = 1;
            }
        }
        parse_class(cclass, cu, node);
        break;
    case DW_TAG_union_type:
        if (node->var_type != POINTER){
            node->var_type = UNION;
            if(*name){
                strncpy(node->name, name, sizeof(node->name));
            }
            else{
                sprintf(node->name, "union ...");
                node->is_anon = 1;
            }
        }
        ctype = tag__type(type);
        parse_union(ctype,cu, &tconf, node);
        break;
    case DW_TAG_base_type:
        node->var_type = BASE;
        strncpy(node->name, name, sizeof(node->name));
        if(!(*node->ctype_name)){
            tag__name(type, cu, node->ctype_name, sizeof(node->ctype_name), &tconf);
        }
        struct base_type* base = tag__base_type(type);
        node->is_signed =  base->is_signed;
        if (base->float_type){
            node->is_float = 1;
        }
        break;

    case DW_TAG_enumeration_type:
        node->var_type = ENUM;
        ctype = tag__type(type);
        if (type__name(ctype) != NULL){
//            printf("enum %s", type__name(ctype), name ?: "");
            strcpy(node->name, name);
        }
        else{
//            printed += enumeration__fprintf(type, &tconf, fp);
//            printf("%s ", name);
            strncpy(node->name, name, sizeof(node->name));
//            node->name = name;
//            printf("enumeration__fprintf");

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
    struct conf_fprintf cconf = {};// : conf_fprintf__defaults;

    class__infer_packed_attributes(class, cu);

    /* First look if we have DW_TAG_inheritance */
//    printf("\n>>>");
    indent++;
    type__for_each_tag(type, tag_pos) {

//        const char *accessibility;

        if (tag_pos->tag != DW_TAG_inheritance){
            continue;
        }

        pos = tag__class_member(tag_pos);

        struct tag *pos_type = cu__type(cu, tag_pos->type);
        if (pos_type != NULL){
            strncpy(node->name, type__name(tag__type(pos_type)), sizeof(node->name));
        }
    }


    type__for_each_tag(type, tag_pos) {

        if (tag_pos->tag != DW_TAG_member &&
            tag_pos->tag != DW_TAG_inheritance) {
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
                    continue;
                }
                /*
                 * Now check if this isn't something like 'unsigned :N' with N > 0,
                 * i.e. _explicitely_ adding a bit hole.
                 */
                if (last->byte_offset != pos->byte_offset) {
                    bitfield_size = 0;
                }
                parse_type(pos_type, cu, NULL, node);
            }
        }

        struct tag *pos_type = cu__type(cu, pos->tag.type);
        if (pos_type == NULL) {
            continue;
        }

        cconf.last_member = list_is_last(&tag_pos->node, &type->namespace.tags);
        cconf.first_member = last == NULL;

        size = pos->byte_size;
        parse_member(pos, false, pos_type, cu, &cconf, node);

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
//            int bit_size = size * 8;

            for (added_padding = 0; added_padding < class->padding; added_padding += size) {
                parse_type(tag_pos, cu, NULL, node);
            }
        }
    }

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
    // "remove" top level nodes with no address
    varloc_node_t* node = tree_base;
    while(node){
        if(node->address.base == 0){
            if(node->previous){
                node->previous->next = node->next;
            }
            if (node->next){
                node->next->previous = node->previous;
            }
        }
        node = node->next;
    }

    return tree_base;
}
