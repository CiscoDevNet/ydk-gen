#include <Python.h>
#include <structmember.h>

#include <libnetconf.h>

#include "netconf.h"

typedef struct {
	PyObject_HEAD
	struct nc_session* session;
} ncSessionObject;

/* from netconf.c */
extern PyObject *libnetconfError;

extern struct nc_cpblts *global_cpblts;

static PyMemberDef ncSessionMembers[] = {
	{NULL}  /* Sentinel */
};

#define SESSION_CHECK(self) if(!(self->session)){PyErr_SetString(libnetconfError,"Session closed.");return NULL;}

static void ncSessionFree(ncSessionObject *self)
{
	PyObject *err_type, *err_value, *err_traceback;

	/* save the current exception state */
	PyErr_Fetch(&err_type, &err_value, &err_traceback);

	nc_session_free(self->session);

	/* restore the saved exception state */
	PyErr_Restore(err_type, err_value, err_traceback);

	Py_TYPE(self)->tp_free((PyObject*)self);
}

/* rpc parameter is freed after the function call */
static int op_send_recv(ncSessionObject *self, nc_rpc* rpc, char **data)
{
	nc_reply *reply = NULL;
	int ret = EXIT_SUCCESS;

	/* send the request and get the reply */
	switch (nc_session_send_recv(self->session, rpc, &reply)) {
	case NC_MSG_UNKNOWN:
		if (nc_session_get_status(self->session) != NC_SESSION_STATUS_WORKING) {
			PyErr_SetString(libnetconfError, "Session damaged, closing.");
			/* free the Session */
			nc_session_free(self->session);
			self->session = NULL;
		}
		ret = EXIT_FAILURE;
		break;
	case NC_MSG_NONE:
		/* error occurred, but processed by callback */
		break;
	case NC_MSG_REPLY:
		switch (nc_reply_get_type(reply)) {
		case NC_REPLY_OK:
			break;
		case NC_REPLY_DATA:
			if (data) {
				*data = nc_reply_get_data (reply);
			}
			break;
		case NC_REPLY_ERROR:
			ret = EXIT_FAILURE;
			break;
		default:
			PyErr_SetString(libnetconfError, "Unexpected operation result.");
			ret = EXIT_FAILURE;
			break;
		}
		break;
	default:
		PyErr_SetString(libnetconfError, "Unknown error occurred.");
		ret = EXIT_FAILURE;
		break;
	}
	nc_rpc_free(rpc);
	nc_reply_free(reply);

	return (ret);
}

static PyObject *get_common(ncSessionObject *self, const char *filter, int wdmode, int datastore)
{
	char *data = NULL;
	struct nc_filter *st_filter = NULL;
	nc_rpc *rpc = NULL;
	PyObject *result = NULL;

	/* create filter if specified */
	if (filter) {
		if ((st_filter = nc_filter_new(NC_FILTER_SUBTREE, filter)) == NULL) {
			return (NULL);
		}
	}

	/* check datastore */
	switch(datastore) {
	case NC_DATASTORE_STARTUP:
		if (!nc_cpblts_enabled(self->session, NETCONF_CAP_STARTUP)) {
			PyErr_SetString(libnetconfError, ":startup capability not supported.");
			return (NULL);
		}
		break;
	case NC_DATASTORE_CANDIDATE:
		if (!nc_cpblts_enabled(self->session, NETCONF_CAP_CANDIDATE)) {
			PyErr_SetString(libnetconfError, ":candidate capability not supported.");
			return (NULL);
		}
		break;
	}

	/* create RPC */
	if (datastore == NC_DATASTORE_ERROR) {
		rpc = nc_rpc_get(st_filter);
	} else {
		rpc = nc_rpc_getconfig(datastore, st_filter);
	}
	nc_filter_free(st_filter);
	if (!rpc) {
		return(NULL);
	}

	/* set with defaults settings */
	if (wdmode) {
		if (nc_rpc_capability_attr(rpc, NC_CAP_ATTR_WITHDEFAULTS_MODE, wdmode) != EXIT_SUCCESS) {
			nc_rpc_free(rpc);
			return (NULL);
		}
	}

	/* send request ... */
	if (op_send_recv(self, rpc, &data) == EXIT_SUCCESS && data != NULL) {
		/* ... and prepare the result */
		result = PyUnicode_FromString(data);
		free(data);
	}

	return (result);
}

