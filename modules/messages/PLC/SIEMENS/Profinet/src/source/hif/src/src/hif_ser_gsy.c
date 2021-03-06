/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_ser_gsy.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Automatically generated with HIF Code Generator v2.1.0                   */
/*                                                                           */
/*                                                                           */
/*  !!!ATTENTION: Do not edit this file.                                     */
/*                Please use the HIF Code Generator to generate this file.   */
/*                                                                           */
/*  The hash value was automatically calculated by HIF Code Generator!       */
/*  HASH-VALUE: 1BnWhCwRa4jNggjS6YjeawQu4P5xoe5EoKhj7ktUOHU=                 */
/*****************************************************************************/

#if defined( TOOL_CHAIN_MICROSOFT ) /* Microsoft Visual C++ */
#pragma component(browser, off, references)
#endif

#define HIF_MODULE_ID		34
#define LTRC_ACT_MODUL_ID	34

#include "hif_int.h"		

HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)

#if ( (HIF_CFG_COMPILE_SERIALIZATION == 1) && (HIF_CFG_USE_GSY == 1) )


/* Prototypes */


/* Helper Functions */

/****************************************************************************
*  Name:        hif_gsy_serialize_upper()
*  Purpose:     Function serializes the RQB to the HIF shared memory - 
*               according to the component ID and OP-code in the RQB.
*  Input:       pRqbSrc - pointer to the RQB, stored in the hosts local memory
*               pPipe   - pointer to pipe management structure
*  Output:      ppRqbDest - pointer to the RQB in the HIF shared memory
*  Return:      HIF_OK
*               HIF_ERR_RESOURCE
****************************************************************************/
LSA_UINT16  hif_gsy_serialize_upper (
    HIF_SYS_HANDLE hSysDev,
    LSA_VOID*      pHifPipeHandle,
    LSA_INT        hShmPool,
    LSA_VOID*      pRqbSrc,
    LSA_VOID**     ppRqbDest,
    LSA_UINT32*    pSerRqbLen )
{
    LSA_UINT8* help_ptr = LSA_NULL;
    LSA_UINT32 rqb_size = 0;
    LSA_OPCODE_TYPE opcode = 0;
    GSY_RQB_TYPE* ptr_rqb_src = (GSY_RQB_TYPE*)pRqbSrc;
    LSA_VOID** ptr_ptr_rqb_dest = ppRqbDest;
    
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(pHifPipeHandle);

    HIF_ASSERT(HIF_IS_NOT_NULL(ptr_rqb_src)); /*#5*/
    opcode=HIF_RQB_GET_OPCODE(ptr_rqb_src);

    if (opcode==GSY_OPC_OPEN_CHANNEL)
    {
        rqb_size+=HIF_SER_SIZEOF(4);        /* original upper RQB pointer */
        rqb_size+=HIF_SER_SIZEOF_OPCODE_TYPE();        /* _opcode */
        rqb_size+=HIF_SER_SIZEOF_SYS_PATH_TYPE();        /* sys_path */
        HIF_ALLOC_SER_MEM((LSA_VOID**)(&help_ptr), rqb_size, hSysDev, hShmPool);
        HIF_PROGRAM_TRACE_04(0,LSA_TRACE_LEVEL_NOTE_LOW,"hif_gsy_serialize_upper() opc(%d) rqb_size_shm(%d) shm_addr(0x%08x) pRQB(0x%08x)",opcode,rqb_size,help_ptr,ptr_rqb_src);

        if (HIF_IS_NULL(help_ptr))
        {
            return HIF_ERR_RESOURCE;
        }

        *ptr_ptr_rqb_dest = help_ptr;
        HIF_SER_COPY_HTSHM_32((LSA_UINT32**)&help_ptr,ptr_rqb_src);                /* original upper RQB pointer */

        HIF_SER_OPCODE_HTSHM((LSA_UINT32**)&help_ptr,opcode);                /* _opcode */
        HIF_SER_SYS_PATH_HTSHM((LSA_UINT32**)&help_ptr,ptr_rqb_src->args.channel.sys_path);                /* sys_path */
    }
    else if (opcode==GSY_OPC_CLOSE_CHANNEL)
    {
        rqb_size+=HIF_SER_SIZEOF(4);        /* original upper RQB pointer */
        rqb_size+=HIF_SER_SIZEOF_OPCODE_TYPE();        /* _opcode */
        HIF_ALLOC_SER_MEM((LSA_VOID**)(&help_ptr), rqb_size, hSysDev, hShmPool);
        HIF_PROGRAM_TRACE_04(0,LSA_TRACE_LEVEL_NOTE_LOW,"hif_gsy_serialize_upper() opc(%d) rqb_size_shm(%d) shm_addr(0x%08x) pRQB(0x%08x)",opcode,rqb_size,help_ptr,ptr_rqb_src);

        if (HIF_IS_NULL(help_ptr))
        {
            return HIF_ERR_RESOURCE;
        }

        *ptr_ptr_rqb_dest = help_ptr;
        HIF_SER_COPY_HTSHM_32((LSA_UINT32**)&help_ptr,ptr_rqb_src);                /* original upper RQB pointer */

        HIF_SER_OPCODE_HTSHM((LSA_UINT32**)&help_ptr,opcode);                /* _opcode */
    }
    else
    {
        HIF_PROGRAM_TRACE_02(0,LSA_TRACE_LEVEL_FATAL,"hif_gsy_serialize_upper(). RQB-Opcode:0x%x is not valid! pRQB(0x%08x)",opcode,ptr_rqb_src+4);
        HIF_FATAL(0);
    }

    *pSerRqbLen = rqb_size;

    HIF_ASSERT(HIF_IS_NOT_NULL(help_ptr)); /*#4*/

    /* Check for overwrite */
    HIF_SHM_CHECK_OVERWRITE_SERIALIZE(opcode, *ptr_ptr_rqb_dest, rqb_size, help_ptr, "hif_gsy_serialize_upper");

    return HIF_OK;
}

