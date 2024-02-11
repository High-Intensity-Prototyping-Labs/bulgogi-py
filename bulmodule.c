#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include <string.h>
#include <stddef.h>

#include "bulgogi/inc/core.h"

typedef struct {
        PyObject_HEAD
        int number;
} CustomObject;

static PyMemberDef Custom_members[] = {
        {"number", T_INT, offsetof(CustomObject, number), 0, "custom number"},
        {NULL},
};

static PyTypeObject CustomType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "bul.Custom",
        .tp_doc  = PyDoc_STR("A custom object"),
        .tp_basicsize = sizeof(CustomObject),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_new = PyType_GenericNew,
        .tp_members = Custom_members,
};

static PyObject *bul_py_system(PyObject *self, PyObject *args) {
        const char *command;
        int sts;

        if(!PyArg_ParseTuple(args, "s", &command)) {
                return NULL;
        }
        sts = system(command);
        return PyLong_FromLong(sts);
}

static PyObject *bul_py_core_from_file(PyObject *self, PyObject *args) {
        PyObject   *core_py   = NULL;
        PyObject   *dep_list  = NULL;
        PyObject   *dep_str   = NULL;

        size_t     x, y;
        bul_core_s core;
        bul_id_t   dep_id     = BUL_MAX_ID;

        FILE       *file      = NULL;
        const char *filename  = NULL;

        if(!PyArg_ParseTuple(args, "s", &filename)) {
                return NULL;
        }

        if(!(file = fopen(filename, "rb"))) {
                return NULL;
        }

        core = bul_core_from_file(file);

        fclose(file);

        core_py = PyDict_New();

        for(x = 0; x < core.size; x++) {
                dep_list = PyList_New(core.targets[x].size);

                for(y = 0; y < core.targets[x].size; y++) {
                        dep_id = core.targets[x].deps[y];

                        dep_str = PyUnicode_FromString(core.targets[dep_id].name);

                        PyList_SetItem(dep_list, y, dep_str);
                }

                PyDict_SetItemString(core_py, core.targets[x].name, dep_list);

                Py_DecRef(dep_list);
        }

        return core_py;
}

static PyMethodDef BulMethods[] = {
        {"system", bul_py_system, METH_VARARGS, "Execute a shell command."},
        {"core_from_file", bul_py_core_from_file, METH_VARARGS, "Initializes a core from a YAML file."},
        {NULL, NULL, 0, NULL},
};

static struct PyModuleDef bulmodule = {
        PyModuleDef_HEAD_INIT,
        "bulgogi",
        NULL, /* module doc */
        -1,
        BulMethods,
};

PyMODINIT_FUNC PyInit_bulgogi(void) {
        PyObject *m = NULL;

        if(PyType_Ready(&CustomType) < 0) {
                return NULL;
        }

        m = PyModule_Create(&bulmodule);
        if(m == NULL) {
                return NULL;
        }

        Py_INCREF(&CustomType);
        if(PyModule_AddObject(m, "Custom", (PyObject*) &CustomType) < 0) {
                Py_DECREF(&CustomType);
                Py_DECREF(m);
                return NULL;
        }

        return m;
}
