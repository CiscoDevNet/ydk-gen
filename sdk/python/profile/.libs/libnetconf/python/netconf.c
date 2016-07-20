#include <Python.h>

#include <syslog.h>

#include <libnetconf.h>

#include "netconf.h"

extern PyTypeObject ncSessionType;

PyObject *libnetconfError;
PyObject *libnetconfWarning;

struct nc_cpblts *global_cpblts = NULL;
PyObject *datastores = NULL; /* dictionary */

static int syslogEnabled = 1;
static void clb_print(NC_VERB_LEVEL level, const char* msg)
{
	switch (level) {
	case NC_VERB_ERROR:
		PyErr_SetString(libnetconfError, msg);
		if (syslogEnabled) {syslog(LOG_ERR, "%s", msg);}
		break;
	case NC_VERB_WARNING:
		if (syslogEnabled) {syslog(LOG_WARNING, "%s", msg);}
		PyErr_WarnEx(libnetconfWarning, msg, 1);
		break;
	case NC_VERB_VERBOSE:
		if (syslogEnabled) {syslog(LOG_INFO, "%s", msg);}
		break;
	case NC_VERB_DEBUG:
		if (syslogEnabled) {syslog(LOG_DEBUG, "%s", msg);}
		break;
	}
}

static PyObject *setSyslog(PyObject *self, PyObject *args, PyObject *keywds)
{
	char* name = NULL;
	static char* logname = NULL;
	static int option = LOG_PID;
	static int facility = LOG_DAEMON;

	static char *kwlist[] = {"enabled", "name", "option", "facility", NULL};

	if (! PyArg_ParseTupleAndKeywords(args, keywds, "p|sii", kwlist, &syslogEnabled, &name, &option, &facility)) {
		return NULL;
	}

	if (name) {
		free(logname);
		logname = strdup(name);
	} else {
		free(logname);
		logname = NULL;
	}
	closelog();
	openlog(logname, option, facility);

	Py_RETURN_NONE;
}

static PyObject *setVerbosity(PyObject *self, PyObject *args, PyObject *keywds)
{
	int level = NC_VERB_ERROR; /* 0 */

	static char *kwlist[] = {"level", NULL};

	if (! PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &level)) {
		return NULL;
	}

	/* normalize level value if not from the enum */
	if (level < NC_VERB_ERROR) {
		nc_verbosity(NC_VERB_ERROR);
	} else if (level > NC_VERB_DEBUG) {
		nc_verbosity(NC_VERB_DEBUG);
	} else {
		nc_verbosity(level);
	}

	Py_RETURN_NONE;
}

static PyObject *getCapabilities(PyObject *self)
{
	PyObject *result = NULL;
	int c, i;

	result = PyList_New(c = nc_cpblts_count(global_cpblts));
	nc_cpblts_iter_start(global_cpblts);
	for (i = 0; i < c; i++) {
		PyList_SET_ITEM(result, i, PyUnicode_FromString(nc_cpblts_iter_next(global_cpblts)));
	}

	return (result);
}

