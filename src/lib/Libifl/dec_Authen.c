/*
 * Copyright (C) 1994-2018 Altair Engineering, Inc.
 * For more information, contact Altair at www.altair.com.
 *
 * This file is part of the PBS Professional ("PBS Pro") software.
 *
 * Open Source License Information:
 *
 * PBS Pro is free software. You can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * PBS Pro is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Commercial License Information:
 *
 * For a copy of the commercial license terms and conditions,
 * go to: (http://www.pbspro.com/UserArea/agreement.html)
 * or contact the Altair Legal Department.
 *
 * Altair’s dual-license business model allows companies, individuals, and
 * organizations to create proprietary derivative works of PBS Pro and
 * distribute them - whether embedded or bundled with other software -
 * under a commercial license agreement.
 *
 * Use of Altair’s trademarks, including but not limited to "PBS™",
 * "PBS Professional®", and "PBS Pro™" and Altair’s logos is subject to Altair's
 * trademark licensing policies.
 *
 */


/**
 * @file	dec_Authen.c
 * @brief
 * 	decode_DIS_AuthenResvPort() - decode a priv port based authentication request
 * 	decode_DIS_AuthExternal() - decode a External authentication request
 *
 *	The batch_request structure must already exist (be allocated by the
 *	caller.   It is assumed that the header fields (protocol type,
 *	protocol version, request type, and user name) have already be decoded.
 *
 *	Data items are:	unsigned int	port number
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include <sys/types.h>
#include "libpbs.h"
#include "list_link.h"
#include "server_limits.h"
#include "attribute.h"
#include "credential.h"
#include "batch_request.h"
#include "dis.h"

/**
 * @brief
 *      Decode PBS batch request to authenticate user
 * @param [in] sock socket connection
 * @param [in] preq PBS bath request
 * @return in
 * @retval 0 on success 
 * @retval > 0 on failure
 */
int
decode_DIS_AuthenResvPort(int sock, struct batch_request *preq)
{
	int rc;

	preq->rq_ind.rq_authen_resvport.rq_port = disrui(sock, &rc);
	return rc;
}

/**
 * @brief
 *      Decode PBS batch request to authenticate based on external (non-resv-port) mechanisms.
 *      The batch request contains type and the auth data.
 * 
 * @param [in] sock socket connection
 * @param [in] preq PBS bath request
 * @return in
 * @retval 0 on success 
 * @retval > 0 on failure
 */
int
decode_DIS_AuthExternal(int sock, struct batch_request *preq)
{
	int rc;
	int cred_len = 0;

	preq->rq_ind.rq_authen_external.rq_auth_type = disruc(sock, &rc);
	if (rc != DIS_SUCCESS)
		return (rc);

	cred_len = disrsi(sock, &rc);
	if (rc != DIS_SUCCESS)
		return (rc);

	switch (preq->rq_ind.rq_authen_external.rq_auth_type) {
		case AUTH_MUNGE:
			return (disrfst(sock, cred_len, preq->rq_ind.rq_authen_external.rq_authen_un.rq_munge.rq_authkey) != 0);
	}

	return DIS_EOF;
}