static PyObject *ncOpGet(ncSessionObject *self, PyObject *args, PyObject *keywords)
{
	const char *filter = NULL;
	int wdmode = NCWD_MODE_NOTSET;
	char *kwlist[] = {"filter", "wd", NULL};

	SESSION_CHECK(self);

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "|zi", kwlist, &filter, &wdmode)) {
		return (NULL);
	}

	return (get_common(self, filter, wdmode, NC_DATASTORE_ERROR));
}

static PyObject *ncOpGetConfig(ncSessionObject *self, PyObject *args, PyObject *keywords)
{
	const char *filter = NULL;
	int wdmode = NCWD_MODE_NOTSET;
	int source = NC_DATASTORE_ERROR;
	char *kwlist[] = {"source", "filter", "wd", NULL};

	SESSION_CHECK(self);

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "i|zi", kwlist, &source, &filter, &wdmode)) {
		return (NULL);
	}

	if (source != NC_DATASTORE_RUNNING &&
			source != NC_DATASTORE_STARTUP &&
			source != NC_DATASTORE_CANDIDATE) {
		PyErr_SetString(PyExc_ValueError, "Invalid \'source\' value.");
		return (NULL);
	}

	return (get_common(self, filter, wdmode, source));
}

static PyObject *ncOpDeleteConfig(ncSessionObject *self, PyObject *args, PyObject *keywords)
{
	int target = NC_DATASTORE_ERROR;
	PyObject *PyTarget;
	nc_rpc *rpc = NULL;
	char *url = NULL;
	char *kwlist[] = {"target", NULL};

	SESSION_CHECK(self);

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "O", kwlist, &PyTarget)) {
		return (NULL);
	}

	/* get target type */
	if (strcmp(Py_TYPE(PyTarget)->tp_name, "int") == 0) {
		if (!PyArg_Parse(PyTarget, "i", &target)) {
			return (NULL);
		}
	} else if (strcmp(Py_TYPE(PyTarget)->tp_name, "str") == 0) {
		if (!PyArg_Parse(PyTarget, "s", &url)) {
			return (NULL);
		}
		if (strcasestr(url, "://") != NULL) {
			target = NC_DATASTORE_URL;
		} else {
			PyErr_SetString(PyExc_ValueError, "Invalid \'target\' value.");
			return (NULL);
		}
	}

	/* create RPC */
	rpc = nc_rpc_deleteconfig(target, url);

	/* send request ... */
	if (op_send_recv(self, rpc, NULL) == EXIT_SUCCESS) {
		/* ... and return the result */
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

static PyObject *ncOpKillSession(ncSessionObject *self, PyObject *args, PyObject *keywords)
{
	const char *id = NULL;
	nc_rpc *rpc = NULL;

	char *kwlist[] = {"id", NULL};

	SESSION_CHECK(self);

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "s", kwlist, &id)) {
		return (NULL);
	}

	/* create RPC */
	rpc = nc_rpc_killsession(id);

	/* send request ... */
	if (op_send_recv(self, rpc, NULL) == EXIT_SUCCESS) {
		/* ... and return the result */
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

static PyObject *ncOpEditConfig(ncSessionObject *self, PyObject *args, PyObject *keywords)
{
	int source = NC_DATASTORE_ERROR, target = NC_DATASTORE_ERROR;
	int defop = 0, erroropt = NC_EDIT_ERROPT_NOTSET, testopt = NC_EDIT_TESTOPT_TESTSET;
	char *data1 = NULL;
	int i;
	PyObject *PySource;
	nc_rpc *rpc = NULL;
	char *kwlist[] = {"target", "source", "defop", "erropt", "testopt", NULL};

	SESSION_CHECK(self);

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "iO|iii", kwlist, &target, &PySource,
			&defop, &erroropt, &testopt)) {
		return (NULL);
	}

	/* get source type */
	if (strcmp(Py_TYPE(PySource)->tp_name, "int") == 0) {
		if (!PyArg_Parse(PySource, "i", &source)) {
			return (NULL);
		}
	} else if (strcmp(Py_TYPE(PySource)->tp_name, "str") == 0) {
		if (!PyArg_Parse(PySource, "s", &data1)) {
			return (NULL);
		}
		for (i = 0; data1[i] != '\0' && isspace(data1[i]); i++);
		if (data1[i] == '<') {
			source = NC_DATASTORE_CONFIG;
		} else {
			if (strcasestr(data1, "://") != NULL) {
				source = NC_DATASTORE_URL;
			} else {
				PyErr_SetString(PyExc_ValueError, "Invalid \'source\' value.");
				return (NULL);
			}
		}
	}

	/* create RPC */
	rpc = nc_rpc_editconfig(target, source, defop, erroropt, testopt, data1);

	if (rpc == NULL) {
		Py_RETURN_FALSE;
	}

	/* send request ... */
	if (op_send_recv(self, rpc, NULL) == EXIT_SUCCESS) {
		/* ... and return the result */
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

static PyObject *ncOpCopyConfig(ncSessionObject *self, PyObject *args, PyObject *keywords)
{
	int wdmode = NCWD_MODE_NOTSET;
	int source = NC_DATASTORE_ERROR, target = NC_DATASTORE_ERROR;
	char *data1 = NULL, *data2 = NULL;
	int i;
	PyObject *PySource, *PyTarget;
	nc_rpc *rpc = NULL;
	char *kwlist[] = {"source", "target", "wd", NULL};

	SESSION_CHECK(self);

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "OO|i", kwlist, &PySource, &PyTarget, &wdmode)) {
		return (NULL);
	}

	/* get source type */
	if (strcmp(Py_TYPE(PySource)->tp_name, "int") == 0) {
		if (!PyArg_Parse(PySource, "i", &source)) {
			return (NULL);
		}
	} else if (strcmp(Py_TYPE(PySource)->tp_name, "str") == 0) {
		if (!PyArg_Parse(PySource, "s", &data1)) {
			return (NULL);
		}
		for (i = 0; data1[i] != '\0' && isspace(data1[i]); i++);
		if (data1[i] == '<') {
			source = NC_DATASTORE_CONFIG;
		} else {
			if (strcasestr(data1, "://") != NULL) {
				source = NC_DATASTORE_URL;
			} else {
				PyErr_SetString(PyExc_ValueError, "Invalid \'source\' value.");
				return (NULL);
			}
		}
	}

	/* get target type */
	if (strcmp(Py_TYPE(PyTarget)->tp_name, "int") == 0) {
		if (!PyArg_Parse(PyTarget, "i", &target)) {
			return (NULL);
		}
	} else if (strcmp(Py_TYPE(PyTarget)->tp_name, "str") == 0) {
		if (!PyArg_Parse(PyTarget, "s", &data2)) {
			return (NULL);
		}
		if (strcasestr(data2, "://") != NULL) {
			target = NC_DATASTORE_URL;
		} else {
			PyErr_SetString(PyExc_ValueError, "Invalid \'target\' value.");
			return (NULL);
		}
		if (data1 == NULL) {
			/* 3rd and 4th argument to nc_rpc_copyconfig() are variadic - if the
			 * source is not config or URL, the target URL must be placed as
			 * 3rd argument, not 4th
			 */
			data1 = data2;
			data2 = NULL;
		}
	}

	/* create RPC */
	rpc = nc_rpc_copyconfig(source, target, data1, data2);

	/* set with defaults settings */
	if (wdmode) {
		if (nc_rpc_capability_attr(rpc, NC_CAP_ATTR_WITHDEFAULTS_MODE, wdmode) != EXIT_SUCCESS) {
			nc_rpc_free(rpc);
			return (NULL);
		}
	}

	/* send request ... */
	if (op_send_recv(self, rpc, NULL) == EXIT_SUCCESS) {
		/* ... and return the result */
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

static PyObject *lock_common(ncSessionObject *self, PyObject *args, PyObject *keywords, nc_rpc* (func)(NC_DATASTORE))
{
	int target = NC_DATASTORE_ERROR;
	nc_rpc *rpc = NULL;
	char *kwlist[] = {"target", NULL};

	SESSION_CHECK(self);

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "i|zi", kwlist, &target)) {
		return (NULL);
	}

	/* check datastore */
	switch(target) {
	case NC_DATASTORE_STARTUP:
		if (!nc_cpblts_enabled(self->session, NETCONF_CAP_STARTUP)) {
			PyErr_SetString(libnetconfError, ":startup capability not supported.");
			return (NULL);
		}
		break;
	case NC_DATASTORE_CANDIDATE:
		if (!nc_cpblts_enabled(self->session, NETCONF_CAP_CANDIDATE)) {
			PyErr_SetString(libnetconfError, ":candidate capability not supported.");
			return (NULL);
		}
		break;
	}

	/* create RPC */
	rpc = func(target);

	/* send request ... */
	if (op_send_recv(self, rpc, NULL) == EXIT_SUCCESS) {
		/* ... and return the result */
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

static PyObject *ncOpLock(ncSessionObject *self, PyObject *args, PyObject *keywords)
{
	return (lock_common(self, args, keywords, nc_rpc_lock));
}

static PyObject *ncOpUnlock(ncSessionObject *self, PyObject *args, PyObject *keywords)
{
	return (lock_common(self, args, keywords, nc_rpc_unlock));
}

static PyObject *ncProcessRPC(ncSessionObject *self)
{
	NC_MSG_TYPE ret;
	NC_RPC_TYPE req_type;
	NC_OP req_op;
	nc_rpc *rpc = NULL;
	nc_reply *reply = NULL;
	struct nc_err* e = NULL;

	SESSION_CHECK(self);

	/* receive incoming message */
	ret = nc_session_recv_rpc(self->session, -1, &rpc);
	if (ret != NC_MSG_RPC) {
		if (nc_session_get_status(self->session) != NC_SESSION_STATUS_WORKING) {
			/* something really bad happend, and communication is not possible anymore */
			nc_session_free(self->session);
			self->session = NULL;
		}
		Py_RETURN_NONE;
	}

	/* process it */
	req_type = nc_rpc_get_type(rpc);
	req_op = nc_rpc_get_op(rpc);
	if (req_type == NC_RPC_SESSION) {
		/* process operations affectinf session */
		switch(req_op) {
		case NC_OP_CLOSESESSION:
			/* exit the event loop immediately without processing any following request */
			reply = nc_reply_ok();
			break;
		case NC_OP_KILLSESSION:
			/* todo: kill the requested session */
			reply = nc_reply_error(nc_err_new(NC_ERR_OP_NOT_SUPPORTED));
			break;
		default:
			reply = nc_reply_error(nc_err_new(NC_ERR_OP_NOT_SUPPORTED));
			break;
		}
	} else if (req_type == NC_RPC_DATASTORE_READ) {
		/* process operations reading datastore */
		switch (req_op) {
		case NC_OP_GET:
		case NC_OP_GETCONFIG:
			reply = ncds_apply_rpc2all(self->session, rpc,  NULL);
			break;
		default:
			reply = nc_reply_error(nc_err_new(NC_ERR_OP_NOT_SUPPORTED));
			break;
		}
	} else if (req_type == NC_RPC_DATASTORE_WRITE) {
		/* process operations affecting datastore */
		switch (req_op) {
		case NC_OP_LOCK:
		case NC_OP_UNLOCK:
		case NC_OP_COPYCONFIG:
		case NC_OP_DELETECONFIG:
		case NC_OP_EDITCONFIG:
			reply = ncds_apply_rpc2all(self->session, rpc, NULL);
			break;
		default:
			reply = nc_reply_error(nc_err_new(NC_ERR_OP_NOT_SUPPORTED));
			break;
		}
	} else {
		/* process other operations */
		reply = ncds_apply_rpc2all(self->session, rpc, NULL);
	}

	/* create reply */
	if (reply == NULL) {
		reply = nc_reply_error(nc_err_new(NC_ERR_OP_FAILED));
	} else if (reply == NCDS_RPC_NOT_APPLICABLE) {
		e = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(e, NC_ERR_PARAM_MSG, "Requested operation cannot be performed on the managed datastore.");
		reply = nc_reply_error(e);
	}

	/* and send the reply to the client */
	nc_session_send_reply(self->session, rpc, reply);
	nc_rpc_free(rpc);
	nc_reply_free(reply);

	if (req_op == NC_OP_CLOSESESSION) {
		/* free the Session */
		nc_session_free(self->session);
		self->session = NULL;
	}

	Py_RETURN_NONE;
}

static PyObject *ncIsActive(ncSessionObject *self)
{
	if (self->session) {
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

static PyObject *ncSessionConnect(PyObject *cls, PyObject *args, PyObject *keywords)
{
	PyObject *result = NULL;
	char *host = NULL, *user = NULL, *transport_s = "ssh", *version = NULL;
	unsigned short port = 830;
	PyObject *PyCpblts = NULL;
	int fd_in = -1, fd_out = -1;

	char *kwlist[] = {"host", "port", "user", "transport", "version", "fd_in", "fd_out", NULL};

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "s|Hzzsii", kwlist, &host, &port, &user, &transport_s, &version, &fd_in, &fd_out)) {
		return (NULL);
	}

	if (!version) {
		/* let the default value to ncSessionInit() */
		Py_INCREF(Py_None);
		PyCpblts = Py_None;
	} else {
		PyCpblts = PyList_New(1);
		PyList_SET_ITEM(PyCpblts, 0, PyUnicode_FromString(version));
	}

	result = PyObject_CallFunction(cls, "sHssOii", host, port, user, transport_s, PyCpblts, fd_in, fd_out);

	Py_DECREF(PyCpblts);

	return(result);
}

static PyObject *ncSessionAccept(PyObject *cls, PyObject *args, PyObject *keywords)
{
	PyObject *result = NULL;
	PyObject *PyCpblts = NULL;
	const char *user = NULL;
	int fd_in = STDIN_FILENO, fd_out = STDOUT_FILENO;

	char *kwlist[] = {"user", "capabilities", "fd_in", "fd_out", NULL};

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "|zOii", kwlist, &user, &PyCpblts, &fd_in, &fd_out)) {
		return (NULL);
	}

	if (PyCpblts == NULL) {
		Py_INCREF(Py_None);
		PyCpblts = Py_None;
	}

	result = PyObject_CallFunction(cls, "sHssOii", NULL, 0, user, NULL, PyCpblts, fd_in, fd_out);

	if (PyCpblts == Py_None) {
		Py_DECREF(Py_None);
	}

	return(result);
}

