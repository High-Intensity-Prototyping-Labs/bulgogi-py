#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "bulgogi/inc/dot_bul.h"

static PyObject *bul_system(PyObject *self, PyObject *args) {
        const char *command;
        int sts;

        if(!PyArg_ParseTuple(args, "s", &command)) {
                return NULL;
        }
        sts = system(command);
        return PyLong_FromLong(sts);
}

static PyObject *bul_init(PyObject *self, PyObject *args) {
        bul_dot_init();

        Py_INCREF(Py_None);
        return Py_None;
}

static PyMethodDef BulMethods[] = {
        {"system", bul_system, METH_VARARGS, "Execute a shell command."},
        {"init", bul_init, METH_NOARGS, "Initializes the bulgogi project directory."},
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
        return PyModule_Create(&bulmodule);
}
