/**
 ******************************************************************************
 * @file    varloc.h
 * @author  kasper
 * @date    2023-Jul-13
 * @brief   Description
 ******************************************************************************
 */
#ifndef INC_VARLOC_H_
#define INC_VARLOC_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported includes ---------------------------------------------------------*/
#include <stdint.h>
/* Exported types ------------------------------------------------------------*/
 typedef struct{
     uint32_t base;
     uint32_t offset_bits;
     uint32_t size_bits;
 }varloc_address_t;

 typedef enum{
     BASE,
     STRUCT,
     ENUM,
     POINTER,
     UNION,
     ARRAY,
 }varloc_node_type_t;

 typedef struct varloc_s{
     struct varloc_s*   next;
     struct varloc_s*   previous;
     struct varloc_s*   child;
     struct varloc_s*   parent;
     char*              name;
     char               ctype_name[100];
     varloc_node_type_t var_type;
     varloc_address_t   address;
     uint32_t           n_items;
     uint8_t            type_size;
     uint8_t            is_signed;
 }varloc_node_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

 // parse elf file for static variables and return root node
 varloc_node_t*  varloc_open_elf(char* file);

 varloc_node_t* new_var_node();

 varloc_node_t* var_node_get_parent(varloc_node_t* child);

 char* var_node_get_type_name(varloc_node_t* node);

 uint32_t var_node_get_address(varloc_node_t* node);

 void varloc_delete_tree(varloc_node_t* root);


#ifdef __cplusplus
}
#endif

#endif /* INC_VARLOC_H_ */