static int ncSessionInit(ncSessionObject *self, PyObject *args, PyObject *keywords)
{
	const char *host = NULL, *user = NULL, *transport_s = NULL;
	unsigned short port = 830;
	PyObject *PyCpblts = NULL;
	PyObject *PyStr;
	struct nc_session *session;
	struct nc_cpblts *cpblts = NULL;
	char cpblts_free_flag = 0;
	char* item = NULL;
	Py_ssize_t l, i;
	int ret;
	int fd_in = -1, fd_out = -1;
	NC_TRANSPORT transport = NC_TRANSPORT_UNKNOWN;

	char *kwlist[] = {"host", "port", "user", "transport", "capabilities", "fd_in", "fd_out", NULL};

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "|zHzzOii", kwlist, &host, &port, &user, &transport_s, &PyCpblts, &fd_in, &fd_out)) {
		return -1;
	}

	if ((fd_in >= 0 && fd_out < 0) || (fd_in < 0 && fd_out >= 0)) {
		PyErr_SetString(PyExc_ValueError, "Both or none of fd_in and fd_out arguments must be set.");
		return -1;
	}

	if (host != NULL) {
		/* Client side */
		if (transport_s && strcasecmp(transport_s, NETCONF_TRANSPORT_TLS) == 0) {
			ret = nc_session_transport(NC_TRANSPORT_TLS);
			transport = NC_TRANSPORT_TLS;
		} else {
			ret = nc_session_transport(NC_TRANSPORT_SSH);
			transport = NC_TRANSPORT_SSH;
		}
		if (ret != EXIT_SUCCESS) {
			return -1;
		}
	}

	if (PyCpblts != NULL && PyCpblts != Py_None) {
		cpblts = nc_cpblts_new(NULL);
		cpblts_free_flag = 1;
		if (PyList_Check(PyCpblts)) {
			if ((l = PyList_Size(PyCpblts)) > 0) {
				for (i = 0; i < l; i++) {
					PyObject *PyUni = PyList_GetItem(PyCpblts, i);
					Py_INCREF(PyUni);
					if (!PyUnicode_Check(PyUni)) {
						PyErr_SetString(PyExc_TypeError, "Capabilities list must contain strings.");
						nc_cpblts_free(cpblts);
						Py_DECREF(PyUni);
						return -1;
					}
					PyStr = PyUnicode_AsEncodedString(PyUni, "UTF-8", NULL);
					Py_DECREF(PyUni);
					if (PyStr == NULL) {
						nc_cpblts_free(cpblts);
						return -1;
					}
					item = PyBytes_AsString(PyStr);
					if (item == NULL) {
						nc_cpblts_free(cpblts);
						Py_DECREF(PyStr);
						return -1;
					}
					ret = nc_cpblts_add(cpblts, item);
					Py_DECREF(PyStr);
					if (ret != EXIT_SUCCESS) {
						nc_cpblts_free(cpblts);
						return -1;
					}
				}
			}
		} else {
			PyErr_SetString(PyExc_TypeError, "Capabilities argument is expected to be a list of strings.");
			return -1;
		}
	}

	if (cpblts == NULL) {
		/* use global capabilities, that are, by default, same as libnetconf's
		 * default capabilities
		 */
		cpblts = global_cpblts;
	}

	if (host != NULL) {
		/* Client side */
		if (fd_in != -1 && fd_out != -1) {
			session = nc_session_connect_inout(fd_in, fd_out, cpblts,
					host, NULL, user, transport);
		} else {
			session = nc_session_connect(host, port, user, cpblts);
		}
	} else {
		/* Server side */
		session = nc_session_accept_inout(cpblts, user,
				fd_in != -1 ? fd_in : STDIN_FILENO,
				fd_out != -1 ? fd_out : STDOUT_FILENO);

		/* add to the list of monitored sessions */
		nc_session_monitor(session);
	}

	if (cpblts_free_flag) {
		nc_cpblts_free(cpblts);
	}

	if (session == NULL) {
		return -1;
	}

	nc_session_free(self->session);
	self->session = session;

	return 0;
}