static PyObject *setCapabilities(PyObject *self, PyObject *args, PyObject *keywds)
{
	PyObject *PyCpblts;
	Py_ssize_t l, i;
	int ret;
	char *item;
	static char *kwlist[] = {"list", NULL};
	PyObject *PyStr;

	if (! PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist, &PyList_Type, &PyCpblts)) {
		return (NULL);
	}

	if ((l = PyList_Size(PyCpblts)) < 1) {
		PyErr_SetString(PyExc_ValueError, "The capabilities list must not be empty.");
		return (NULL);
	}

	nc_cpblts_free(global_cpblts);
	global_cpblts = nc_cpblts_new(NULL);
	for (i = 0; i < l; i++) {
		PyObject *PyUni = PyList_GetItem(PyCpblts, i);
		Py_INCREF(PyUni);
		if (!PyUnicode_Check(PyUni)) {
			PyErr_SetString(PyExc_TypeError, "Capabilities list must contain strings.");
			nc_cpblts_free(global_cpblts);
			global_cpblts = NULL;
			Py_DECREF(PyUni);
			return (NULL);
		}
		PyStr = PyUnicode_AsEncodedString(PyUni, "UTF-8", NULL);
		Py_DECREF(PyUni);
		if (PyStr == NULL) {
			nc_cpblts_free(global_cpblts);
			global_cpblts = NULL;
			return (NULL);
		}
		item = PyBytes_AsString(PyStr);
		if (item == NULL) {
			nc_cpblts_free(global_cpblts);
			global_cpblts = NULL;
			Py_DECREF(PyStr);
			return (NULL);
		}
		ret = nc_cpblts_add(global_cpblts, item);
		Py_DECREF(PyStr);
		if (ret != EXIT_SUCCESS) {
			nc_cpblts_free(global_cpblts);
			global_cpblts = NULL;
			return (NULL);
		}
	}

	Py_RETURN_NONE;
}

static void set_features(const char* name, PyObject *PyFeatures)
{
	Py_ssize_t l, i;
	PyObject *PyStr;

	if (!PyFeatures) {
		/* not specified -> enable all */
		ncds_features_enableall(name);
	} else if ((l = PyList_Size(PyFeatures)) == 0) {
		/* empty list -> disable all */
		ncds_features_disableall(name);
	} else {
		/* enable specified */
		for (i = 0; i < l; i++) {
			PyObject *PyUni = PyList_GetItem(PyFeatures, i);
			Py_INCREF(PyUni);
			if (!PyUnicode_Check(PyUni)) {
				Py_DECREF(PyUni);
				continue;
			}
			PyStr = PyUnicode_AsEncodedString(PyUni, "UTF-8", NULL);
			Py_DECREF(PyUni);
			if (PyStr == NULL) {
				continue;
			}
			ncds_feature_enable(name, PyBytes_AsString(PyStr));
			Py_DECREF(PyStr);
		}
	}
}

static PyObject *ncDatastoreAddImport(PyObject *self, PyObject *args, PyObject *keywords)
{
	const char *path;
	char *name = NULL;
	PyObject *PyFeatures = NULL;
	char *kwlist[] = {"model", "features", NULL};

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "s|O!", kwlist, &path, &PyList_Type, &PyFeatures)) {
		return (NULL);
	}

	if (ncds_model_info(path, &name, NULL, NULL, NULL, NULL, NULL) != EXIT_SUCCESS ||
			ncds_add_model(path) != EXIT_SUCCESS) {
		free(name);
		return (NULL);
	}

	set_features(name, PyFeatures);

	free(name);
	Py_RETURN_NONE;
}

static PyObject *ncDatastoreAdd(PyObject *self, PyObject *args, PyObject *keywords)
{
	const char *path, *datastore = NULL, *transapi = NULL;
	char *name = NULL;
	NCDS_TYPE type = NCDS_TYPE_EMPTY;
	struct ncds_ds *ds;
	ncds_id dsid;
	PyObject *PyFeatures = NULL;
	char *kwlist[] = {"model", "datastore", "transapi", "features", NULL};

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "s|zzO!", kwlist, &path, &datastore, &transapi, &PyList_Type, &PyFeatures)) {
		return (NULL);
	}

	/* set correct type according to provided parameters */
	if (datastore) {
		type = NCDS_TYPE_FILE;
	}

	/* get name of the datastore for further referencing */
	if (ncds_model_info(path, &name, NULL, NULL, NULL, NULL, NULL) != EXIT_SUCCESS) {
		return (NULL);
	}

	/* create datastore */
	if (transapi) {
		if ((ds = ncds_new_transapi(type, path, transapi)) == NULL) {
			free(name);
			return (NULL);
		}
	} else {
		/* todo get_state() */
		if ((ds = ncds_new(type, path, NULL)) == NULL) {
			free(name);
			return (NULL);
		}
	}

	if (datastore) {
		if (ncds_file_set_path(ds, datastore) != EXIT_SUCCESS) {
			ncds_free(ds);
			free(name);
			return (NULL);
		}
	}

	if ((dsid = ncds_init(ds)) <= 0) {
		ncds_free(ds);
		free(name);
		return (NULL);
	}

	set_features(name, PyFeatures);

	if (ncds_consolidate() != EXIT_SUCCESS) {
		ncds_free(ds);
		free(name);
		return (NULL);
	}

	if (ncds_device_init(&dsid, global_cpblts, 0)) {
		ncds_free(ds);
		free(name);
		return (NULL);
	}

	PyDict_SetItem(datastores, PyUnicode_FromFormat("%d", dsid), PyUnicode_FromString(name));

	free(name);
	Py_RETURN_NONE;
}

