#include "moarvm.h"

/* An array mapping representation IDs to function tables. */
static MVMREPROps **repr_registry = NULL;

/* Number of representations registered so far. */
static MVMuint32 num_reprs = 0;

/* Hash mapping representation names to IDs. */
/* XXX TODO */

/* Default REPR function handlers. */
MVM_NO_RETURN
static void die_no_attrs(MVMThreadContext *tc, MVMString *repr_name) {
    MVM_exception_throw_adhoc(tc,
        "This representation does not support attribute storage");
}
static MVMObject * default_get_attribute_boxed(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMObject *class_handle, MVMString *name, MVMint64 hint) {
    die_no_attrs(tc, st->REPR->name);
}
static void * default_get_attribute_ref(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMObject *class_handle, MVMString *name, MVMint64 hint) {
    die_no_attrs(tc, st->REPR->name);
}
static void default_bind_attribute_boxed(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMObject *class_handle, MVMString *name, MVMint64 hint, MVMObject *value) {
    die_no_attrs(tc, st->REPR->name);
}
static void default_bind_attribute_ref(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMObject *class_handle, MVMString *name, MVMint64 hint, void *value) {
    die_no_attrs(tc, st->REPR->name);
}
static MVMint32 default_is_attribute_initialized(MVMThreadContext *tc, MVMSTable *st, void *data, MVMObject *class_handle, MVMString *name, MVMint64 hint) {
    die_no_attrs(tc, st->REPR->name);
}
static MVMint64 default_hint_for(MVMThreadContext *tc, MVMSTable *st, MVMObject *class_handle, MVMString *name) {
    return MVM_NO_HINT;
}
static void default_set_int(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMint64 value) {
    MVM_exception_throw_adhoc(tc,
        "This representation cannot box a native int");
}
static MVMint64 default_get_int(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data) {
    MVM_exception_throw_adhoc(tc,
        "This representation cannot unbox to a native int");
}
static void default_set_num(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMnum64 value) {
    MVM_exception_throw_adhoc(tc,
        "This representation cannot box a native num");
}
static MVMnum64 default_get_num(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data) {
    MVM_exception_throw_adhoc(tc,
        "This representation cannot unbox to a native num");
}
static void default_set_str(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMString *value) {
    MVM_exception_throw_adhoc(tc,
        "This representation cannot box a native string");
}
static MVMString * default_get_str(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data) {
    MVM_exception_throw_adhoc(tc,
        "This representation cannot unbox to a native string");
}
static void * default_get_boxed_ref(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMuint32 repr_id) {
    MVM_exception_throw_adhoc(tc,
        "This representation cannot unbox to other types");
}
MVM_NO_RETURN
static void die_no_pos(MVMThreadContext *tc, MVMString *repr_name) {
    MVM_exception_throw_adhoc(tc,
        "This representation does not support positional access");
}
static void * default_at_pos_ref(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMuint64 index) {
    die_no_pos(tc, st->REPR->name);
}
static MVMObject * default_at_pos_boxed(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMuint64 index) {
    die_no_pos(tc, st->REPR->name);
}
static void default_bind_pos_ref(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMuint64 index, void *addr) {
    die_no_pos(tc, st->REPR->name);
}
static void default_bind_pos_boxed(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMuint64 index, MVMObject *obj) {
    die_no_pos(tc, st->REPR->name);
}
static MVMuint64 default_elems(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data) {
    die_no_pos(tc, st->REPR->name);
}
static void default_preallocate(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMuint64 count) {
    die_no_pos(tc, st->REPR->name);
}
static void default_trim_to(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMuint64 count) {
    die_no_pos(tc, st->REPR->name);
}
static void default_make_hole(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMuint64 at_index, MVMuint64 count) {
    die_no_pos(tc, st->REPR->name);
}
static void default_delete_elems(MVMThreadContext *tc, MVMSTable *st, MVMObject *root, void *data, MVMuint64 at_index, MVMuint64 count) {
    die_no_pos(tc, st->REPR->name);
}
static MVMSTable * default_get_elem_stable(MVMThreadContext *tc, MVMSTable *st) {
    die_no_pos(tc, st->REPR->name);
}