static PyObject *ncSessionGetId(ncSessionObject *self, void *closure)
{
	return PyUnicode_FromFormat("%s", nc_session_get_id(self->session));
}

static PyObject *ncSessionGetHost(ncSessionObject *self, void *closure)
{
	return PyUnicode_FromFormat("%s", nc_session_get_host(self->session));
}

static PyObject *ncSessionGetPort(ncSessionObject *self, void *closure)
{
	return PyUnicode_FromFormat("%s", nc_session_get_port(self->session));
}

static PyObject *ncSessionGetUser(ncSessionObject *self, void *closure)
{
	return PyUnicode_FromFormat("%s", nc_session_get_user(self->session));
}

static PyObject *ncSessionGetTransport(ncSessionObject *self, void *closure)
{
	NC_TRANSPORT t = nc_session_get_transport(self->session);
	return PyUnicode_FromFormat("%s", (t == NC_TRANSPORT_TLS) ? "TLS" : "SSH");
}

static PyObject *ncSessionStr(ncSessionObject *self)
{
	const char* port = nc_session_get_port(self->session);

	if (port) {
		/* client side */
		return PyUnicode_FromFormat("NETCONF Session %s to %s:%s (%lu)",
				nc_session_get_id(self->session),
				nc_session_get_host(self->session),
				port,
				((PyObject*)(self))->ob_refcnt);
	} else {
		/* server side */
		return PyUnicode_FromFormat("NETCONF Session %s (%lu)",
				nc_session_get_id(self->session),
				((PyObject*)(self))->ob_refcnt);
	}
}