static PyObject *ncDatastoreAddAugment(PyObject *self, PyObject *args, PyObject *keywords)
{

	const char *path, *transapi = NULL;
	char *name = NULL;
	PyObject *PyFeatures = NULL;
	char *kwlist[] = {"model", "transapi", "features", NULL};

	/* Get input parameters */
	if (! PyArg_ParseTupleAndKeywords(args, keywords, "s|zO!", kwlist, &path, &transapi, &PyList_Type, &PyFeatures)) {
		return (NULL);
	}

	/* get name of the datastore for further referencing */
	if (ncds_model_info(path, &name, NULL, NULL, NULL, NULL, NULL) != EXIT_SUCCESS) {
		return (NULL);
	}

	/* create datastore */
	if (transapi) {
		if (ncds_add_augment_transapi(path, transapi) == EXIT_FAILURE) {
			free(name);
			return (NULL);
		}
	} else {
		if (ncds_add_model(path) == EXIT_FAILURE) {
			free(name);
			return (NULL);
		}
	}

	set_features(name, PyFeatures);
	free(name);

	if (ncds_consolidate() != EXIT_SUCCESS) {
		return (NULL);
	}

	Py_RETURN_NONE;
}

static PyMethodDef netconfMethods[] = {
		{"setVerbosity", (PyCFunction)setVerbosity, METH_VARARGS | METH_KEYWORDS, "Set verbose level (0-3)."},
		{"setSyslog", (PyCFunction)setSyslog, METH_VARARGS | METH_KEYWORDS, "Set application settings for syslog."},
		{"setCapabilities", (PyCFunction)setCapabilities, METH_VARARGS | METH_KEYWORDS, "Set list of default capabilities for the following actions."},
		{"getCapabilities", (PyCFunction)getCapabilities, METH_NOARGS, "Get list of default capabilities."},
		{"addModel", (PyCFunction)ncDatastoreAddImport, METH_VARARGS | METH_KEYWORDS, "Add standalone model without datastore needed as import from other data model."},
		{"addDatastore", (PyCFunction)ncDatastoreAdd, METH_VARARGS | METH_KEYWORDS, "Add basic data model connected with the datastore."},
		{"addAugment", (PyCFunction)ncDatastoreAddAugment, METH_VARARGS | METH_KEYWORDS, "Add augmenting model."},
		{NULL, NULL, 0, NULL}
};

static struct PyModuleDef ncModule = {
		PyModuleDef_HEAD_INIT,
		"netconf",
		"NETCONF Protocol implementation",
		-1,
		netconfMethods,
};