/****************************************************************************
*  Name:        hif_gsy_serialize_lower()
*  Purpose:     Function serializes the RQB to the HIF shared memory - 
*               according to the component ID and OP-code in the RQB.
*  Input:       pRqbSrc - pointer to the RQB, stored in the hosts local memory
*               pPipe   - pointer to pipe management structure
*  Output:      ppRqbDest - pointer to the RQB in the HIF shared memory
*  Return:      HIF_OK
*               HIF_ERR_RESOURCE
****************************************************************************/
LSA_UINT16  hif_gsy_serialize_lower (
    HIF_SYS_HANDLE hSysDev,
    LSA_VOID*      pHifPipeHandle,
    LSA_INT        hShmPool,
    LSA_VOID*      pRqbSrc,
    LSA_VOID**     ppRqbDest,
    LSA_UINT32*    pSerRqbLen )
{
    LSA_UINT8* help_ptr = LSA_NULL;
    LSA_UINT32 rqb_size = 0;
    LSA_OPCODE_TYPE opcode = 0;
    GSY_RQB_TYPE* ptr_rqb_src = (GSY_RQB_TYPE*)pRqbSrc;
    LSA_VOID** ptr_ptr_rqb_dest = ppRqbDest;
    LSA_UINT16 freeRes = 0;
    
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(pHifPipeHandle);

    HIF_ASSERT(HIF_IS_NOT_NULL(ptr_rqb_src)); /*#5*/
    opcode=HIF_RQB_GET_OPCODE(ptr_rqb_src);

    if (opcode==GSY_OPC_OPEN_CHANNEL)
    {
        rqb_size+=HIF_SER_SIZEOF(4);        /* original upper RQB pointer */
        rqb_size+=HIF_SER_SIZEOF_OPCODE_TYPE();        /* _opcode */
        rqb_size+=HIF_SER_SIZEOF_RESPONSE_TYPE();        /* _response */
        HIF_ALLOC_SER_MEM((LSA_VOID**)(&help_ptr), rqb_size, hSysDev, hShmPool);
        HIF_PROGRAM_TRACE_04(0,LSA_TRACE_LEVEL_NOTE_LOW,"hif_gsy_serialize_lower() opc(%d) rqb_size_shm(%d) shm_addr(0x%08x) pRQB(0x%08x)",opcode,rqb_size,help_ptr,ptr_rqb_src);

        if (HIF_IS_NULL(help_ptr))
        {
            return HIF_ERR_RESOURCE;
        }

        *ptr_ptr_rqb_dest = help_ptr;
        HIF_SER_COPY_HTSHM_32((LSA_UINT32**)&help_ptr,*((LSA_UINT32*)ptr_rqb_src-1));                /* original upper RQB pointer */

        HIF_SER_OPCODE_HTSHM((LSA_UINT32**)&help_ptr,opcode);                /* _opcode */
        HIF_SER_RESPONSE_HTSHM((LSA_UINT32**)&help_ptr,HIF_RQB_GET_RESPONSE(ptr_rqb_src));                /* _response */

        /* Delete lower RQB after lower serialization */
        HIF_FREE_LOCAL_MEM(&freeRes,(LSA_UINT8*)ptr_rqb_src-HIF_SER_SIZEOF(4));
        if (freeRes != LSA_RET_OK)
        {
            return HIF_ERR_RESOURCE;
        }
    }
    else if (opcode==GSY_OPC_CLOSE_CHANNEL)
    {
        rqb_size+=HIF_SER_SIZEOF(4);        /* original upper RQB pointer */
        rqb_size+=HIF_SER_SIZEOF_OPCODE_TYPE();        /* _opcode */
        rqb_size+=HIF_SER_SIZEOF_RESPONSE_TYPE();        /* _response */
        HIF_ALLOC_SER_MEM((LSA_VOID**)(&help_ptr), rqb_size, hSysDev, hShmPool);
        HIF_PROGRAM_TRACE_04(0,LSA_TRACE_LEVEL_NOTE_LOW,"hif_gsy_serialize_lower() opc(%d) rqb_size_shm(%d) shm_addr(0x%08x) pRQB(0x%08x)",opcode,rqb_size,help_ptr,ptr_rqb_src);

        if (HIF_IS_NULL(help_ptr))
        {
            return HIF_ERR_RESOURCE;
        }

        *ptr_ptr_rqb_dest = help_ptr;
        HIF_SER_COPY_HTSHM_32((LSA_UINT32**)&help_ptr,*((LSA_UINT32*)ptr_rqb_src-1));                /* original upper RQB pointer */

        HIF_SER_OPCODE_HTSHM((LSA_UINT32**)&help_ptr,opcode);                /* _opcode */
        HIF_SER_RESPONSE_HTSHM((LSA_UINT32**)&help_ptr,HIF_RQB_GET_RESPONSE(ptr_rqb_src));                /* _response */

        /* Delete lower RQB after lower serialization */
        HIF_FREE_LOCAL_MEM(&freeRes,(LSA_UINT8*)ptr_rqb_src-HIF_SER_SIZEOF(4));
        if (freeRes != LSA_RET_OK)
        {
            return HIF_ERR_RESOURCE;
        }
    }
    else
    {
        HIF_PROGRAM_TRACE_02(0,LSA_TRACE_LEVEL_FATAL,"hif_gsy_serialize_lower(). RQB-Opcode:0x%x is not valid! pRQB(0x%08x)",opcode,ptr_rqb_src+4);
        HIF_FATAL(0);
    }

    *pSerRqbLen = rqb_size;

    HIF_ASSERT(HIF_IS_NOT_NULL(help_ptr)); /*#4*/

    /* Check for overwrite */
    HIF_SHM_CHECK_OVERWRITE_SERIALIZE(opcode, *ptr_ptr_rqb_dest, rqb_size, help_ptr, "hif_gsy_serialize_lower");

    return HIF_OK;
}