static PyObject *ncSessionGetCapabilities(ncSessionObject *self, void *closure)
{
	struct nc_cpblts* cpblts;
	PyObject *list;
	const char *item;
	ssize_t pos;

	cpblts = nc_session_get_cpblts(self->session);
	if (cpblts == NULL) {
		return (NULL);
	}

	list = PyList_New(nc_cpblts_count(cpblts));
	nc_cpblts_iter_start(cpblts);
	for (pos = 0; pos < nc_cpblts_count(cpblts); pos++) {
		item = nc_cpblts_iter_next(cpblts);
		PyList_SetItem(list, pos, PyUnicode_FromFormat("%s", item));
	}

	return list;
}

static PyObject *ncSessionGetVersion(ncSessionObject *self, void *closure)
{
	int ver;
	const char* ver_s;

	ver = nc_session_get_version(self->session);
	switch(ver) {
	case 0:
		ver_s = "1.0";
		break;
	case 1:
		ver_s = "1.1";
		break;
	default:
		ver_s = "unknown";
		break;
	}

	return PyUnicode_FromFormat("%s", ver_s);
}

static PyGetSetDef ncSessionGetSeters[] = {
    {"id", (getter)ncSessionGetId, NULL, "NETCONF Session id.", NULL},
    {"host", (getter)ncSessionGetHost, NULL, "Host where the NETCONF Session is connected.", NULL},
    {"port", (getter)ncSessionGetPort, NULL, "Port number where the NETCONF Session is connected.", NULL},
    {"user", (getter)ncSessionGetUser, NULL, "Username of the user connected with the NETCONF Session.", NULL},
    {"transport", (getter)ncSessionGetTransport, NULL, "Transport protocol used for the NETCONF Session.", NULL},
    {"version", (getter)ncSessionGetVersion, NULL, "NETCONF Protocol version used for the NETCONF Session.", NULL},
    {"capabilities", (getter)ncSessionGetCapabilities, NULL, "Capabilities of the NETCONF Session.", NULL},
    {NULL}  /* Sentinel */
};