/* Set default attribute functions on a REPR that lacks them. */
static void add_default_attr_funcs(MVMThreadContext *tc, MVMREPROps *repr) {
    repr->attr_funcs = malloc(sizeof(MVMREPROps_Attribute));
    repr->attr_funcs->get_attribute_boxed = default_get_attribute_boxed;
    repr->attr_funcs->get_attribute_ref = default_get_attribute_ref;
    repr->attr_funcs->bind_attribute_boxed = default_bind_attribute_boxed;
    repr->attr_funcs->bind_attribute_ref = default_bind_attribute_ref;
    repr->attr_funcs->is_attribute_initialized = default_is_attribute_initialized;
    repr->attr_funcs->hint_for = default_hint_for;
}

/* Set default boxing functions on a REPR that lacks them. */
static void add_default_box_funcs(MVMThreadContext *tc, MVMREPROps *repr) {
    repr->box_funcs = malloc(sizeof(MVMREPROps_Boxing));
    repr->box_funcs->set_int = default_set_int;
    repr->box_funcs->get_int = default_get_int;
    repr->box_funcs->set_num = default_set_num;
    repr->box_funcs->get_num = default_get_num;
    repr->box_funcs->set_str = default_set_str;
    repr->box_funcs->get_str = default_get_str;
    repr->box_funcs->get_boxed_ref = default_get_boxed_ref;
}

/* Set default positional functions on a REPR that lacks them. */
static void add_default_pos_funcs(MVMThreadContext *tc, MVMREPROps *repr) {
    repr->idx_funcs = malloc(sizeof(MVMREPROps_Positional));
    repr->idx_funcs->at_pos_ref = default_at_pos_ref;
    repr->idx_funcs->at_pos_boxed = default_at_pos_boxed;
    repr->idx_funcs->bind_pos_ref = default_bind_pos_ref;
    repr->idx_funcs->bind_pos_boxed = default_bind_pos_boxed;
    repr->idx_funcs->elems = default_elems;
    repr->idx_funcs->preallocate = default_preallocate;
    repr->idx_funcs->trim_to = default_trim_to;
    repr->idx_funcs->make_hole = default_make_hole;
    repr->idx_funcs->delete_elems = default_delete_elems;
    repr->idx_funcs->get_elem_stable = default_get_elem_stable;
}

/* Registers a representation. It this is ever made public, it should first be
 * made thread-safe. */
static void register_repr(MVMThreadContext *tc, MVMString *name, MVMREPROps *repr) {
    MVMuint32 ID = num_reprs;
    num_reprs++;
    if (repr_registry)
        repr_registry = realloc(repr_registry, num_reprs * sizeof(MVMREPROps *));
    else
        repr_registry = malloc(num_reprs * sizeof(MVMREPROps *));
    repr_registry[ID] = repr;
    /* XXX Add to hash also. */
    repr->ID = ID;
    repr->name = name;
    if (!repr->attr_funcs)
        add_default_attr_funcs(tc, repr);
    if (!repr->box_funcs)
        add_default_box_funcs(tc, repr);
    if (!repr->idx_funcs)
        add_default_pos_funcs(tc, repr);
}

/* Initializes the representations registry, building up all of the various
 * representations. */
void MVM_repr_initialize_registry(MVMThreadContext *tc) {
    /* XXX Initialize name to ID map. */
    
    /* Add all core representations. */
    register_repr(tc,
        MVM_string_ascii_decode_nt(tc, tc->instance->boot_types->BOOTStr, "MVMString"), 
        MVMString_initialize(tc));
}

/* Get a representation's ID from its name. Note that the IDs may change so
 * it's best not to store references to them in e.g. the bytecode stream. */
MVMuint32 MVM_repr_name_to_id(MVMThreadContext *tc, MVMString *name) {
    MVM_panic("MVM_repr_name_to_id");
}

/* Gets a representation by ID. */
MVMREPROps * MVM_repr_get_by_id(MVMThreadContext *tc, MVMuint32 id) {
    return repr_registry[id];
}

/* Gets a representation by name. */
MVMREPROps * MVM_repr_get_by_name(MVMThreadContext *tc, MVMString *name) {
    return repr_registry[MVM_repr_name_to_id(tc, name)];
}