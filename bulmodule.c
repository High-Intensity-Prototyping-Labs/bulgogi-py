#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>

#include "bulgogi/inc/core.h"

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
        bul_core_s core;
        size_t     x, y;
        FILE       *file      = NULL;
        const char *filename  = NULL;
        char       *build_str = NULL;
        PyObject   *dict      = NULL;
        PyObject   *next_str  = NULL;
        PyObject   *next_list = NULL;
        bul_id_t   dep_id     = BUL_MAX_ID;


        if(!PyArg_ParseTuple(args, "s", &filename)) {
                return NULL;
        }

        if(!(file = fopen(filename, "rb"))) {
                return NULL;
        }

        core = bul_core_from_file(file);

        fclose(file);

        dict = PyDict_New();
        for(size_t x = 0; x < core.size; x++) {
                next_list = PyList_New(core.targets[x].size);

                for(size_t y = 0; y < core.targets[x].size; y++) {
                        dep_id = core.targets[x].deps[y];
                        next_str = PyUnicode_FromString(core.targets[dep_id].name);

                        PyList_SetItem(next_list, y, next_str);
                }

                PyDict_SetItemString(dict, core.targets[x].name, next_list);
        }

        return dict;
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
        return PyModule_Create(&bulmodule);
}