static PyMethodDef ncSessionMethods[] = {
	{"connect", (PyCFunction)ncSessionConnect,
		METH_VARARGS | METH_KEYWORDS | METH_CLASS,
		PyDoc_STR("ncSessionConnect(host, port, username, transport, version)-> ncSessionObject\n\n"
				"Create NETCONF session connecting to a NETCONF server.")},
	{"accept", (PyCFunction)ncSessionAccept,
		METH_VARARGS | METH_KEYWORDS | METH_CLASS,
		PyDoc_STR("Create NETCONF session accepting connection from a NETCONF client.")},
	{"get", (PyCFunction)ncOpGet,
		METH_VARARGS | METH_KEYWORDS,
		PyDoc_STR("Execute NETCONF <get> RPC.")},
	{"getConfig", (PyCFunction)ncOpGetConfig,
		METH_VARARGS | METH_KEYWORDS,
		PyDoc_STR("Execute NETCONF <get-config> RPC.")},
	{"lock", (PyCFunction)ncOpLock,
		METH_VARARGS | METH_KEYWORDS,
		PyDoc_STR("Execute NETCONF <lock> RPC.")},
	{"unlock", (PyCFunction)ncOpUnlock,
		METH_VARARGS | METH_KEYWORDS,
		PyDoc_STR("Execute NETCONF <unlock> RPC.")},
	{"editConfig", (PyCFunction)ncOpEditConfig,
		METH_VARARGS | METH_KEYWORDS,
		PyDoc_STR("Execute NETCONF <edit-config> RPC.")},
	{"copyConfig", (PyCFunction)ncOpCopyConfig,
		METH_VARARGS | METH_KEYWORDS,
		PyDoc_STR("Execute NETCONF <copy-config> RPC.")},
	{"deleteConfig", (PyCFunction)ncOpDeleteConfig,
		METH_VARARGS | METH_KEYWORDS,
		PyDoc_STR("Execute NETCONF <delete-config> RPC.")},
	{"killSession", (PyCFunction)ncOpKillSession,
		METH_VARARGS | METH_KEYWORDS,
		PyDoc_STR("Execute NETCONF <kill-session> RPC.")},
	{"processRequest", (PyCFunction)ncProcessRPC,
		METH_NOARGS,
		PyDoc_STR("Process a client request.")},
	{"isActive", (PyCFunction)ncIsActive,
		METH_NOARGS,
		PyDoc_STR("Ask if the session is still active.")},
	{NULL, NULL, 0, NULL}
};