/****************************************************************************
*  Name:        hif_gsy_deserialize_upper()
*  Purpose:     De-serializes the RQB from the HIF shared memory into the
*               hosts local memory - according to the component ID and
*               OP-code in the RQB.
*  Input:       pRqbSrc - pointer to the RQB in shared memory
                pPipe   - pointer to pipe management structure
*  Output:      ppRqbDest - pointer to the RQB in the hosts local memory
*  Return:      HIF_OK
*               HIF_ERR_RESOURCE
****************************************************************************/
LSA_UINT16      hif_gsy_deserialize_upper (
        HIF_SYS_HANDLE hSysDev,
        LSA_VOID* pHifPipeHandle,
        LSA_VOID* pRqbSrc,
        LSA_VOID** ppRqbDest )
{
    LSA_UINT8* help_ptr = LSA_NULL;
    LSA_OPCODE_TYPE opcode = 0;
    LSA_VOID* ptr_rqb_src = (LSA_VOID*)pRqbSrc;
    GSY_RQB_TYPE** ptr_ptr_rqb_dest = (GSY_RQB_TYPE**)ppRqbDest;
    LSA_RESPONSE_TYPE response = 0;
    GSY_RQB_TYPE* new_rqb  = LSA_NULL;
    

    HIF_ASSERT(HIF_IS_NOT_NULL(ptr_ptr_rqb_dest)); /*#17*/
    HIF_DESER_GET_OPCODE(ptr_rqb_src, opcode);

    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(pHifPipeHandle);

    if (opcode==GSY_OPC_OPEN_CHANNEL)
    {
        help_ptr = (LSA_UINT8*)ptr_rqb_src + HIF_SER_SIZEOF(4);        /* start deserialization after the original upper RQB pointer */

        if (HIF_IS_NULL((LSA_UINT8*)ptr_rqb_src))
        {
            return HIF_ERR_RESOURCE;
        }
        else
        {
            new_rqb = (GSY_RQB_TYPE*)hif_get_rqb_id(ptr_rqb_src); /* get orig. upper RQB */
            HIF_PROGRAM_TRACE_03(0,LSA_TRACE_LEVEL_NOTE_LOW,"hif_gsy_deserialize_upper() opc(%d) shm_addr(0x%08x) pRQB(0x%08x)",opcode,ptr_rqb_src,new_rqb+4);
            *ptr_ptr_rqb_dest = new_rqb;

            /* restore the Upper RQB */
            HIF_RQB_SET_OPCODE(new_rqb,opcode);                        /* _opcode */
            help_ptr+=4;                        /* skip opcode on shared-mem */
            HIF_SER_RESPONSE_SHMTH(&response,(LSA_UINT32**)&help_ptr);                        /* _response */
            HIF_RQB_SET_RESPONSE(new_rqb,response);                                                /* _response */
        }
    }
    else if (opcode==GSY_OPC_CLOSE_CHANNEL)
    {
        help_ptr = (LSA_UINT8*)ptr_rqb_src + HIF_SER_SIZEOF(4);        /* start deserialization after the original upper RQB pointer */

        if (HIF_IS_NULL((LSA_UINT8*)ptr_rqb_src))
        {
            return HIF_ERR_RESOURCE;
        }
        else
        {
            new_rqb = (GSY_RQB_TYPE*)hif_get_rqb_id(ptr_rqb_src); /* get orig. upper RQB */
            HIF_PROGRAM_TRACE_03(0,LSA_TRACE_LEVEL_NOTE_LOW,"hif_gsy_deserialize_upper() opc(%d) shm_addr(0x%08x) pRQB(0x%08x)",opcode,ptr_rqb_src,new_rqb+4);
            *ptr_ptr_rqb_dest = new_rqb;

            /* restore the Upper RQB */
            HIF_RQB_SET_OPCODE(new_rqb,opcode);                        /* _opcode */
            help_ptr+=4;                        /* skip opcode on shared-mem */
            HIF_SER_RESPONSE_SHMTH(&response,(LSA_UINT32**)&help_ptr);                        /* _response */
            HIF_RQB_SET_RESPONSE(new_rqb,response);                                                /* _response */
        }
    }
    else
    {
        HIF_PROGRAM_TRACE_03(0,LSA_TRACE_LEVEL_FATAL,"hif_gsy_deserialize_upper(). RQB-Opcode:0x%x is not valid! shm_addr(0x%08x) pRQB(0x%08x)", opcode, ptr_rqb_src, new_rqb+4);
        HIF_FATAL(0);
    }
    HIF_ASSERT(HIF_IS_NOT_NULL(new_rqb)); /*#16*/
    *ptr_ptr_rqb_dest = new_rqb;

    /* Check for overwrite */
    HIF_SHM_CHECK_OVERWRITE_DESERIALIZE(ptr_rqb_src, opcode, help_ptr, "hif_gsy_deserialize_upper");

    return HIF_OK;
}

