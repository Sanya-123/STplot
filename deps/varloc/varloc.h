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
 }varloc_type_t;

 typedef struct varloc_s{
     struct varloc_s*  sibling;
     struct varloc_s*  child;
     char*      name;
     char*      ctype;
     varloc_type_t  var_type;
     varloc_address_t   address;
 }varloc_node_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int varloc(char* file);

#ifdef __cplusplus
}
#endif

#endif /* INC_VARLOC_H_ */
