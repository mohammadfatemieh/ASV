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
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_vxworks.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Portability layer for socket access                                      */
/*  This module supports a portability layer for socket functions.           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 8
#define SOCK_MODULE_ID    8

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_VXWORKS
/*****************************************************************************/

#include <ioLib.h>
#include <sockLib.h>
#include <netinet/tcp.h>

/*------------------------------------------------------------------------------
// inline documentation of parameters --> sock_low.h
//----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
SockIF_Init(
	LSA_VOID
) {
	/* empty */
}

/*----------------------------------------------------------------------------*/

SOCKIF_FD SOCK_LOCAL_FCT_ATTR
SockIF_Socket(
	LSA_INT32 type
) {
	return socket(PF_INET, type, 0);
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Bind(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT *err
) {
	struct sockaddr addr;

	addr.sa_family = AF_INET; /* always, see SockIF_Socket */
	SOCK_MEMCPY(addr.sa_data, &address->sin_port, 14);

	if( bind(sock_fd, &addr, sizeof(addr)) < 0 )
	{
		*err = errno;

		switch(*err)
		{
		case EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		case ENOMEM:
		case ENOBUFS:
			return SOCK_RSP_ERR_RESOURCE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Listen(
	SOCKIF_FD sock_fd,
	LSA_INT32 backlog,
	LSA_INT* err
) {
	if( listen(sock_fd, backlog) < 0 )
	{
		*err = errno;

		switch(*err)
		{
		case EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapConnectError
	(LSA_INT error
) {
	switch(error)
	{
	case EINPROGRESS:
		return SOCK_RSP_OK_PROGRESS;

	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	case ECONNREFUSED:
		return SOCK_RSP_ERR_REM_ABORT;

	case ENETUNREACH:
	case ETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Connect(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
) {
	struct sockaddr addr;

	addr.sa_family = AF_INET; /* always, see SockIF_Socket */
	SOCK_MEMCPY(addr.sa_data, &address->sin_port, 14);

	if( connect(sock_fd, &addr, sizeof(addr)) < 0 )
	{
		*err = errno;

		return SockIF_MapConnectError(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

SOCKIF_FD SOCK_LOCAL_FCT_ATTR
SockIF_Accept(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
	/* user will not see accept errors (ACCEPT_NOTIFY rqb is not returned if accept fails) */
) {
	SOCKIF_FD new_fd;
	struct sockaddr addr;
	int addr_len = sizeof(addr);

	addr.sa_family = AF_INET;

	new_fd = accept(sock_fd, &addr, &addr_len);

	if (new_fd == SOCKIF_INVALID_FD)
	{
		*err = errno;
	}
	else
	{
		address->sin_family = addr.sa_family; /* see check above */
		SOCK_MEMCPY(&address->sin_port, addr.sa_data, 14);
	}

	return new_fd;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_Close(
	SOCKIF_FD sock_fd
) {
	if( close(sock_fd) < 0 )
	{
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapTCPError(
	LSA_INT error
) {
	switch(error)
	{
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ECONNABORTED:
	case ECONNRESET:
	case ESHUTDOWN:
		return SOCK_RSP_ERR_REM_ABORT;

	case EPIPE: /* socket is not connected */
	case ETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	case ENOMEM:
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Send(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_INT32 length,
	LSA_INT32* sent,
	LSA_INT* err
) {
	*sent = send(sock_fd
				 , (char*)buffer
				 , length
				 , 0
				 );

	if (*sent < 0)
	{
		*err = errno;

		return SockIF_MapTCPError(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_Recv(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_INT32 length,
	LSA_BOOL peek,
	LSA_INT32* received,
	LSA_INT* err
) {
	*received = recv(sock_fd
					 , (char*)buffer
					 , length
					 , peek ? MSG_PEEK : 0
					 );

	if (*received < 0)
	{
		*err = errno;

		return SockIF_MapTCPError(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

static LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_MapUDPError(
	LSA_INT err
) {
	switch(err) /* note: no ERR_REM_NET possible */
	{
	case EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case ENOMEM:
	case ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_SendTo(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_UINT32 length,
	SockIF_AddrIn* address,
	LSA_INT32* sent,
	LSA_INT* err
) {
	struct sockaddr addr;

	addr.sa_family = AF_INET; /* always, see SockIF_Socket */
	SOCK_MEMCPY(addr.sa_data, &address->sin_port, 14);

	*sent = sendto(sock_fd
				   , (char*)buffer
				   , length
				   , 0
				   , &addr
				   , sizeof(addr)
				   );

	if (*sent < 0)
	{
		*err = errno;

		return SockIF_MapUDPError(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_RecvFrom(
	SOCKIF_FD sock_fd,
	LSA_VOID* buffer,
	LSA_UINT32 length,
	LSA_BOOL peek,
	SockIF_AddrIn* address,
	LSA_INT32* received,
	LSA_INT* err
) {
	struct sockaddr addr;
	LSA_INT addr_len = sizeof(addr);

	*received = recvfrom(sock_fd
						 , (char*)buffer
						 , length
						 , peek ? MSG_PEEK : 0
						 , &addr
						 , &addr_len
						 );

	if (*received < 0)
	{
		*err = errno;

		return SockIF_MapUDPError(*err);
	}
	else
	{
		address->sin_family = addr.sa_family; /* see check above */
		SOCK_MEMCPY(&address->sin_port, addr.sa_data, 14);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_Get_Socket_Error(
	SOCKIF_FD sock_fd,
	LSA_INT* socket_error,
	LSA_INT* err
) {
	LSA_INT len = sizeof(LSA_INT);

	int ret = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, (char*)socket_error, &len);

	if (ret < 0)
	{
		*err = errno;

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
SockIF_SetSockOpt(
	SOCKIF_FD sock_fd,
	SockIF_Option option_name,
	LSA_VOID* option_value,
	LSA_INT* err
) {
	SOCK_LOG_FCT("SockIF_SetSockOpt")

	int ret;

	switch (option_name)
	{
	case SockOpt_NonBlocking:
		if( ioctl(sock_fd, FIONBIO, (LSA_INT)option_value) == ERROR )
		{
			ret = -1;
		}
		else
		{
			ret = 0;
		}
		break;

	case SockOpt_TCPNoDelay:
		ret = setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_TCPNoAckDelay:
		ret = 0;
		break;

	case SockOpt_ReuseAddr:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_KeepAlive:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_KEEPALIVE, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_DontRoute:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_DONTROUTE, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_SendBuffer:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_RecvBuffer:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_Linger:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, option_value, sizeof(SockIF_Linger));
		break;

	case SockOpt_Broadcast:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, option_value, sizeof(LSA_INT32));
		break;

	case SockOpt_OOBInline:
		ret = setsockopt(sock_fd, SOL_SOCKET, SO_OOBINLINE, option_value, sizeof(LSA_INT32));
		break;

	default:
		ret = -1; /* prevent compiler warning C4701: potentially uninitialized local variable 'ret' used */
		
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL
			, "option_name(%d) not supported by this function"
			, option_name
			);

		SOCK_FATAL();

		break;
	}

	if (ret < 0)
	{
		*err = errno;

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
SockIF_GetSockName(
	SOCKIF_FD sock_fd,
	SockIF_AddrIn* address,
	LSA_INT* err
) {
	struct sockaddr addr;
	int addr_len = sizeof(addr);

	if( getsockname(sock_fd, &addr, &addr_len) < 0 )
	{
		*err = errno;

		return SOCK_RSP_ERR_LOC_NET;
	}
	else
	{
		address->sin_family = addr.sa_family;
		SOCK_MEMCPY(&address->sin_port, addr.sa_data, 14);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_INT32 SOCK_LOCAL_FCT_ATTR
SockIF_Select(
	int nfds,
	SOCKIF_FD_SET_TYPE* readfds,
	SOCKIF_FD_SET_TYPE* writefds,
	SOCKIF_FD_SET_TYPE* errorfds,
	LSA_INT32 timeout
) {
	struct timeval tv;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	return select(nfds, readfds, writefds, errorfds, &tv);
}

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