/****************************************************************************
*  Name:        hif_gsy_deserialize_lower()
*  Purpose:     De-serializes the RQB from the HIF shared memory into the
*               hosts local memory - according to the component ID and
*               OP-code in the RQB.
*  Input:       pRqbSrc - pointer to the RQB in shared memory
                pPipe   - pointer to pipe management structure
*  Output:      ppRqbDest - pointer to the RQB in the hosts local memory
*  Return:      HIF_OK
*               HIF_ERR_RESOURCE
****************************************************************************/
LSA_UINT16      hif_gsy_deserialize_lower (
        HIF_SYS_HANDLE hSysDev,
        LSA_VOID* pHifPipeHandle,
        LSA_VOID* pRqbSrc,
        LSA_VOID** ppRqbDest )
{
    LSA_UINT8* help_ptr = LSA_NULL;
    LSA_OPCODE_TYPE opcode = 0;
    LSA_VOID* ptr_rqb_src = (LSA_VOID*)pRqbSrc;
    GSY_RQB_TYPE** ptr_ptr_rqb_dest = (GSY_RQB_TYPE**)ppRqbDest;
    GSY_RQB_TYPE* new_rqb  = LSA_NULL;
    LSA_UINT32 rqb_size = 0;
    

    HIF_ASSERT(HIF_IS_NOT_NULL(ptr_ptr_rqb_dest)); /*#17*/
    HIF_DESER_GET_OPCODE(ptr_rqb_src, opcode);

    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(pHifPipeHandle);

    if (opcode==GSY_OPC_OPEN_CHANNEL)
    {
        /* calculate rqb-size */
        rqb_size=sizeof(GSY_RQB_TYPE) + HIF_SER_SIZEOF(4);        /* RQB-size + original upper RQB pointer */
        help_ptr = (LSA_UINT8*)ptr_rqb_src;

        HIF_ALLOC_LOCAL_MEM(((LSA_VOID**)(&new_rqb)), rqb_size );
        HIF_PROGRAM_TRACE_04(0,LSA_TRACE_LEVEL_NOTE_LOW,"hif_gsy_deserialize_lower() opc(%d) rqb_size(%d) shm_addr(0x%08x) pRQB(0x%08x)",opcode,rqb_size,help_ptr,new_rqb+4);
        if (HIF_IS_NULL(new_rqb))
        {
            return HIF_ERR_RESOURCE;
        }
        else
        {
            HIF_SER_COPY_SHMTH_32((LSA_UINT32*)new_rqb,(LSA_UINT32**)&help_ptr);            /* orig. upper RQB pointer */
            new_rqb=(GSY_RQB_TYPE*)((LSA_UINT8*)new_rqb+HIF_SER_SIZEOF(4));                                     /* real lower rqb src without orig. upper RQB pointer */
            *ptr_ptr_rqb_dest = new_rqb;                                    /* return real lower rqb src without orig. upper RQB pointer */

            /* restore Lower RQB */
            HIF_RQB_SET_OPCODE(new_rqb,opcode);                        /* _opcode */
            help_ptr+=4;                        /* skip opcode on shared-mem */
            HIF_SER_SYS_PATH_SHMTH(&new_rqb->args.channel.sys_path,(LSA_UINT32**)&help_ptr);            /* sys_path */
        }
    }
    else if (opcode==GSY_OPC_CLOSE_CHANNEL)
    {
        /* calculate rqb-size */
        rqb_size=sizeof(GSY_RQB_TYPE) + HIF_SER_SIZEOF(4);        /* RQB-size + original upper RQB pointer */
        help_ptr = (LSA_UINT8*)ptr_rqb_src;

        HIF_ALLOC_LOCAL_MEM(((LSA_VOID**)(&new_rqb)), rqb_size );
        HIF_PROGRAM_TRACE_04(0,LSA_TRACE_LEVEL_NOTE_LOW,"hif_gsy_deserialize_lower() opc(%d) rqb_size(%d) shm_addr(0x%08x) pRQB(0x%08x)",opcode,rqb_size,help_ptr,new_rqb+4);
        if (HIF_IS_NULL(new_rqb))
        {
            return HIF_ERR_RESOURCE;
        }
        else
        {
            HIF_SER_COPY_SHMTH_32((LSA_UINT32*)new_rqb,(LSA_UINT32**)&help_ptr);            /* orig. upper RQB pointer */
            new_rqb=(GSY_RQB_TYPE*)((LSA_UINT8*)new_rqb+HIF_SER_SIZEOF(4));                                     /* real lower rqb src without orig. upper RQB pointer */
            *ptr_ptr_rqb_dest = new_rqb;                                    /* return real lower rqb src without orig. upper RQB pointer */

            /* restore Lower RQB */
            HIF_RQB_SET_OPCODE(new_rqb,opcode);                        /* _opcode */
            help_ptr+=4;                        /* skip opcode on shared-mem */
        }
    }
    else
    {
        HIF_PROGRAM_TRACE_03(0,LSA_TRACE_LEVEL_FATAL,"hif_gsy_deserialize_lower(). RQB-Opcode:0x%x is not valid! shm_addr(0x%08x) pRQB(0x%08x)", opcode, ptr_rqb_src, new_rqb+4);
        HIF_FATAL(0);
    }
    HIF_ASSERT(HIF_IS_NOT_NULL(new_rqb)); /*#16*/
    HIF_RQB_SET_COMP_ID(new_rqb, LSA_COMP_ID_GSY);            /* set comp id */
    *ptr_ptr_rqb_dest = new_rqb;

    /* Check for overwrite */
    HIF_SHM_CHECK_OVERWRITE_DESERIALIZE(ptr_rqb_src, opcode, help_ptr, "hif_gsy_deserialize_lower");

    return HIF_OK;
}



#endif /* of HIF_CFG_USE_GSY */
#if defined( TOOL_CHAIN_MICROSOFT ) /* Microsoft Visual C++ */
#pragma component(browser, on, references)
#endif
/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