/* module initializer */
PyMODINIT_FUNC PyInit_netconf(void)
{
	PyObject *nc;

	/* initiate libnetconf - all subsystems */
	nc_init(NC_INIT_ALL);

	/* set print callback */
	nc_callback_print(clb_print);

	/* get default caapbilities */
	global_cpblts = nc_session_get_cpblts_default();

	ncSessionType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ncSessionType) < 0) {
	    return NULL;
	}

	/* create netconf as the Python module */
	nc = PyModule_Create(&ncModule);
	if (nc == NULL) {
		return NULL;
	}

    Py_INCREF(&ncSessionType);
    PyModule_AddObject(nc, "Session", (PyObject *)&ncSessionType);

    datastores = PyDict_New();
    PyModule_AddObject(nc, "Datastores", datastores);

    PyModule_AddStringConstant(nc, "NETCONFv1_0", NETCONF_CAP_BASE10);
    PyModule_AddStringConstant(nc, "NETCONFv1_1", NETCONF_CAP_BASE11);
    PyModule_AddStringConstant(nc, "TRANSPORT_SSH", NETCONF_TRANSPORT_SSH);
    PyModule_AddStringConstant(nc, "TRANSPORT_TLS", NETCONF_TRANSPORT_TLS);

    PyModule_AddIntConstant(nc, "WD_ALL", NCWD_MODE_ALL);
    PyModule_AddIntConstant(nc, "WD_ALL_TAGGED", NCWD_MODE_ALL_TAGGED);
    PyModule_AddIntConstant(nc, "WD_TRIM", NCWD_MODE_TRIM);
    PyModule_AddIntConstant(nc, "WD_MODE_EXPLICIT", NCWD_MODE_EXPLICIT);

    PyModule_AddIntConstant(nc, "RUNNING", NC_DATASTORE_RUNNING);
    PyModule_AddIntConstant(nc, "STARTUP", NC_DATASTORE_STARTUP);
    PyModule_AddIntConstant(nc, "CANDIDATE", NC_DATASTORE_CANDIDATE);

    PyModule_AddIntConstant(nc, "NC_EDIT_DEFOP_NOTSET", NC_EDIT_DEFOP_NOTSET);
    PyModule_AddIntConstant(nc, "NC_EDIT_DEFOP_MERGE", NC_EDIT_DEFOP_MERGE);
    PyModule_AddIntConstant(nc, "NC_EDIT_DEFOP_REPLACE", NC_EDIT_DEFOP_REPLACE);
    PyModule_AddIntConstant(nc, "NC_EDIT_DEFOP_NONE", NC_EDIT_DEFOP_NONE);

    PyModule_AddIntConstant(nc, "NC_EDIT_ERROPT_NOTSET", NC_EDIT_ERROPT_NOTSET);
    PyModule_AddIntConstant(nc, "NC_EDIT_ERROPT_STOP", NC_EDIT_ERROPT_STOP);
    PyModule_AddIntConstant(nc, "NC_EDIT_ERROPT_CONT", NC_EDIT_ERROPT_CONT);
    PyModule_AddIntConstant(nc, "NC_EDIT_ERROPT_ROLLBACK", NC_EDIT_ERROPT_ROLLBACK);

    PyModule_AddIntConstant(nc, "NC_EDIT_TESTOPT_NOTSET", NC_EDIT_TESTOPT_NOTSET);
    PyModule_AddIntConstant(nc, "NC_EDIT_TESTOPT_TESTSET", NC_EDIT_TESTOPT_TESTSET);
    PyModule_AddIntConstant(nc, "NC_EDIT_TESTOPT_SET", NC_EDIT_TESTOPT_SET);
    PyModule_AddIntConstant(nc, "NC_EDIT_TESTOPT_TEST", NC_EDIT_TESTOPT_TEST);

	/* init libnetconf exceptions for use in clb_print() */
	libnetconfError = PyErr_NewExceptionWithDoc("netconf.Error", "Error passed from the underlying libnetconf library.", NULL, NULL);
	Py_INCREF(libnetconfError);
	PyModule_AddObject(nc, "Error", libnetconfError);

	libnetconfWarning = PyErr_NewExceptionWithDoc("netconf.Warning", "Warning passed from the underlying libnetconf library.", PyExc_Warning, NULL);
	Py_INCREF(libnetconfWarning);
	PyModule_AddObject(nc, "Warning", libnetconfWarning);

	return nc;
}