PyDoc_STRVAR(sessionDoc,
"Create the NETCONF Session\n"
"netconf.Session(host, [port, user, transport, capabilities]) -> connect to the NETCONF server\n"
"netconf.Session([user, capabilities, fd_in, fd_out]) -> accept incoming connection from fd_in (stdin)\n\n"
"Arguments:\n"
"\thost         - hostname or address of the server\n"
"\tport         - port where to connect to, by default 830\n"
"\tuser         - username, by default the currently logged user\n"
"\ttransport    - NETCONF transport protocol, by default SSH\n"
"\tcapbilities  - list of strings with the supported NETCONF capabilities,\n"
"\tfd_in        - input file descriptor for reading decrypted data on server side (stdin by default),\n"
"\tfd_out       - output file descriptor for writing unencrypted data on server side (stdout by default),\n"
"\t               by default generated by the libnetconf\n");

PyTypeObject ncSessionType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"netconf.Session", /* tp_name */
		sizeof(ncSessionObject), /* tp_basicsize */
		0, /* tp_itemsize */
		(destructor) ncSessionFree, /* tp_dealloc */
		0, /* tp_print */
		0, /* tp_getattr */
		0, /* tp_setattr */
		0, /* tp_reserved */
		(reprfunc)ncSessionStr, /* tp_repr */
		0, /* tp_as_number */
		0, /* tp_as_sequence */
		0, /* tp_as_mapping */
		0, /* tp_hash  */
		0, /* tp_call */
		(reprfunc)ncSessionStr, /* tp_str */
		0, /* tp_getattro */
		0, /* tp_setattro */
		0, /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
		sessionDoc, /* tp_doc */
		0, /* tp_traverse */
		0, /* tp_clear */
		0, /* tp_richcompare */
		0, /* tp_weaklistoffset */
		0, /* tp_iter */
		0, /* tp_iternext */
		ncSessionMethods, /* tp_methods */
		ncSessionMembers, /* tp_members */
		ncSessionGetSeters, /* tp_getset */
		0, /* tp_base */
		0, /* tp_dict */
		0, /* tp_descr_get */
		0, /* tp_descr_set */
		0, /* tp_dictoffset */
		(initproc) ncSessionInit, /* tp_init */
		0, /* tp_alloc */
		0, /* tp_new, set by netconf module */
};

